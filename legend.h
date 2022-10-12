//---------------------------------------------------------------------------
#ifndef legendH
#define legendH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <FileCtrl.hpp>
//---------------------------------------------------------------------------
class TSettingsDlg : public TForm
{
__published:	// IDE-managed Components
   TButton *OKBtn;
   TButton *cancelBtn;
   TPageControl *PageControl1;
   TTabSheet *TabSheet1;
   TTabSheet *TabSheet2;
   TGroupBox *GroupBox1;
   TEdit *Edit1;
   TEdit *Edit2;
   TLabel *Label1;
   TLabel *Label2;
   TGroupBox *GroupBox2;
   TCheckBox *CheckBox1;
   TComboBox *ComboBox1;
   TLabel *Label3;
   TGroupBox *GroupBox3;
   TLabel *Label4;
   TComboBox *ComboBox2;
   TCheckBox *CheckBox2;
   TGroupBox *GroupBox4;
   TCheckBox *CheckBox3;
   TLabel *Label5;
   TComboBox *ComboBox3;
   TCheckBox *CheckBox4;
   TGroupBox *GroupBox5;
   TLabel *Label6;
   TLabel *Label7;
   TLabel *Label8;
   TEdit *Edit3;
   TEdit *Edit4;
   TEdit *Edit5;
   TGroupBox *GroupBox6;
   TComboBox *ComboBox4;
   TRadioGroup *RadioGroup1;
   TRadioGroup *RadioGroup2;
   TRadioGroup *RadioGroup3;
   TTabSheet *TabSheet3;
   TGroupBox *GroupBox7;
   TComboBox *ComboBox5;
   TComboBox *ComboBox6;
   TComboBox *ComboBox7;
   TLabel *Label9;
   TLabel *Label10;
   TLabel *Label11;
   TTabSheet *TabSheet4;
   TEdit *Edit6;
   TLabel *Label12;
   TLabel *Label13;
   TEdit *Edit7;
   TLabel *Label14;
   TOpenDialog *OpenDialog1;
   TDirectoryListBox *DirectoryListBox1;
   TDriveComboBox *DriveComboBox1;
   TLabel *Label15;
   TCheckBox *CheckBox5;
   TEdit *Edit8;
   TLabel *Label16;
   TLabel *Label17;
   TEdit *Edit9;
   TLabel *Label18;
   TEdit *Edit10;
   TLabel *Label19;
   TGroupBox *GroupBox8;
   TCheckBox *CheckBox6;
   TCheckBox *CheckBox7;
   TCheckBox *CheckBox8;
   TCheckBox *CheckBox9;
    TCheckBox *CheckBox12;
    TCheckBox *CheckBox13;
    TEdit *Edit12;
    TEdit *Edit13;
    TEdit *Edit14;
    TLabel *Label20;
    TLabel *Label21;
    TLabel *Label22;
    TCheckBox *CheckBox10;
    TCheckBox *CheckBox11;
    TEdit *Edit11;
    TLabel *Label23;
   void __fastcall CheckBox5Click(TObject *Sender);
   void __fastcall DirectoryListBox1Change(TObject *Sender);
   void __fastcall RadioGroup1Click(TObject *Sender);

   private:	// User declarations
public:		// User declarations
   __fastcall TSettingsDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsDlg *SettingsDlg;
//---------------------------------------------------------------------------
#endif
