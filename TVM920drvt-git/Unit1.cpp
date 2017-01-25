//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "tvmt.h"
#include "COPNPServer.h"
#include "CTVMcmdQueue.h"
#include "Cmachloc.h"
#include "globs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
CLogHandlerX * logx;
CTVMT  tvmt;




//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	logx = new CLogHandlerX(NULL,true,0);
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa)!=0) {
     	throw("noWSAstartup!");
    }
    HomeAbort = false;

    can = Image1->Canvas;


}



void __fastcall TForm1::LM(AnsiString as) {
    Memo1->Lines->Add(as);
}
void __fastcall TForm1::LM(char * s) {
    Memo1->Lines->Add(AnsiString(s));
}

void __fastcall TForm1::Shutdown(void) {

   tvmt.End();
   opnpsvr.End();


}


//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Shutdown();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Startup(void) {

	/* start socket threads */
    /* TVM920 interface thread is transaction based, but async */

    tvmt.Start();


}
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	Startup();
    Button1->Enabled=false;
    Button2->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
   Shutdown();
   Button2->Enabled=false;
   Button1->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	delete logx;
    logx=NULL;
    WSACleanup();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button3Click(TObject *Sender)
{
	static state = 1;
    const int stdtimeout = 100;
	/* toggle UP light */
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_GPIOCMD_UPLAMP,0);
    p[TVMCMD_DATASTART_BYTEPOS] =  state;
	GUIcmdQueue.Post(stdtimeout);
    LM("state="+IntToStr(state));
    state ^=1;

}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button4Click(TObject *Sender)
{
	static state = 1;
    const int stdtimeout = 100;
	/* toggle UP light */
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_GPIOCMD_DOWNLAMP,0);
    p[TVMCMD_DATASTART_BYTEPOS] =  state;
	GUIcmdQueue.Post(stdtimeout);
    LM("state="+IntToStr(state));
    state ^=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DoPick(int num, int state) {




    GUIcmdQueue.NozzleVacCtl(num,state);
}

void __fastcall TForm1::Button5Click(TObject *Sender)
{
	/* pick 1 */
	static state = 1;
    LM("state="+IntToStr(state));
    DoPick(0,state);
    state ^=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
	/* pick 2 */

	static state = 1;
    LM("state="+IntToStr(state));
    DoPick(1,state);
    state ^=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
	/* pick 3 */

	static state = 1;
    LM("state="+IntToStr(state));
    DoPick(2,state);
    state ^=1;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
	/* pick 4 */

	static state = 1;
    LM("state="+IntToStr(state));
    DoPick(3,state);
    state ^=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
	/* feeder open */

    int feedernumber =  StrToInt(Edit1->Text);
    GUIcmdQueue.FeederCtl(feedernumber,1);
    LM("feeder open "+IntToStr(feedernumber));
}

//---------------------------------------------------------------------------


void __fastcall TForm1::Status1Update(void) {

    CheckBox1->Checked = TVMstats.stats1.NegLimX_1;
    CheckBox2->Checked = TVMstats.stats1.PosLimX_1;
    CheckBox3->Checked = TVMstats.stats1.PosLimY_1;
    CheckBox4->Checked = TVMstats.stats1.PosLimY_2;
    CheckBox5->Checked = TVMstats.stats1.NegLimY_1;
    CheckBox6->Checked = TVMstats.stats1.Nozz1_2_NegZ;
    CheckBox7->Checked = TVMstats.stats1.Nozz3_4_NegZ;

    CheckBox8->Checked = TVMstats.stats1.Pick1Active;
    CheckBox9->Checked = TVMstats.stats1.Pick2Active;
    CheckBox10->Checked = TVMstats.stats1.Pick3Active;
    CheckBox11->Checked = TVMstats.stats1.Pick4Active;
    CheckBox12->Checked = TVMstats.stats1.UpLightActive;
    CheckBox13->Checked = TVMstats.stats1.DownLightActive;
    CheckBox14->Checked = TVMstats.stats1.YellowButton;
    CheckBox15->Checked = TVMstats.stats1.RedButton;
    CheckBox16->Checked = TVMstats.stats1.GreenButton;
    CheckBox17->Checked = TVMstats.stats1.Mystery1;
    CheckBox18->Checked = TVMstats.stats1.Mystery2;
    CheckBox19->Checked = TVMstats.stats1.Mystery3;
}

AnsiString __fastcall TForm1::TVMmotionstate(int i) {
    switch (i) {
        case TVM920_MOTION_STATUS_STOPPED :
        	return "stopped";
        case TVM920_MOTION_STATUS_OTHER :
        	return "other";
        case TVM920_MOTION_STATUS_TRAVEL :
        	return "moving";
        case TVM920_MOTION_STATUS_DECEL :
        	return "decel";
        default :
        	return "undefined";
    }

}

