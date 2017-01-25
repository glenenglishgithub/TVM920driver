# TVM920driver
GUI/ RT interface for QIHeTVM920

Here is a motion and system driver for the TVM920
it connects directly to  the mcirocontroller PCB inside the left hand side of the TVM920.
(192.168.0.8)
it replaces the TVM920 QIHe PC software (running on the PC 192.168.0.10)

It is written in c++, for C++ BUILDER and will compile on the free STARTER edition
ultimately it may transfer to Codelite and wxWidgets GUI

the program supports all operations and diagnostics and correct assymetrical resolution homing

in addition, it has a TCP socket interface to a driver interface I have written for Open PNP. (MyTVMDriver.java)
this currently expects to run on the same machine on a loopback port.
but this will be configurable to run over networks.

both interfaces (manual and socket) can be simultaneously active

there is a GUI showing feeder openings and head motion vectors and a bunch of diagnostics

Such things like actuator and nozzle names from OpenPNP are translated into commands for the TVM920

Project direction :
At the moment, the multi feeder system in OpenPNP is under active development. It's not finished.
I personally dont think the OpenPNP feeder setup is right for a multi feeder system, so that will have to change before more is done on the openPNP side. OpenPNP has some excellent attributes, so I will be persevering with it for the moment. Either open PNP will change or I will write a 'helper importer' for OpenPNP so it can work with multi feeders sensibly, and manipulate the XML setup files directly. (IMO).

An alternative path will be I will write a PNP application specific to the TVM920 , and it will be modelled on the Yamaha YVL88 control system. That will likely be written in Python and use PyOpenCV for vision, and , again communicate directly with the TVM920 driver via the same OpenPNP created-for socket interface .
