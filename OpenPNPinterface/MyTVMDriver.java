/*
 * Copyright (C) 2011 Jason von Nieda <jason@vonnieda.org>
 * 
 * This file is part of OpenPnP.
 * 
 * OpenPnP is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * OpenPnP is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with OpenPnP. If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * For more information about OpenPnP visit http://openpnp.org
 *
 * TVM920 handler driver by Glen English glen@cortexrf.com.au
 * OPNP communicates by socket interface with the TVM handler, which in turn, communicates with the low level TVM920MCU PCB 
 */

/*

 */

package org.openpnp.machine.reference.driver;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Queue;
import java.util.Scanner;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Semaphore;
import java.util.regex.Matcher;

import javax.swing.Action;
import javax.swing.Icon;

import org.openpnp.gui.support.PropertySheetWizardAdapter;
import org.openpnp.gui.support.Wizard;
import org.openpnp.machine.reference.ReferenceActuator;
import org.openpnp.machine.reference.ReferenceDriver;
import org.openpnp.machine.reference.ReferenceHead;
import org.openpnp.machine.reference.ReferenceHeadMountable;
import org.openpnp.machine.reference.ReferenceMachine;
import org.openpnp.machine.reference.ReferenceNozzle;
import org.openpnp.machine.reference.ReferencePasteDispenser;
import org.openpnp.machine.reference.driver.GcodeDriver.Axis;
import org.openpnp.model.Configuration;
import org.openpnp.model.LengthUnit;
import org.openpnp.model.Location;
import org.openpnp.spi.Head;
import org.openpnp.spi.HeadMountable;
import org.openpnp.spi.Nozzle;
import org.openpnp.spi.PropertySheetHolder;
import org.pmw.tinylog.Logger;
import org.simpleframework.xml.Attribute;

/**
 * TODO: Consider adding some type of heartbeat to the firmware.
 */
public class MyTVMDriver implements ReferenceDriver, Runnable {

    private static final double minimumRequiredVersion = 0.81;

    @Attribute(required = false)
    private String serverIp = "127.0.0.1";
    @Attribute(required = false)
    private int port = 8888;
    
    private int timeToCompleteCurrentCommand;
    public int tester;
    private static final int NUMNOZZ  = 4;
    private double x, y; /* driver held values */
    private double z [] = new double[NUMNOZZ];
    private double c [] = new double[NUMNOZZ];
    private Socket socket;
    private InputStream input;
    private OutputStream output;
    private Thread readerThread;
    private boolean disconnectRequested;
    private Object sendcmdLock = new Object();

    public Semaphore taskidsema = new Semaphore(1);
    //public Semaphore sendcommandsema = new Semaphore(1);
    public boolean motiondone = false;
    public static  final int Nsemas = 10;
    public static Semaphore[] semaphore = new Semaphore[] {new Semaphore(0),new Semaphore(0),new Semaphore(0),new Semaphore(0),
    		new Semaphore(0),new Semaphore(0),new Semaphore(0),new Semaphore(0),new Semaphore(0),new Semaphore(0  )};
    public int taskidlist [] = new int[Nsemas];
    public int NexttaskId=1; 

    private boolean connected;
    private double connectedVersion;
    //private Queue<String> responseQueue = new ConcurrentLinkedQueue<>();
    private final static int CONNECT_TIMOUT = 5; // 5 second time-out for
                                                 // connection


    
    //private static Scanner in;
    //private static PrintWriter out;

    public MyTVMDriver() {

    }
    
    
    public int SemaPoolGetNextId() throws Exception {
    	taskidsema.acquire();/* lock */
    	
    	int x = NexttaskId;
		NexttaskId++;
		if (NexttaskId >= 2147483646) NexttaskId=1;/* wrap before max unsigned int32 */
    	
		taskidsema.release();
    	return x;
    }
    
    /* semaphore management , return index to use */
    public  int AddSemaToPool(int id) throws Exception {
    	/* add to the list */
    	
    	taskidsema.acquire();/* lock */
    	int x=0;
    	
    	/* look for gap in the pool */
    	for (x = 0  ; x < Nsemas ; x++) {
    		if (taskidlist[x] == 0) {
    			taskidlist[x] = id; /* use this one... */
    			taskidsema.release();

    			return x;/* return a semaphore index */
    		}
    	}
    	/* no room in pool ? */
    	taskidsema.release();
    	throw new Exception("TVM:taskid pool assignment fail");
    }

