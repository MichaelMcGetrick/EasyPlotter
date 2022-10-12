//---------------------------------------------------------------------------
#ifndef movieH
#define movieH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "global.hpp"
//---------------------------------------------------------------------------
class TMovieForm : public TForm
{
__published:	// IDE-managed Components
   TImage *Image1;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
   TTimer *m_MovieTimer;



public:		// User declarations
   __fastcall TMovieForm(TComponent* Owner,TImage *image, int height, int width,
                         TTimer *timer,TBorderStyle bstyle);
   void Update(TImage *image, int height, int width);
};
//---------------------------------------------------------------------------
extern PACKAGE TMovieForm *MovieForm;
//---------------------------------------------------------------------------
#endif