void __fastcall TForm1::Status2Update(void) {
	Edit2->Text=IntToStr(TVMstats.loctik.x);
    Edit3->Text=IntToStr(TVMstats.loctik.y);

    float x = StrToFloat(TVMstats.loctik.x) / TVMinfo.xticks_per_mm;
    float y = StrToFloat(TVMstats.loctik.y) / TVMinfo.yticks_per_mm;

    char s[64];
    sprintf(s,"%.3f",x);
    Edit15->Text = AnsiString(s);
    StatusBar1->Panels->Items[1]->Text = "X="+  Edit15->Text ;
    sprintf(s,"%.3f",y);
    Edit16->Text = AnsiString(s);
    StatusBar1->Panels->Items[2]->Text = "Y="+  Edit16->Text ;

    Label5->Caption = TVMmotionstate(TVMstats.motionstate.xstate);
    Label6->Caption = TVMmotionstate(TVMstats.motionstate.ystate);


	Edit4->Text=IntToStr(TVMstats.loctik.z[0]);
    Edit26->Text=FloatToStr(TVMstats.locdub.z[0]);
    Label11->Caption = TVMmotionstate(TVMstats.motionstate.z_state[0]);
	Edit5->Text=IntToStr(TVMstats.loctik.z[1]);
    Edit27->Text=FloatToStr(TVMstats.locdub.z[1]);
    Label12->Caption = TVMmotionstate(TVMstats.motionstate.z_state[1]);
	Edit6->Text=IntToStr(TVMstats.loctik.z[2]);
    Edit28->Text=FloatToStr(TVMstats.locdub.z[2]);
    Label13->Caption = TVMmotionstate(TVMstats.motionstate.z_state[2]);
	Edit7->Text=IntToStr(TVMstats.loctik.z[3]);
    Edit29->Text=FloatToStr(TVMstats.locdub.z[3]);
    Label14->Caption = TVMmotionstate(TVMstats.motionstate.z_state[3]);


	Edit8->Text=IntToStr(TVMstats.loctik.a[0]);
	Edit31->Text=FloatToStr(TVMstats.locdub.a[0]);
    Label19->Caption = TVMmotionstate(TVMstats.motionstate.a_state[0]);
	Edit9->Text=IntToStr(TVMstats.loctik.a[1]);
	Edit32->Text=FloatToStr(TVMstats.locdub.a[1]);
    Label20->Caption = TVMmotionstate(TVMstats.motionstate.a_state[1]);
	Edit10->Text=IntToStr(TVMstats.loctik.a[2]);
	Edit33->Text=FloatToStr(TVMstats.locdub.a[2]);
    Label21->Caption = TVMmotionstate(TVMstats.motionstate.a_state[2]);
	Edit11->Text=IntToStr(TVMstats.loctik.a[3]);
	Edit34->Text=FloatToStr(TVMstats.locdub.a[3]);
    Label22->Caption = TVMmotionstate(TVMstats.motionstate.a_state[3]);
}

void __fastcall TForm1::Status3Update(void) {
	/* update feeders */
    FillCanvasBlack();
    UpdateFeederCanvas(TVMstats.ReadFeederStatus());
    ShowTrajectory(TVMstats.trajectory_start,TVMstats.trajectory_end);

}

void __fastcall TForm1::UpdateStartupGUI(void) {
	Edit22->Text = FloatToStr(TVMinfo.xticks_per_mm);
	Edit23->Text = FloatToStr(TVMinfo.yticks_per_mm);
    Button40Click(NULL);
}

void __fastcall TForm1::AppMessage(tagMSG &Msg, bool &Handled)
{
	static int8_t statusupdatecounter = 65;/* just a abcdefg counter */
    if (Msg.message < WM_USER) return;
  	switch (Msg.message) {
		case   UMTVM_STATUSUPDATE :
        	Status1Update();
        	Status2Update();
        	StatusBar1->Panels->Items[0]->Text = AnsiString((char)statusupdatecounter)+AnsiString((char)statusupdatecounter)+AnsiString((char)statusupdatecounter);
            if (++statusupdatecounter > (65+26))  statusupdatecounter = 65;
   	       	//Handled = true;
        	break;
		case   UMTVM_INITIALIZE_DONE :
        	UpdateStartupGUI();
            //Handled = true;

            break;
		case   UMTVM_GOHOME :
        	GoHome();
            //Handled = true;
            break;
        case   UMTVM_FEEDER_STATUS_UPDATE :
           Status3Update();
           //Handled = true;
           break;
        case  UMTVM_TRAJECTORY_UPDATE  :
           Status3Update();
           break;
   		default :

        	break;
   }
   /* for all other messages, Handled remains False so that other message handlers can respond */

}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	Application->OnMessage = AppMessage;
    FillCanvasBlack();



}
//---------------------------------------------------------------------------



void __fastcall TForm1::MoveXYZA(uint32_t axismask, int x, int y, int z[TVM920_NNOZZ], int a[TVM920_NNOZZ]) {


    int speed = 0;
    if (Button30->Caption == "SLOW") speed = 0;
    else if (Button30->Caption == "FAST") speed = 1;
    else if (Button30->Caption == "WARP") speed = 2;

    Cmachloc<int32_t>  loci(TVM920_NNOZZ);
    loci.x  = x;
    loci.y = y;

    int n = TVM920_NNOZZ;
    while (n--) {
    	loci.a[n] = a[n];
    	loci.z[n] = z[n];
    }
    GUIcmdQueue.MoveXYZA(loci,axismask,speed);


}