    public  int LookupAndReleaseSema(int id) throws Exception {
    	
    	taskidsema.acquire();/* lock */ 
    	
    	/* look up to get the index which also is the semaphore array value */
    	int x=0;
    	
    	/* look for it in the pool */
    	for (x = 0  ; x < Nsemas ; x++) {
    		if (taskidlist[x] ==id) {
    			semaphore[x].release();
    			taskidlist[x] = 0;/* assign = now unused */ 
    			taskidsema.release();
    			return 0;
    		}
    	} 
    	taskidsema.release();/* un lock */
    	throw new Exception("TVM:taskid pool no id found");

    }
    
    
    @Override
    public void home(ReferenceHead head) throws Exception {
    	sendCommand("GOHOME");
    	throw new Exception("TVM:wait for machine to be done");
        
    }


    public void DoLocalLocationUpdate() throws Exception {
		sendCommand("GETFAST_XYZA");
		/* force gui update */
	    ReferenceMachine machine = ((ReferenceMachine) Configuration.get().getMachine());
	    for (Head head : Configuration.get().getMachine().getHeads()) {
	        machine.fireMachineHeadActivity(head);
	    }
    }
	
    public int ExtractNumeral(String str) {
        /* get numeral */
        int i = 0;
        while (i < str.length() && !Character.isDigit(str.charAt(i))) i++;
        int j = i;
        while (j < str.length() && Character.isDigit(str.charAt(j))) j++;
        return  Integer.parseInt(str.substring(i, j)); // might be an off-by-1 here    	
    }
    
    public int GetNozIndex(ReferenceHeadMountable hm) throws Exception {
    	/* but all sorts of things come here - TODO */
    	int nozid=0;
    	/* try extracting the index from Name first, and then try id */
    	String sname = hm.getName() ;/* get the nozzle ID */
    	/* now if nozzle, will start with an N */
    	
    	/* special handlinf for each type */
    	if (sname.charAt(0) == 'N') {/* a nozzle */
    		
    	}
    	else if (sname.charAt(0) == 'H') {	/* heads */
    		
    	}
    	else if (sname.charAt(0) == 'C') {/* camera */ 
    		
    	}
    	else if (sname.charAt(0) == 'D') {/* dispenser */ 
    		
    	}
    	else {
    		/* ignore and throw exception*/
    		throw new Exception("TVM:Dont know prefix GetNozIndex with getName="+sname);
    	}
    	
    	try {
    		nozid  = ExtractNumeral(sname);/* set nozid to be zero based  */
    	}
    	catch (Exception e1) {
    		throw new Exception("TVM:unable to convert string in GetNozIndex with getName="+sname);
   		
    	}
    	
        
        if (nozid<0) throw new Exception("TVM:nozid  equals zero. lowest should be N1");
        if (nozid > NUMNOZZ ) throw new Exception("TVM:nozid  range N1 to N4");

        return nozid; /* return 1.. 4 */
    }

    
    
    @Override
    public void moveTo(ReferenceHeadMountable hm, Location location, double speed)
            throws Exception {
        location = location.convertToUnits(LengthUnit.Millimeters);
        location = location.subtract(hm.getHeadOffsets());
        

        int nozid = GetNozIndex(hm);

        
        double x = location.getX();
        double y = location.getY();
        double z = location.getZ();
        double c = location.getRotation();
        
        
        StringBuffer sb = new StringBuffer();
        if (!Double.isNaN(x) && x != this.x) {
            sb.append(String.format(Locale.US, "x=%f ", x));
        }
        if (!Double.isNaN(y) && y != this.y) {
            sb.append(String.format(Locale.US, "y=%f ", y));
        }
        if (!Double.isNaN(z) && z != this.z[nozid-1]) {
            sb.append(String.format(Locale.US, "z%d=%f ", nozid,z));
        }

        if (!Double.isNaN(c) && c != this.c[nozid-1]) {
            sb.append(String.format(Locale.US, "a%d=%f ", nozid,c));
        }
             

        if (sb.length() > 0) {/* is there anythign to do ? */
            if (!Double.isNaN(speed)) {
                sb.append(String.format(Locale.US, "speed=%.0f ", speed * 100.0)); /* convert to percentage 0.100 */
            }
        	//sb.append(String.format(Locale.US, "F%2.2f", feedRateMmPerMinute * speed));
        	motiondone=false;
        	sendCommand("MOVETO_XYZA:"+sb.toString());
        }
        else {
        	Logger.info("Move to - nothing to do ");
        	
        	return;/* nothing to do */
        }
        
        
        if (timeToCompleteCurrentCommand == 0) {
        	/* this means we gave the TVM nothing to do, according to the TVM */
        	return;
        }
        /* now set time that we should be complete */
        int timeout = timeToCompleteCurrentCommand;/* we get that back from TVMdriver */

        long t = System.currentTimeMillis() + timeout;
        
        while (System.currentTimeMillis() < t) {
        	/* poll for completion */
        	DoLocalLocationUpdate();
        	Thread.sleep(50);
        	/* get motion location */
        	
        	
        	sendCommand("LASTDONE?");
        	/* wait */
        	if (motiondone==true) break;
     	
        }
        if (motiondone==true) {
        	
        }
        else {
        	throw new Exception("TVM:machine move_to timeout lastcmd = "+sb + " with timeout = " + timeToCompleteCurrentCommand);
        }
  
        
    }



