//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
#include "CLogHandlerX.h"
#include "tvm920def.h"

#include "Cmachloc.h"
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

#define UMTVM_STATUSUPDATE WM_USER+1
#define UMTVM_INITIALIZE_DONE WM_USER+2
#define UMTVM_GOHOME WM_USER+3
#define UMTVM_FEEDER_STATUS_UPDATE WM_USER+4
#define UMTVM_TRAJECTORY_UPDATE WM_USER+5

extern CLogHandlerX * logx;


//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TButton *Button2;
	TMemo *Memo1;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *Button6;
	TButton *Button7;
	TButton *Button8;
	TButton *Button9;
	TEdit *Edit1;
	TLabel *Label1;
	TButton *Button10;
	TButton *Button11;
	TButton *Button12;
	TButton *Button13;
	TLabel *Label2;
	TStatusBar *StatusBar1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox5;
	TBevel *Bevel1;
	TStaticText *StaticText1;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox7;
	TCheckBox *CheckBox8;
	TCheckBox *CheckBox9;
	TCheckBox *CheckBox10;
	TCheckBox *CheckBox11;
	TCheckBox *CheckBox12;
	TCheckBox *CheckBox13;
	TBevel *Bevel2;
	TStaticText *StaticText2;
	TEdit *Edit2;
	TLabel *Label3;
	TEdit *Edit3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TEdit *Edit4;
	TEdit *Edit5;
	TEdit *Edit6;
	TEdit *Edit7;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TEdit *Edit8;
	TEdit *Edit9;
	TEdit *Edit10;
	TEdit *Edit11;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TBevel *Bevel3;
	TStaticText *StaticText3;
	TButton *Button14;
	TButton *Button15;
	TButton *Button16;
	TButton *Button17;
	TButton *Button18;
	TButton *Button19;
	TButton *Button20;
	TButton *Button21;
	TStaticText *StaticText4;
	TStaticText *StaticText5;
	TBevel *Bevel4;
	TBevel *Bevel5;
	TButton *Button23;
	TButton *Button25;
	TButton *Button27;
	TButton *Button29;
	TStaticText *StaticText7;
	TStaticText *StaticText8;
	TButton *Button30;
	TEdit *Edit12;
	TStaticText *StaticText9;
	TEdit *Edit13;
	TLabel *Label23;
	TEdit *Edit14;
	TLabel *Label24;
	TCheckBox *CheckBox14;
	TCheckBox *CheckBox15;
	TCheckBox *CheckBox16;
	TStaticText *StaticText10;
	TEdit *Edit15;
	TEdit *Edit16;
	TStaticText *StaticText11;
	TButton *Button31;
	TCheckBox *CheckBox17;
	TCheckBox *CheckBox18;
	TCheckBox *CheckBox19;
	TButton *Button33;
	TButton *Button34;
	TButton *Button35;
	TEdit *Edit17;
	TEdit *Edit18;
	TStaticText *StaticText12;
	TStaticText *StaticText13;
	TBevel *Bevel6;
	TButton *Button32;
	TButton *Button36;
	TEdit *Edit19;
	TStaticText *StaticText14;
	TStaticText *StaticText15;
	TEdit *Edit20;
	TButton *Button37;
	TEdit *Edit21;
	TButton *Button38;
	TButton *Button39;
	TButton *Button40;
	TStaticText *StaticText16;
	TBevel *Bevel7;
	TEdit *Edit22;
	TEdit *Edit23;
	TStaticText *StaticText17;
	TStaticText *StaticText18;
	TEdit *Edit24;
	TEdit *Edit25;
	TButton *Button41;
	TCheckBox *CheckBox20;
	TCheckBox *CheckBox21;
	TCheckBox *CheckBox22;
	TBevel *Bevel8;
	TCheckBox *CheckBox23;
	TStaticText *StaticText19;
	TButton *Button42;
	TButton *Button43;
	TButton *Button44;
	TEdit *Edit26;
	TEdit *Edit27;
	TEdit *Edit28;
	TEdit *Edit29;
	TStaticText *StaticText20;
	TStaticText *StaticText21;
	TStaticText *StaticText22;
	TEdit *Edit30;
	TStaticText *StaticText23;
	TStaticText *StaticText24;
	TCheckBox *CheckBox24;
	TButton *Button45;
	TButton *Button46;
	TEdit *Edit31;
	TEdit *Edit32;
	TEdit *Edit33;
	TEdit *Edit34;
	TLabel *Label25;
	TEdit *Edit35;
	TLabel *Label26;
	TButton *Button22;
	TEdit *Edit36;
	TButton *Button24;
	TImage *Image1;
	TButton *Button26;


	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall Button30Click(TObject *Sender);
	void __fastcall Button11Click(TObject *Sender);
	void __fastcall Button13Click(TObject *Sender);
	void __fastcall Button12Click(TObject *Sender);
	void __fastcall Memo1DblClick(TObject *Sender);
	void __fastcall Button31Click(TObject *Sender);
	void __fastcall Button32Click(TObject *Sender);
	void __fastcall Button33Click(TObject *Sender);
	void __fastcall Button34Click(TObject *Sender);
	void __fastcall Button35Click(TObject *Sender);
	void __fastcall Button36Click(TObject *Sender);
	void __fastcall Button14Click(TObject *Sender);
	void __fastcall Button15Click(TObject *Sender);
	void __fastcall Button16Click(TObject *Sender);
	void __fastcall Button17Click(TObject *Sender);
	void __fastcall Button18Click(TObject *Sender);
	void __fastcall Button19Click(TObject *Sender);
	void __fastcall Button20Click(TObject *Sender);
	void __fastcall Button21Click(TObject *Sender);

	void __fastcall Button23Click(TObject *Sender);

	void __fastcall Button25Click(TObject *Sender);

	void __fastcall Button27Click(TObject *Sender);

	void __fastcall Button29Click(TObject *Sender);
	void __fastcall Button37Click(TObject *Sender);
	void __fastcall Button38Click(TObject *Sender);
	void __fastcall Button39Click(TObject *Sender);
	void __fastcall Button40Click(TObject *Sender);
	void __fastcall Button41Click(TObject *Sender);
	void __fastcall CheckBox20Click(TObject *Sender);
	void __fastcall CheckBox21Click(TObject *Sender);
	void __fastcall CheckBox22Click(TObject *Sender);
	void __fastcall CheckBox23Click(TObject *Sender);
	void __fastcall Button42Click(TObject *Sender);
	void __fastcall Button43Click(TObject *Sender);
	void __fastcall Button44Click(TObject *Sender);
	void __fastcall Button45Click(TObject *Sender);
	void __fastcall Button46Click(TObject *Sender);
	void __fastcall Button22Click(TObject *Sender);
	void __fastcall Button24Click(TObject *Sender);
	void __fastcall Button26Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    void __fastcall Startup(void);
	void __fastcall Shutdown(void);
    void __fastcall LM(AnsiString as);
    void __fastcall LM(char * s);
    void __fastcall DoPick(int num, int state);
    void __fastcall TForm1::AppMessage(tagMSG &Msg, bool &Handled);
    void __fastcall TForm1::Status1Update(void);
    void __fastcall TForm1::Status2Update(void);
    void __fastcall TForm1::Status3Update(void);
    AnsiString __fastcall TForm1::TVMmotionstate(int i);
    void __fastcall TForm1::MoveXYZA(uint32_t axismask,int x, int y, int z[TVM920_NNOZZ], int a[TVM920_NNOZZ]);
	void __fastcall TForm1::ForceXY(void);
    void __fastcall TForm1::SendMotionZeroMotion(void);
    void __fastcall TForm1::TVMSetEnd_Stop_MotionEnables(uint32_t axis, uint32_t disabled);
    void __fastcall TForm1::SetGPIOdirect(int bitnum, int state01);
    void __fastcall TForm1::MoveXYincrement(int isX, int isY);
    void __fastcall TForm1::NozzleA(int nozzle,  int useticks);
    void __fastcall TForm1::UpdateStartupGUI(void);
    void __fastcall TForm1::NozzleSetZ(int nozzle, int value );
	void __fastcall TForm1::NozzlePlungeZ(int nozzle, int pos, bool IsTicks );
    void __fastcall TForm1::ForceZzero(void);
    void __fastcall TForm1::ForceAzero(void);
    void __fastcall TForm1::HomeX(int step1, int step2);
	void __fastcall TForm1::AxisControlMuteClick(void);
	void __fastcall TForm1::HomeY(int step1);
    void __fastcall TForm1::GoHome(void);
    void __fastcall TForm1::HomeZ(void);
    void __fastcall TForm1::MoveZbipolar(uint32_t axismask, int z[TVM920_NNOZZ]);

	void __fastcall FillCanvasBlack(void);

	void __fastcall TForm1::CloseAllFeeders(void);
    void __fastcall TForm1::UpdateCanvas(void);
    void __fastcall TForm1::UpdateFeederCanvas(__int64 feeder);
    void __fastcall TForm1::ShowTrajectory(Cmachloc<double> & s, Cmachloc<double> &d );
    void __fastcall TForm1::WriteRegister(uint32_t reg, int32_t data);

    public :

    int HomeAbort;
    TCanvas * can;


};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