void __fastcall TForm1::MoveXYincrement(int isX, int isY) {
	// jog -y

    int32_t machx, machy;
    machx =  StrToInt(Edit13->Text);
    machy =  StrToInt(Edit14->Text);

    machx += isX * StrToInt(Edit12->Text);
    machy += isY * StrToInt(Edit12->Text);
    Edit13->Text=IntToStr(machx);
    Edit14->Text=IntToStr(machy);

    int z[TVM920_NNOZZ];int a[TVM920_NNOZZ] ;
    MoveXYZA((1<<TVMCMD_MOVEXYZA_X_BIT ) | (1<<TVMCMD_MOVEXYZA_Y_BIT),
    	machx,machy,z,a);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button30Click(TObject *Sender)
{

    if ( Button30->Caption =="SLOW") {
    	Button30->Caption="FAST";
    }
    else if ( Button30->Caption =="FAST") {
    	Button30->Caption="WARP";
    }
    else if ( Button30->Caption =="WARP") {
    	Button30->Caption="SLOW";
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button11Click(TObject *Sender)
{
	/* +Y */
   	MoveXYincrement(0,1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button10Click(TObject *Sender)
{
	/* -Y */
   	MoveXYincrement(0,-1);
}
void __fastcall TForm1::Button13Click(TObject *Sender)
{
	/* -X */
   	MoveXYincrement(-1,0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button12Click(TObject *Sender)
{
	/* +X */
   	MoveXYincrement(1,0);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Memo1DblClick(TObject *Sender)
{
	Memo1->Lines->SaveToFile("memo1.txt");
    LM("SAVED MEMO to file memo1.txt");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ForceAzero(void) {     /* set the machine so it knows where it is */
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_SETXYZA,0);



    *((uint32_t *) &p[TVMCMD_MOVEXYZA_INFO_BYTEPOS]) =  0 ;
    *((uint32_t *) &p[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]) =
    	(1<<TVMCMD_MOVEXYZA_A1_BIT ) | (1<<TVMCMD_MOVEXYZA_A2_BIT)|
        	(1<<TVMCMD_MOVEXYZA_A3_BIT ) | (1<<TVMCMD_MOVEXYZA_A4_BIT);


     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A1_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A2_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A3_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A4_BYTEPOS]) = 0;

    LM("sending : ForceZzero");

	GUIcmdQueue.Post(stdtimeout);

}


void __fastcall TForm1::ForceZzero(void) {     /* set the machine so it knows where it is */
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_SETXYZA,0);



    *((uint32_t *) &p[TVMCMD_MOVEXYZA_INFO_BYTEPOS]) =  0 ;
    *((uint32_t *) &p[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]) =
    	(1<<TVMCMD_MOVEXYZA_Z1_BIT ) | (1<<TVMCMD_MOVEXYZA_Z2_BIT)|
        	(1<<TVMCMD_MOVEXYZA_Z3_BIT ) | (1<<TVMCMD_MOVEXYZA_Z4_BIT);
    *((int32_t *) &p[TVMCMD_MOVEXYZA_X_BYTEPOS]) = 0;
    *((int32_t *) &p[TVMCMD_MOVEXYZA_Y_BYTEPOS]) = 0;

     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS]) = 0;

    LM("sending : ForceZzero");

	GUIcmdQueue.Post(stdtimeout);

}


void __fastcall TForm1::ForceXY(void) {     /* set the machine so it knows where it is */
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_SETXYZA,0);


    /* figure out ticks from distance  */

    double x  = StrToFloat(Edit15->Text);
    double y =  StrToFloat(Edit16->Text);

    x *= TVMinfo.xticks_per_mm;
    y *= TVMinfo.yticks_per_mm;

    *((uint32_t *) &p[TVMCMD_MOVEXYZA_INFO_BYTEPOS]) =  0 ;
    *((uint32_t *) &p[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]) = (1<<TVMCMD_MOVEXYZA_X_BIT ) | (1<<TVMCMD_MOVEXYZA_Y_BIT) ;
    *((int32_t *) &p[TVMCMD_MOVEXYZA_X_BYTEPOS]) = x;
    *((int32_t *) &p[TVMCMD_MOVEXYZA_Y_BYTEPOS]) = y;

     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS]) = 0;
     *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS]) = 0;

    LM("sending : ForceXY");

	GUIcmdQueue.Post(stdtimeout);

}

void __fastcall TForm1::Button31Click(TObject *Sender)
{
	ForceXY();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::SendMotionZeroMotion(void) {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_MOTIONZEROMOTION,0);
    LM("sending : SendMotionStopMotion");
 	GUIcmdQueue.Post(stdtimeout);
}

void __fastcall TForm1::Button32Click(TObject *Sender)
{
	SendMotionZeroMotion();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TVMSetEnd_Stop_MotionEnables(uint32_t axis, uint32_t disabled) {

    const int stdtimeout = 100;
    if (disabled) LM("defeating end stop Y2 ");
    else     LM("enabling end stop Y2 ");
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_SET_DEFEAT_END_STOPS,0);
    *((uint32_t *) & p[TVMCMD_DATASTART_BYTEPOS+0]) = axis ;
    *((uint32_t *) & p[TVMCMD_DATASTART_BYTEPOS+4]) = disabled ? 1 : 0;
    LM("TVMSetEnd_Stop_MotionEnables =" + IntToHex((int) axis,(int)2) + " disable=" +  IntToStr((int)disabled));
	GUIcmdQueue.Post(stdtimeout);
}