    private int To_Integer(String id) {
		// TODO Auto-generated method stub
		return 0;
	}



	@Override
    public Location getLocation(ReferenceHeadMountable hm) {
    	/* 
    	 * get location from machine
    	 * 
    	 * */
        int nozid = 1;
        
    	if (!connected) {
    		return new Location(LengthUnit.Millimeters, -1, -1, 0, 0).add(hm.getHeadOffsets());
    	}
  	

        try {
			sendCommand("GETFAST_XYZA");
			/* wait for signal */
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
       

        
        try {
			nozid = GetNozIndex(hm);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return new Location(LengthUnit.Millimeters, x, y, z[nozid-1], c[nozid-1]).add(hm.getHeadOffsets());




        
    }



    @Override
    public void pick(ReferenceNozzle nozzle) throws Exception {
    	
    	/*
        int nozid  = ExtractNumeral(nozzle.getName());    
        StringBuffer sb = new StringBuffer();
        sb.append("NOZZVACCTL STATE=1 ");
        sb.append(String.format(Locale.US, " NOZZN=%d ",nozid) );
        String s = sb.toString();
    	sendCommand(s);
    	*/

    }



    @Override
    public void place(ReferenceNozzle nozzle) throws Exception {
    	/*
        int nozid  = ExtractNumeral(nozzle.getName());    
        StringBuffer sb = new StringBuffer();
        sb.append("NOZZVACCTL STATE=0 ");
        sb.append(String.format(Locale.US, " NOZZN=%d ",nozid) );
        String s = sb.toString();
    	sendCommand(s);
    	*/

    }



    @Override
    public void actuate(ReferenceActuator actuator, boolean on) throws Exception {
    	/* could be vacuum control or feeder control here */
        String actuatorname= actuator.getName();
        int actuatorindex = actuator.getIndex();
       	int state = 0;
       	if (on) state = 1;
       	else state = 0;
       	
        if (actuatorname.startsWith("AutoFeedA")) {
           	

          	sendCommand("FEEDERCTL " + " INDEX= " + actuatorindex +   " STATE=" + state );        	
        	
        }
        else if (actuatorname.startsWith("NozzVacA")) {
            sendCommand("NOZZVACCTL " + " INDEX= " + actuatorindex +   " STATE=" + state );  	
        }
	    else {
	    	throw new Exception("TVM:actuate: unknown actuator name =" + actuatorname);
	    }

       	
       	

    }



    @Override
    public void actuate(ReferenceActuator actuator, double value) throws Exception {
        // TODO Auto-generated method stub
    	tester=2;

    }

    @Override
    public void setEnabled(boolean enabled) throws Exception {
        if (enabled && !connected) {
            connect(serverIp, port);
        }
        if (connected) {
            //sendCommand("set machine " + (enabled ? "on" : "off"));
        }
    }

    public synchronized void connect(String serverIp, int port) throws Exception {
        // disconnect();
        Logger.debug("connect({}, {})", serverIp, port);
        SocketAddress sa = new InetSocketAddress(serverIp, port);
        socket = new Socket();
        socket.connect(sa, CONNECT_TIMOUT * 1000);
        input = socket.getInputStream();
        output = socket.getOutputStream();

        readerThread = new Thread(this);
        readerThread.start();

        sendCommand("HELO");
        /* wait for command to be processed */
        

        if (!connected) {
            throw new Exception(
                    "TVM:Unable to receive connection response from LinuxCNC ver 1.1. Check your server ip and port in machine.xml");
        }

        if (!connected) {
            throw new Exception(String.format(
                    "TVM:Unable to receive connection response from LinuxCNC. Check your server ip and port in machine.xml and that you are running at least version %f of LinuxCNCrsh",
                    minimumRequiredVersion));
        }
        
        DoLocalLocationUpdate();

        
    }




    private  int sendCommand(String command) throws Exception {
    	long timeout = 1000;
    	//if (!connected) throw new Exception(" TVM not connected ");
        synchronized (sendcmdLock) {
        	int taskid = SemaPoolGetNextId();
	        Logger.info("sendCommand({}, {}, {})", command, timeout,taskid);
	        command = command + " TASKID="+taskid + " ";
	        command = command + "\r\n";
	        int SemaphoreIndex = AddSemaToPool(taskid);	   
	        output.write(command.getBytes());
			semaphore[SemaphoreIndex].acquire();	/* now, wait for the semaphore to be released by the inbound consumer */	        
	        /* now wait for return.... */
     

        }
    
        
        return 0;
    }

    public void run() {
        while (!disconnectRequested) {
            String line = readLine().trim(); /* wait for response */
            Logger.debug(line);
            try {
				processResponses(line);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

        }
    }

    private  void processResponses(String response) throws Exception {
    	
    	/* locate the task ID */
    	int taskid=-1;
    	String [] s = response.substring(response.indexOf("TASKID=")+7).split(" ");
    	try {
    		taskid = Integer.parseInt(s[0]);/* parse the returning task ID */
    		}
    	catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    		

    	
        if (response.startsWith("HELO ACK 1.0")) {

            connectedVersion = 1.1;
            connected = true;
            Logger.info(
                    String.format("Connected to TVMhandler Version: %.2f", connectedVersion));

        }
        else if (response.startsWith("GETFAST_XYZA")) {
        	/* decode the response */
        	s = response.substring(response.indexOf("x=")+2).split(" ");
        	x = Double.parseDouble(s[0]);
        	
        	s = response.substring(response.indexOf("y=")+2).split(" ");
        	y = Double.parseDouble(s[0]);	
        	
        	s = response.substring(response.indexOf("z1=")+3).split(" ");
        	z[1-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("z2=")+3).split(" ");
        	z[2-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("z3=")+3).split(" ");
        	z[3-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("z4=")+3).split(" ");
        	z[4-1] = Double.parseDouble(s[0]);	
        	
        	s = response.substring(response.indexOf("a1=")+3).split(" ");
        	c[1-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("a2=")+3).split(" ");
        	c[2-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("a3=")+3).split(" ");
        	c[3-1] = Double.parseDouble(s[0]);	
        	s = response.substring(response.indexOf("a4=")+3).split(" ");
        	c[4-1] = Double.parseDouble(s[0]);	
            	

           
        }
        else if (response.startsWith("MOVETO_XYZA")) {/* get the time to completion.*/
        	s = response.substring(response.indexOf("TIME=")+5).split(" ");
        	timeToCompleteCurrentCommand = Integer.parseInt(s[0]);

        	
        }
        else if (response.startsWith("YESDONE")) {
        	motiondone=true;
        	
        }
        else if (response.startsWith("NOZZVACCTL")) {
        	
        }
        else if (response.startsWith("GOHOME")) {
        	
        }
        Logger.info("LookupAndReleaseSema({})", response);
        LookupAndReleaseSema(taskid);       

    }


    private String readLine() {
        StringBuffer line = new StringBuffer();
        try {
            while (true) {
                int ch = readChar();
                if (ch == -1) {
                    return null;
                }
                else if (ch == '\n' || ch == '\r') {
                    if (line.length() > 0) {
                        return line.toString();
                    }
                }
                else {
                    line.append((char) ch);
                }
            }
        }
        catch (Exception e) {
            Logger.error("readLine()", e);
        }
        return null;
    }

    private int readChar() {
        try {
            int ch = -1;
            while (ch == -1 && !disconnectRequested) {
                ch = input.read();
            }
            return ch;
        }
        catch (Exception e) {
            Logger.error("readChar()", e);
            return -1;
        }
    }
    
    public synchronized void disconnect() {
        disconnectRequested = true;
        connected = false;

        try {
            if (readerThread != null && readerThread.isAlive()) {
                readerThread.join();
            }
            input.close();
            output.close();

            socket.close();
        }
        catch (Exception e) {
            Logger.error("disconnect()", e);
        }

        disconnectRequested = false;
    }

    @Override
    public void close() throws IOException {
        try {
            disconnect();
        }
        catch (Exception e) {
            throw new IOException(e);
        }
    }

    @Override
    public Wizard getConfigurationWizard() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getPropertySheetHolderTitle() {
        return getClass().getSimpleName();
    }

    @Override
    public PropertySheetHolder[] getChildPropertySheetHolders() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public PropertySheet[] getPropertySheets() {
        return new PropertySheet[] {new PropertySheetWizardAdapter(getConfigurationWizard())};
    }

    @Override
    public Action[] getPropertySheetHolderActions() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Icon getPropertySheetHolderIcon() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void dispense(ReferencePasteDispenser dispenser, Location startLocation,
            Location endLocation, long dispenseTimeMilliseconds) throws Exception {
        // TODO Auto-generated method stub

    }


}
 
  