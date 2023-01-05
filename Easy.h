//---------------------------------------------------------------------------
#ifndef EasyH
#define EasyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include "global.hpp"
#include "graph.h"
#include "legend.h"
#include "movie.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <float.h>
#include <jpeg.hpp>
#include <vector>
#include "a_shape.hpp"
#include "a_text.hpp"
#include "a_line.hpp"
#include "a_circle.hpp"
#include "a_arrow.hpp"
#include "a_barplot.hpp"
#include "a_bitmap.hpp"
#include <windows.h>


#if !defined(LINUX_ANIMATION)
//#include "Tiller/tiller.h"
#include "ByPassCaps/bypass_cap.h"
#endif

#define MAX_NUM_DATASETS 3
#define MAX_NUM_FIXEDPLOTS 30
#define MAX_NUM_VARPLOTS 30
#define MAX_NUM_PLOTTYPES 45
#define MAX_ARGS 20



//---------------------------------------------------------------------------
class TEasyPlotForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TButton *OKBtn;
   TEdit *Edit1;
   TLabel *Label1;
   TLabel *Label3;
   TLabel *Label4;
   TButton *PlotBtn;
   TButton *Button4;
   TGroupBox *GroupBox1;
   TEdit *Edit2;
   TEdit *Edit3;
   TLabel *Label2;
   TLabel *Label7;
   TEdit *Edit4;
   TLabel *Label8;
   TEdit *Edit5;
   TEdit *Edit6;
   TEdit *Edit7;
   TEdit *Edit8;
   TLabel *Label9;
   TLabel *Label10;
   TLabel *Label11;
   TLabel *Label12;
   TButton *settingsBtn;
   TCheckBox *CheckBox3;
   TButton *createBtn;
   TButton *convertBtn;
   TOpenDialog *OpenDialog1;
   TFindDialog *FindDialog1;
   TCheckBox *CheckBox2;
   TButton *animateBtn;
   TMainMenu *MainMenu1;
   TMenuItem *File1;
   TMenuItem *Help1;
   TMenuItem *Close1;
   TMenuItem *HelpTopics1;
   TMenuItem *Open1;
   TTimer *Timer1;
   TStatusBar *StatusBar1;
   TImage *Image2;
   TButton *previewBtn;
   TTimer *Timer2;
   TRadioGroup *RadioGroup1;
    TButton *SelPrevBtn;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label13;
    TLabel *Label14;

   void __fastcall PlotBtnClick(TObject *Sender);
   void __fastcall OKBtnClick(TObject *Sender);
   void __fastcall settingsBtnClick(TObject *Sender);
   void __fastcall createBtnClick(TObject *Sender);
   void __fastcall convertBtnClick(TObject *Sender);
   void __fastcall Button4Click(TObject *Sender);
   void __fastcall Edit6Change(TObject *Sender);
   void __fastcall Edit5Change(TObject *Sender);
   void __fastcall Edit7Change(TObject *Sender);
   void __fastcall Edit8Change(TObject *Sender);
   void __fastcall CheckBox2Click(TObject *Sender);
   void __fastcall animateBtnClick(TObject *Sender);
   void __fastcall File1Click(TObject *Sender);
   void __fastcall Close1Click(TObject *Sender);
   void __fastcall HelpTopics1Click(TObject *Sender);
   void __fastcall Open1Click(TObject *Sender);
   void __fastcall Timer1Timer(TObject *Sender);
   void __fastcall previewBtnClick(TObject *Sender);
   void __fastcall Timer2Timer(TObject *Sender);
    void __fastcall SelPrevBtnClick(TObject *Sender);

private:	// User declarations
 int m_iLogPlotMode;
 int m_iWidth, m_iHeight;

 //Colours for graphics:
   TColor m_cBackground,m_cGridlines,m_cSignal,m_cSpectrum,
          m_cOrigin,m_cAudioMarker;

   TColor m_cDataSet[3], m_cLegend;
 //Legend
int m_iLeftPos,m_iTopPos;

 //Define data file name;
 char m_sDataFile[300];
 wchar_t m_swDataFile[300];
 char m_sXLabel[50], m_sYLabel[50];
//Graph features:
 bool m_bXGrad,m_bYGrad, m_bXAxisShow,m_bYAxisShow;
 //Define data
 floatmat m_fmData;
 int m_iNumDataSets;
 bool m_bXDataFileSet;
 bool m_bSkipSet[3];

//Bar graph attributes:
int m_iBarWidth;

//Define size of data point plots (rectangle or circle):
int m_iDataPlotSize; //pixels


 //Define plot modes for data sets
 int m_iPlotMode[3];

 //Auto scale flag:
 bool m_bAutoScale;
 float m_fAutoXMin,m_fAutoXMax,m_fAutoYMin,m_fAutoYMax;
 bool m_bAutoXMin, m_bAutoXMax,m_bAutoYMin,m_bAutoYMax;

 char m_sAutoNumStr[20];

 //Legend flag:
 bool m_bLegendOn;
 //Legend Data Set text:
 char m_sLegendText1[100],m_sLegendText2[100],m_sLegendText3[100];
 wchar_t m_swLegendText1[100],m_swLegendText2[100],m_swLegendText3[100];
 int m_iLegendPlotType1,m_iLegendPlotType2,m_iLegendPlotType3;

 //Flag to indicate file type:
 bool m_bIsWaveFile;

 //Wave file attributes:
 int m_iNumChannels, m_iPlaybackWaveFileSamplesPerSec, m_iBitsPerSample;

 bool m_bTitleOverride;

 bool m_bDrawBorder; //Flag for graph border
 bool m_bDisableGraphMenu;  //Flg to disable the menu of the graph form

 //Location of executable file:
 char m_sProgLocation[200];
 char m_sOldDir[MAXPATH];