void __fastcall TForm1::Button33Click(TObject *Sender)
{
	/* ENable Y stop - normal operation ?*/
	TVMSetEnd_Stop_MotionEnables((1<<TVMCMD_MOVEXYZA_Y_BIT),1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button34Click(TObject *Sender)
{
	/* disable Y stop on first Y*/
	TVMSetEnd_Stop_MotionEnables((1<<TVMCMD_MOVEXYZA_Y_BIT),0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SetGPIOdirect(int bitnum, int state01) {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_GPIOCMD_SRBIT,0);
    p[TVMCMD_DATASTART_BYTEPOS+0] = bitnum ;
    p[TVMCMD_DATASTART_BYTEPOS+1] = state01 ? 1 : 0;
    LM("SetGPIOdirect =" + IntToHex((int) bitnum,(int)2) + " state=" +  IntToStr((int)state01));
	GUIcmdQueue.Post(stdtimeout);

}

void __fastcall TForm1::Button35Click(TObject *Sender)
{
	SetGPIOdirect(StrToInt(Edit17->Text),StrToInt(Edit18->Text));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button36Click(TObject *Sender)
{
	/* goto XY */
    int32_t machx =  StrToInt(Edit13->Text);
    int32_t machy =  StrToInt(Edit14->Text);
    int z[TVM920_NNOZZ];int a[TVM920_NNOZZ] ;
    MoveXYZA( (1<<TVMCMD_MOVEXYZA_X_BIT ) | (1<<TVMCMD_MOVEXYZA_Y_BIT) ,machx,machy,z,a);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NozzleSetZ(int nozzle,  int value ) {
    int z[TVM920_NNOZZ];
    int a[TVM920_NNOZZ];


    memset((void *) z, 0, sizeof(z));
    memset((void *) a, 0, sizeof(a));

    z[nozzle] = value;

    switch (nozzle) {
        case 0 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z1_BIT) ,0,0,z,a);
        	break;
        case 1:
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z2_BIT) ,0,0,z,a);
        	break;
        case 2:
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z3_BIT) ,0,0,z,a);
        	break;
        case 3 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z4_BIT) ,0,0,z,a);

        	break;
        default :
        	break;
    }


}
void __fastcall TForm1::NozzlePlungeZ(int nozzle, int pos, bool IsTicks ) {
    int z[TVM920_NNOZZ];
    int a[TVM920_NNOZZ];


    memset((void *) z, 0, sizeof(z));
    memset((void *) a, 0, sizeof(a));
    int plunge;

    if (IsTicks) {
        plunge = StrToInt(Edit19->Text);
        z[nozzle] = pos < 0 ? -plunge : 0;
    }
    else {
        double plungedub = StrToFloat(Edit30->Text);
        double homeRotOffset =  (nozzle & 0x2 ) ?  TVMinfo.z34_HomeOffsetDegrees : TVMinfo.z12_HomeOffsetDegrees;
        z[nozzle] = pos < 0 ? TVMinfo.xlatemmToZticks(plungedub,homeRotOffset)  : 0;
    }
    switch (nozzle) {
        case 0 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z1_BIT) ,0,0,z,a);
        	break;
        case 1:
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z2_BIT) ,0,0,z,a);
        	break;
        case 2:
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z3_BIT) ,0,0,z,a);
        	break;
        case 3 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_Z4_BIT) ,0,0,z,a);

        	break;
        default :
        	break;
    }


}




