//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "movie.h"
#include "Easy.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMovieForm *MovieForm;
//---------------------------------------------------------------------------
__fastcall TMovieForm::TMovieForm(TComponent* Owner, TImage *image, int height, int width,
                                  TTimer *timer,TBorderStyle bstyle)
   : TForm(Owner)
{
    m_MovieTimer = timer;
	BorderStyle = bstyle;

   Update(image,height,width);


}
//---------------------------------------------------------------------------
void TMovieForm::Update(TImage *image, int height, int width)
{
    int x_margin, y_margin;

   //Set up image:
   x_margin = 20;
   y_margin = 45;
   Height = height + y_margin;
   Width =width + x_margin;

   //Intialise image pointers:
   //Image1 = image;
   Image1->Parent = this;

   Image1->Picture = image->Picture;
   Image1->Height = height;
   Image1->Width =  width;
   Image1->Left = 5;
   Image1->Top =  5;

   Image1->Show();


}//Update


void __fastcall TMovieForm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  switch( Key )
	{
		case VK_RETURN:
			//Toggle between "pause" and "play":
			if(m_MovieTimer->Enabled == false)
			{
			   m_MovieTimer->Enabled = true;
			}
			else
			{
			   m_MovieTimer->Enabled = false;
			}

			break;
		case VK_SPACE:
			//Replay the Movie sequence:
			EasyPlotForm->SelPrevBtnClick(Sender);
			m_MovieTimer->Enabled = true;   //Set to true to allow immediate replay without pause control
			break;
	   	case VK_DELETE:      //Only to be used when top border control is removed
			Close();
			break;

	}
   
}
//---------------------------------------------------------------------------