//ANIMATION EFFECTS: -----------------------------------------------------
//Animation flag:
A_Shape *m_pShape;
bool m_bAnimation;

//Override flag:
bool m_bOverride;   //Override separate subdirectories for each tag program file
int m_iAniCumCnt;
///Movie Preview Control:
int m_bMovieControl,m_bProgrammedControl;
TStringList *m_SLContIndex;
int m_iContCnt;


//Data plot delay:
int m_iViewDelay;
//Pointer to graphic object:
TGraphForm *m_GraphFrm;
//Animation count variables:
int m_iAniCnt, m_iTotAniCnt, m_iMaxTotAniCnt;
//Filename tag for movie still sequence:
char m_sMovieTag[200];
//Target directory for movie stills:
wchar_t m_swMovieTargetRoot[200];
char m_sMovieTarget[200],m_sMovieTargetRoot[200];
//Flag to view stills individually as created
bool m_bViewStill;
//Name of animation program file:
char m_sTagFile[200],m_sDefFile[200];
FILE *m_fpTagFile, *m_fpDefFile;


//Number of fixed plots for animation sequence
int m_iNumFIXEDPlots,m_iFIXEDIndex;
int m_iNumVARPlots,m_iVARIndex;
//Arrays for instruction types:
char  m_sFIXED[MAX_NUM_FIXEDPLOTS][50], m_sFIXEDParams[MAX_NUM_FIXEDPLOTS][MAX_ARGS][100];
int m_iNumFIXEDPlotArgs[MAX_NUM_FIXEDPLOTS];
char  m_sVAR[MAX_NUM_VARPLOTS][50], m_sVARParams[MAX_NUM_VARPLOTS][MAX_ARGS][100];
int m_iNumVARPlotArgs[MAX_NUM_VARPLOTS];

char m_sVARSingle[100], m_sVARSingleParams[MAX_ARGS][100];
char m_sPLOTTYPES[MAX_NUM_PLOTTYPES][50];
wchar_t m_swPLOTTYPES[MAX_NUM_PLOTTYPES][50];
int m_iNumPlotTypes;
int m_iNumArgs;
bool m_bProcFIXED, m_bProcVAR, m_bVARPresent;
//Horizontal line position:
float m_fHPos, m_fVPos;
bool m_bFileIgnore;
TImage *m_ImMovie;
int m_iPreviewIndex, m_iPreviewInt,m_iPreviewIndexStart;

TMovieForm *m_MovieForm;
bool m_bMultiTagFiles;
int m_iTagFileCount,m_iDirCnt;
char m_sDirTag[100];
TStringList *m_SLTagFiles;
bool m_bStartTimer1;

//-------------------------------------------------------------------------
public:		// User declarations
   __fastcall TEasyPlotForm(TComponent* Owner);
   void GetIniFileSettings();
   void doPlotType(TGraphForm *graphFrm,char *plotType,int index);
   void plotData(TGraphForm *graphFrm);
   void plotWaveFile();
   //void parseTagFile();
   void parseTagFile1();
   void parseDefFile();
   //bool getArgList(char *p,int index);
   bool getArgList1(char *p,int index);
   void getDirectoryTag();
   void setUpSubDir();
   void float2Str(float num);
   unsigned long int readWaveHeaderFile(FILE *fp);
   void setDataSetColors(char *color, int index);
   char* setLegendComboBackgroundColor();
   char* setComboBoxDefaultColor(int index);
   void setLegendBackgroundColor(char *color);
   void setRadioDataPlotModes(TSettingsDlg *settings);
   void setDataPlotModes(int index1,int index2,int index3);
   char* getColorText(TColor c);
   TColor setColor(char *color);
   TColor setColor1(char *color);
   bool checkForPrompt();


// Add additional functions if running standard statoc graphics
#if !defined(LINUX_ANIMATION)
//--------------------------------------------------------------------------
// Define data arrays for Linux integrated code
//-------------------------------------------------------------------------

#if (NUM_PLOTS == 1)
	float data1[MAX_DATA_LEN],data2[MAX_DATA_LEN];
#endif

#if (NUM_PLOTS == 2)
	float data1[MAX_DATA_LEN],data2[MAX_DATA_LEN],data3[MAX_DATA_LEN];
#endif

#if (NUM_PLOTS == 3)
		float data1[MAX_DATA_LEN],data2[MAX_DATA_LEN],data3[MAX_DATA_LEN],data4[MAX_DATA_LEN];
#endif

#if (NUM_PLOTS == 4)
	float data1[MAX_DATA_LEN],data2[MAX_DATA_LEN],data3[MAX_DATA_LEN],data4[MAX_DATA_LEN],data5[MAX_DATA_LEN];
#endif

int NumDataSets;

   int readData();
   void doGraph();
#endif


};
//---------------------------------------------------------------------------
extern PACKAGE TEasyPlotForm *EasyPlotForm;
//---------------------------------------------------------------------------
#endif