void __fastcall TForm1::Button14Click(TObject *Sender)
{
	NozzlePlungeZ(0,1,CheckBox24->Checked);	/* nozzle 1 up */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button15Click(TObject *Sender)
{
	NozzlePlungeZ(0,-1,CheckBox24->Checked);	/* nozzle 1 down */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button16Click(TObject *Sender)
{
	NozzlePlungeZ(1,1,CheckBox24->Checked);	/* nozzle 2 up */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button17Click(TObject *Sender)
{
	NozzlePlungeZ(1,-1,CheckBox24->Checked);	/* nozzle 2 down */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button18Click(TObject *Sender)
{
	NozzlePlungeZ(2,1,CheckBox24->Checked);	/* nozzle 3 up */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button19Click(TObject *Sender)
{
	NozzlePlungeZ(2,-1,CheckBox24->Checked);	/* nozzle 3 down */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button20Click(TObject *Sender)
{
	NozzlePlungeZ(3,1,CheckBox24->Checked);	/* nozzle 4 up */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button21Click(TObject *Sender)
{
	NozzlePlungeZ(3,-1,CheckBox24->Checked);	/* nozzle 4 down */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NozzleA(int nozzle,int useticks) {
    int z[TVM920_NNOZZ];
    int a[TVM920_NNOZZ];


    memset((void *) z, 0, sizeof(z));
    memset((void *) a, 0, sizeof(a));
    int rot;

    if (useticks) {
        rot = StrToInt(Edit20->Text);

    }
    else {
        double rotationdub = StrToFloat(Edit35->Text);
        rot = TVMinfo.xlateADegToAticks(rotationdub);
    }

    a[nozzle] = rot;


    switch (nozzle) {
        case 0 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_A1_BIT) ,0,0,z,a);
        	break;
        case 1 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_A2_BIT) ,0,0,z,a);
        	break;
        case 2 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_A3_BIT) ,0,0,z,a);
        	break;
        case 3 :
        	MoveXYZA(  (1<<TVMCMD_MOVEXYZA_A4_BIT) ,0,0,z,a);
        	break;
        default :
        	return;
    }
}


//---------------------------------------------------------------------------

void __fastcall TForm1::Button23Click(TObject *Sender)
{
	NozzleA(0,CheckBox24->Checked);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TForm1::Button25Click(TObject *Sender)
{
	NozzleA(1,CheckBox24->Checked);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TForm1::Button27Click(TObject *Sender)
{
	NozzleA(2,CheckBox24->Checked);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button29Click(TObject *Sender)
{
	NozzleA(3,CheckBox24->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button37Click(TObject *Sender)
{
	/* read register */
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_READREGISTER,0);
    uint32_t reg;

    sscanf(AnsiString(Edit21->Text).c_str(),"%x",&reg);
    LM("Read Register =" + IntToHex((int) reg,(int)8));
    *((uint32_t *) &p[TVMCMD_DATASTART_BYTEPOS+0]) = reg;
	GUIcmdQueue.Post(stdtimeout);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button38Click(TObject *Sender)
{
	Button38->Enabled = false;
	Button39->Enabled = true;
	opnpsvr.Start();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button39Click(TObject *Sender)
{

	opnpsvr.End();
	Button38->Enabled = true;
	Button39->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button40Click(TObject *Sender)
{
	/* copy from machine status ticks to motion control ticks */
	Edit13->Text = Edit2->Text;
	Edit14->Text = Edit3->Text;

    /* copy location in */
	Edit24->Text = Edit15->Text;
	Edit25->Text = Edit16->Text;

}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button41Click(TObject *Sender)
{
	/* goto XY absolute in millimeters */

    /* get from edit box */
    double x =  StrToFloat(Edit24->Text);
    double y =  StrToFloat(Edit25->Text);

    int32_t machx,machy;

    /* convert to ticks */
    machx =  x * TVMinfo.xticks_per_mm;
    machy =  y * TVMinfo.yticks_per_mm;

    /* copy into the box */
    Edit13->Text = IntToStr(machx);
    Edit14->Text = IntToStr(machy);

    Button36Click(NULL);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::AxisControlMuteClick(void) {
	globs.AxisMotionMute = ~0;
    globs.AxisMotionMute ^= ( (CheckBox20->Checked) <<TVMCMD_MOVEXYZA_X_BIT );
    globs.AxisMotionMute ^= ( (CheckBox21->Checked) <<TVMCMD_MOVEXYZA_Y_BIT );


    globs.AxisMotionMute ^=  ((CheckBox22->Checked) <<TVMCMD_MOVEXYZA_Z1_BIT ) | ((CheckBox22->Checked) <<TVMCMD_MOVEXYZA_Z2_BIT);
    globs.AxisMotionMute ^=  ((CheckBox22->Checked) <<TVMCMD_MOVEXYZA_Z3_BIT ) | ((CheckBox22->Checked) <<TVMCMD_MOVEXYZA_Z4_BIT);



    globs.AxisMotionMute ^= ( (CheckBox23->Checked) <<TVMCMD_MOVEXYZA_A1_BIT );
    globs.AxisMotionMute ^= ( (CheckBox23->Checked) <<TVMCMD_MOVEXYZA_A2_BIT );
    globs.AxisMotionMute ^= ( (CheckBox23->Checked) <<TVMCMD_MOVEXYZA_A3_BIT );
    globs.AxisMotionMute ^= ( (CheckBox23->Checked) <<TVMCMD_MOVEXYZA_A4_BIT );


}

void __fastcall TForm1::CheckBox20Click(TObject *Sender)
{
	AxisControlMuteClick();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox21Click(TObject *Sender)
{
	AxisControlMuteClick();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox22Click(TObject *Sender)
{
	AxisControlMuteClick();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox23Click(TObject *Sender)
{
	AxisControlMuteClick();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::HomeX(int step1, int step2) {
	int z[TVM920_NNOZZ];int a[TVM920_NNOZZ] ;
    int xaxis=TVMstats.loctik.x;/* get current */
    LM("go -X if required");              //PosLimX_1
    while ((TVMstats.stats1.PosLimX_1)&&(!HomeAbort)) {
    		Application->ProcessMessages();
    		MoveXYZA( (1<<TVMCMD_MOVEXYZA_X_BIT ) ,xaxis,0,z,a);
            Sleep(100);
            while (TVMstats.motionstate.ystate != TVM920_MOTION_STATUS_STOPPED) {
            	Application->ProcessMessages();
                Sleep(10);
            }
            xaxis=TVMstats.loctik.x;
            xaxis-=step1;
    }
    LM("going +X....");

    do {
    	/* move more */
        Application->ProcessMessages();

        MoveXYZA( (1<<TVMCMD_MOVEXYZA_X_BIT) ,xaxis,0,z,a);
        Sleep(100);
        while (TVMstats.motionstate.ystate!=TVM920_MOTION_STATUS_STOPPED) {
            Application->ProcessMessages();
            Sleep(10);
        }
        xaxis=TVMstats.loctik.x;
        xaxis+=step2;

    } while ((! TVMstats.stats1.PosLimX_1)&&(!HomeAbort)) ;

}

void __fastcall TForm1::HomeY(int step1) {


	LM("Y homing ");

    int z[TVM920_NNOZZ];int a[TVM920_NNOZZ] ;
    int YSTEP = 1024;
    int yaxis=TVMstats.loctik.y;/* get current */
    LM("go -Y FAST");
    int OptoInterrupted;
    /* if already home , come back a bit */

    /* first , if we havea stop valid, step go back until anythign stops   */

    SetGPIOdirect(TVM920_GPIO_CMD_Y1MOTOR,0);/* permit motorY1 */
    SetGPIOdirect(TVM920_GPIO_CMD_Y2MOTOR,0);/* permit motor Y2 */
    WriteRegister(TVM920_RW_REG_ENDSTOPDEFEAT_Y,0x00000000);

    Button30->Caption = "SLOW";

	/* stops - found at Pete's shop :
	
	
	*/

    while (!HomeAbort) {

    		Application->ProcessMessages();

    		OptoInterrupted  = TVMstats.stats1.PosLimY_1 || TVMstats.stats1.PosLimY_2;

            if (!OptoInterrupted) break;

    		MoveXYZA( (1<<TVMCMD_MOVEXYZA_Y_BIT) ,0,yaxis,z,a);
            Sleep(100);
            while (TVMstats.motionstate.ystate != TVM920_MOTION_STATUS_STOPPED) {
            	Application->ProcessMessages();
                Sleep(10);
            }
            yaxis=TVMstats.loctik.y;
            yaxis-=YSTEP;
    }


    /* now go back into either stop */
    LM("going +Y....FAST ");
    Button30->Caption = "FAST";
    YSTEP = step1;

    do {
    	/* move lsowly */

        Application->ProcessMessages();
		OptoInterrupted  = TVMstats.stats1.PosLimY_1 || TVMstats.stats1.PosLimY_2;
        if (OptoInterrupted) break;

        MoveXYZA( (1<<TVMCMD_MOVEXYZA_Y_BIT) ,0,yaxis,z,a);
        Sleep(100);
        while (TVMstats.motionstate.ystate!=TVM920_MOTION_STATUS_STOPPED) {
            Application->ProcessMessages();
            Sleep(10);
        }
        yaxis=TVMstats.loctik.y;
        yaxis+=YSTEP;

    } while (!HomeAbort) ;

    /* now come out a bit slowly and go back in again */
    LM("back neg again..until both stops clear. ");

    YSTEP = 1024;
    Button30->Caption = "SLOW";
    while (!HomeAbort) {

    		Application->ProcessMessages();

    		OptoInterrupted  = TVMstats.stats1.PosLimY_1 || TVMstats.stats1.PosLimY_2;

            if (!OptoInterrupted) break;

    		MoveXYZA( (1<<TVMCMD_MOVEXYZA_Y_BIT) ,0,yaxis,z,a);
            Sleep(100);
            while (TVMstats.motionstate.ystate != TVM920_MOTION_STATUS_STOPPED) {
            	Application->ProcessMessages();
                Sleep(10);
            }
            yaxis=TVMstats.loctik.y;
            yaxis-=YSTEP;
    }


    /* now go back into either stop SLOW */
    LM("going +Y....until a stop hits");
    YSTEP = 512;

    do {
    	/* move lsowly */

        Application->ProcessMessages();
		OptoInterrupted  = TVMstats.stats1.PosLimY_1 || TVMstats.stats1.PosLimY_2;
        if (OptoInterrupted) break;

        MoveXYZA( (1<<TVMCMD_MOVEXYZA_Y_BIT) ,0,yaxis,z,a);
        Sleep(100);
        while (TVMstats.motionstate.ystate!=TVM920_MOTION_STATUS_STOPPED) {
            Application->ProcessMessages();
            Sleep(10);
        }
        yaxis=TVMstats.loctik.y;
        yaxis+=YSTEP;

    } while (!HomeAbort) ;




    LM("Remove ASYM");
    /* assymetrry removal */
    /* ok, now figure out which stop tripped.... */
    bool PosY1tripped=false;
    bool PosY2tripped = false;


    if  (TVMstats.stats1.PosLimY_1)  PosY1tripped=true;

    if (TVMstats.stats1.PosLimY_2) PosY2tripped=true;

    /* pull the other motor in.... */
    if (PosY1tripped && PosY2tripped ) {
    	/* both just right - do nothign to axis ssymetrty */
       	LM("Y1 and Y2 TRIPPED- nothing to do....");
    }
    else if   (PosY1tripped) {
        /* if Y1 stop went first, we must disable Y1 and keep pulling until Y2 */
        SetGPIOdirect(TVM920_GPIO_CMD_Y1MOTOR,1);/* kill Y1 */
        LM("Y1  TRIPPED - kill Y1, allow Y1");
        WriteRegister(TVM920_RW_REG_ENDSTOPDEFEAT_Y,0x01000000);  	/* permit beyond Y1 */
    }
    else {  /* pos 2 tripped */
        SetGPIOdirect(TVM920_GPIO_CMD_Y2MOTOR,1);/* kill Y2 */
        LM("Y2  TRIPPED - kill Y2, allow Y2");
        WriteRegister(TVM920_RW_REG_ENDSTOPDEFEAT_Y,0x00000000);  	/* permit beyond Y2 */
    }

    if (PosY1tripped && PosY2tripped ) {
         /* no need to do anything */
    }
    else {

        YSTEP = 512;

        LM("going +Y...");
        do {
            /* move lsowly until BOTH lit */

            Application->ProcessMessages();
            OptoInterrupted  = TVMstats.stats1.PosLimY_1 && TVMstats.stats1.PosLimY_2;
            if (OptoInterrupted) break;

            MoveXYZA( (1<<TVMCMD_MOVEXYZA_Y_BIT) ,0,yaxis,z,a);
            Sleep(100);
            while (TVMstats.motionstate.ystate!=TVM920_MOTION_STATUS_STOPPED) {
                Application->ProcessMessages();
                Sleep(10);
            }
            yaxis=TVMstats.loctik.y;
            yaxis+=YSTEP;

        } while (!HomeAbort) ;
    }




    SetGPIOdirect(TVM920_GPIO_CMD_Y1MOTOR,0);/* permit motorY1 */
    SetGPIOdirect(TVM920_GPIO_CMD_Y2MOTOR,0);/* permit motor Y2 */
    WriteRegister(TVM920_RW_REG_ENDSTOPDEFEAT_Y,0x00000000);
    LM("Y done.");
}

void __fastcall TForm1::HomeZ(void) {
	Button30->Caption = "SLOW";
    const int NOZZSTEP = 1024;

    int nozzZ1=TVMstats.loctik.z[0];/* get current */
    int nozzZ2=TVMstats.loctik.z[1];/* get current */
    int nozzZ = -nozzZ2+nozzZ1;

    LM("testing end state12");
    while ((TVMstats.stats1.Nozz1_2_NegZ) && (!HomeAbort)) { /* while is -Z IE nozzle 2 of 4 is down a bit */
    		Application->ProcessMessages();

        	if (nozzZ < 0) NozzleSetZ(0,nozzZ);
            else  NozzleSetZ(1,-nozzZ);   /* deal with CAM   for homing- normally these do not take pos values */

            Sleep(100);
            while (TVMstats.motionstate.z_state[0] != TVM920_MOTION_STATUS_STOPPED) {
            	Application->ProcessMessages();
                Sleep(10);
            }
            Application->ProcessMessages();
			nozzZ1=TVMstats.loctik.z[0];/* get current */
			nozzZ2=TVMstats.loctik.z[1];/* get current */
            nozzZ = -nozzZ2+nozzZ1;
            LM("NozzZ="+IntToStr((int)nozzZ));
            nozzZ-=NOZZSTEP;

    }
    LM("going back");
    do {
    	/* move lsowly */
        Application->ProcessMessages();
        if (nozzZ < 0) NozzleSetZ(0,nozzZ);
        else  NozzleSetZ(1,-nozzZ);   /* deal with CAM   for homing- normally these do not take pos values */
        Sleep(100);
        while (TVMstats.motionstate.z_state[0]!=TVM920_MOTION_STATUS_STOPPED) {
            Application->ProcessMessages();
            Sleep(10);
        }
        Application->ProcessMessages();
			nozzZ1=TVMstats.loctik.z[0];/* get current */
			nozzZ2=TVMstats.loctik.z[1];/* get current */
            nozzZ = -nozzZ2+nozzZ1;
            LM("NozzZ="+IntToStr((int)nozzZ));
        nozzZ+=NOZZSTEP;

    } while ((! TVMstats.stats1.Nozz1_2_NegZ)&&(!HomeAbort)) ;
    LM("Z1-2 done.");

    /* do Z3-4  */
    int nozzZ3=TVMstats.loctik.z[2];/* get current */
    int nozzZ4=TVMstats.loctik.z[3];/* get current */
    nozzZ = -nozzZ4+nozzZ3;
    LM("testing end state34");
    while ((TVMstats.stats1.Nozz3_4_NegZ)&& (!HomeAbort)) { /* while is -Z IE nozzle 2 of 4 is down a bit */
    		Application->ProcessMessages();
        	if (nozzZ < 0) NozzleSetZ(2,nozzZ);
        	else  NozzleSetZ(3,-nozzZ);   /* deal with CAM   for homing- normally these do not take pos value */
            Sleep(100);
            while (TVMstats.motionstate.z_state[3] != TVM920_MOTION_STATUS_STOPPED) {
            	Application->ProcessMessages();
                Sleep(10);
            }
            Application->ProcessMessages();
    		nozzZ3=TVMstats.loctik.z[2];/* get current */
    		nozzZ4=TVMstats.loctik.z[3];/* get current */
    		nozzZ = -nozzZ4+nozzZ3;
            nozzZ+=NOZZSTEP;
    }
    LM("going back");  /* now, negZ is OFF */
    do {
    	/* move lsowly */
        Application->ProcessMessages();
        if (nozzZ < 0) NozzleSetZ(2,nozzZ);
        else  NozzleSetZ(3,-nozzZ);   /* deal with CAM   for homing- normally these do not take pos value */
        Sleep(100);
        while (TVMstats.motionstate.z_state[3]!=TVM920_MOTION_STATUS_STOPPED) {
            Application->ProcessMessages();
            Sleep(10);
        }
        Application->ProcessMessages();
        nozzZ3=TVMstats.loctik.z[2];/* get current */
        nozzZ4=TVMstats.loctik.z[3];/* get current */
        nozzZ = -nozzZ4+nozzZ3;
        nozzZ-=NOZZSTEP;

    } while ((! TVMstats.stats1.Nozz3_4_NegZ)&& (!HomeAbort)) ;
    LM("Z3-4 done.");
    if (!HomeAbort)  ForceZzero();
}

void __fastcall TForm1::GoHome(void) {
	/* quick and dirty home */

    /* first get home for Z */
    /* first go down enough until goes non ZERO */
    HomeZ();
    /* set nozzle Z zeros */

    LM("Y stage 1");

    HomeY(1048576);


    LM("Y done.");

    Button30->Caption = "FAST";
    HomeX(1048,1048576);
    LM("X stage 2");
    Button30->Caption = "SLOW";
    HomeX(1024,1024);
    LM("X done.");

    Edit15->Text = FloatToStr(TVMinfo.x_pos_home);
    Edit16->Text = FloatToStr(TVMinfo.y_pos_home);


    if (HomeAbort)  {
    	HomeAbort=false;
        LM("HomeAbort");
        return ;
    }
    ForceXY();
    Sleep(100);
    Application->ProcessMessages();
    Button40Click(NULL);
    LM("*** ready *** ");



}
void __fastcall TForm1::Button42Click(TObject *Sender)
{
	Button42->Enabled=false;
    GoHome();
    Button42->Enabled=true;
    LM("*** ready *** ");
    LM("*** ready *** ");
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button43Click(TObject *Sender)
{
	/* sets scale */
    double x = StrToFloat(Edit22->Text);
    double y = StrToFloat(Edit23->Text);
    TVMinfo.xticks_per_mm = x;
    TVMinfo.yticks_per_mm = y;
    UpdateStartupGUI();


}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button44Click(TObject *Sender)
{
	HomeZ();
    if (HomeAbort)  {
    	HomeAbort=false;
        LM("HomeAbort");
        return ;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button45Click(TObject *Sender)
{
	HomeAbort=true;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button46Click(TObject *Sender)
{
	ForceAzero();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button22Click(TObject *Sender)
{
    uint32_t reg;
    int32_t data;
    sscanf(AnsiString(Edit21->Text).c_str(),"%x",&reg);
    sscanf(AnsiString(Edit36->Text).c_str(),"%x",&data);
    WriteRegister(reg,data);
}
void __fastcall TForm1::WriteRegister(uint32_t reg, int32_t data) {

	/* write register */
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GUIcmdQueue.GetPtrToNextBufferNice(TVMCMD_WRITEREGISTER,0);


    *((uint32_t *) &p[TVMCMD_DATASTART_BYTEPOS+0]) = reg;
    *((int32_t *) &p[TVMCMD_DATASTART_BYTEPOS+4]) = data;
    LM("Write Register @" + IntToHex((int) reg,(int)8) + " : "+IntToHex((int) data,(int)8) );
	GUIcmdQueue.Post(stdtimeout);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button24Click(TObject *Sender)
{
/* feeder close*/
    int feedernumber =  StrToInt(Edit1->Text);
     GUIcmdQueue.FeederCtl(feedernumber,0);
     LM(" Feeder close "+IntToStr(feedernumber));
}

//---------------------------------------------------------------------------

void __fastcall TForm1::CloseAllFeeders(void) {

	for (int i=0;i<(TVM920_FEEDERCMD_NFRONT_FEEDERS+TVM920_FEEDERCMD_NREAR_FEEDERS) ; i++) {
    	GUIcmdQueue.FeederCtl(i,0);
        Sleep(50);
        LM("Feeder close  "+IntToStr(i));
        Application->ProcessMessages();
    }

}

void __fastcall TForm1::ShowTrajectory(Cmachloc<double> & s, Cmachloc<double> &d ) {

	/* draw trajectory line */
    TImage * im = Image1;

    can->Pen->Color=clWhite;
    TPoint start;
    TPoint end;
    //TRect rect;

    /* now, machine is 0,0 BLHC, but canvas is 0,0 TLHC */
    /* IE X is same, but Y coordinates inverse */

    start.x  = ((double) im->Width-1) * (s.x / TVMinfo.xtravel);
    start.y =im->Height -  (((double)im->Height-1) * (s.y/ TVMinfo.ytravel));
    end.x = ((double)im->Width-1) * (d.x /  TVMinfo.xtravel);
    end.y =im->Height - (((double)im->Height-1) * (d.y/ TVMinfo.ytravel));
    try {
    	can->MoveTo(start.x, start.y);
    	can->LineTo(end.x, end.y);
    }
    catch (Exception &e) {

    }
    //can->Brush->Color=clGreen;
    //rect.TopLeft(start);
    //rect.BottomRight(start.X+rectwidth, start.Y+rectwidth)
    //can->FillRect(rect);




}

void __fastcall TForm1::FillCanvasBlack(void) {
    TImage * im = Image1;

    TRect rect;
    rect.top=1;
    rect.left=1;
    rect.right=im->Width-1;
    rect.bottom=im->Height-1;
    can->FrameRect(rect);
    can->Brush->Color=clBlack;
    can->FillRect(rect);
}
void __fastcall TForm1::UpdateFeederCanvas(__int64 feeder) {
	/* decode  */
    /* front row */
    __int64 feedertestbitmask=0;
    TImage * im = Image1;
    can->Brush->Style=bsSolid;
    can->Brush->Color=clYellow;
    int rectwidth = im->Width /  TVM920_FEEDERCMD_NFRONT_FEEDERS;
    int rectheight =   rectwidth;
    TRect rect;

    rect.top= im->Height-rectheight;
    rect.bottom = im->Height-1;
    rect.left = 0;
    for (int i = 0 ; i < TVM920_FEEDERCMD_NFRONT_FEEDERS;i++) {

		rect.left = i*rectwidth;
        rect.right=rect.left + rectwidth;
        feedertestbitmask=(1LL<<i);
        if (feeder & feedertestbitmask) {

        	can->FillRect(rect);
        }
    }

    /* do REAR feeders */
    rect.top= 0;
    rect.bottom = rectheight;
    rect.left = 0;
    int index=0;
    for (int i = TVM920_FEEDERCMD_NFRONT_FEEDERS ; i < TVM920_FEEDERCMD_NFRONT_FEEDERS+TVM920_FEEDERCMD_NREAR_FEEDERS;i++) {

		rect.left = (index++) * rectwidth;
        rect.right=rect.left + rectwidth;
        feedertestbitmask =1LL<<i;
        if (feeder & feedertestbitmask) {

       	can->FillRect(rect);
        }
    }



}


void __fastcall TForm1::Button26Click(TObject *Sender)
{
	CloseAllFeeders();
}
//---------------------------------------------------------------------------

