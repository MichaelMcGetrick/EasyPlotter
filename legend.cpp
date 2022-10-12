//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "legend.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingsDlg *SettingsDlg;
//---------------------------------------------------------------------------
__fastcall TSettingsDlg::TSettingsDlg(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------






void __fastcall TSettingsDlg::CheckBox5Click(TObject *Sender)
{
   if(!CheckBox5->Checked)
   {
      //Set view delay to zero ms:
      Edit6->Text = "10";
   }
   else
   {
      //Set view delay to default ms:
      Edit6->Text = "1000";

   }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsDlg::DirectoryListBox1Change(TObject *Sender)
{
   Edit8->Text = DirectoryListBox1->Directory;   
}
//---------------------------------------------------------------------------

void __fastcall TSettingsDlg::RadioGroup1Click(TObject *Sender)
{
   //Check for bar graph selection:
   if(RadioGroup1->ItemIndex == 3)
   {
      Label17->Enabled = true;
      Edit9->Enabled = true;
   }
   else
   {
      Label17->Enabled = false;
      Edit9->Enabled = false;
   }
}
//---------------------------------------------------------------------------

