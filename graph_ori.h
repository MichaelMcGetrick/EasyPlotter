//---------------------------------------------------------------------------
#ifndef graphH
#define graphH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
//#include "printprog.h"
//#include "audioplayer.h"
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <mmsystem.h>
#include <dir.h>
#include "floatvec.hpp"


#define WINDOW_PIXEL_LENGTH 575   //Num pixels across dual-trace

//---------------------------------------------------------------------------
class TGraphForm : public TForm
{
__published:	// IDE-managed Components
   TImage *Image2;

   TMainMenu *MainMenu1;
   TMenuItem *File1;
   TMenuItem *Print1;
   TMenuItem *Save1;
   TTimer *PrintTimer;
   TMenuItem *Exit1;
   TMenuItem *Play1;
   TTimer *Playback;
   TSaveDialog *SaveDialog1;
   TPrintDialog *PrintDialog1;
   TMenuItem *Tools1;
   TMenuItem *NewAudioFile1;
   TMenuItem *SelectedRegion1;

   void __fastcall Print1Click(TObject *Sender);
   void __fastcall Save1Click(TObject *Sender);
   void __fastcall PrintTimerTimer(TObject *Sender);
   void __fastcall Exit1Click(TObject *Sender);
   void __fastcall FormResize(TObject *Sender);
   void __fastcall Play1Click(TObject *Sender);
   void __fastcall PlaybackTimer(TObject *Sender);
   void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
   void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall SelectedRegion1Click(TObject *Sender);
   void __fastcall FormDblClick(TObject *Sender);
   

private:	// User declarations
        bool monoMode, showGrid, xgrad, ygrad;
        int plotMode, gridInt;
        int xincrement, cirSize, logPlotMode, lineFlag,
            xmargin, ymargin;
        float ymin1, ymin2, ymax1, ymax2, xmin, xmax, sigStore1,
              sigStore2;
        float xminLinear,xmaxLinear;
        float ymaxVal1,ymaxVal2;
        int left, top, right, bottom,C1, counter1, counter2;
        int left1,top1,right1,bottom1,left2,top2,right2,bottom2,C2;
        int xnoDivs, ynoDivs;
        char xfield[4], yfield[4];
        char xLabel[100],yLabel[100], Title[100];
        int image;
        int buffCount1, buffCount2;
        TRect MonoGraphRectangle;
        float buffer1[WINDOW_PIXEL_LENGTH], buffer2[WINDOW_PIXEL_LENGTH];
        bool showXAxisNums,showYAxisNums,m_bGrid;
        bool Bitmap;
        TPrinter *printer;
        //TPrintProgDlg *prntDlg;
        char m_sPrinterName[100];
        int m_iFormHeight, m_iFormWidth;
        TColor m_cMonoBackCol;

         //Legend flag:
         bool m_bLegendOn;

         //Legend Data Set text:
         char m_sLegendText1[100],m_sLegendText2[100],m_sLegendText3[100];
         //Legend position:
         int m_iLegendLeftPos,m_iLegendTopPos;
         //Legend data set colors:
         TColor m_cDataSet[3];
         //Legend background color:
         TColor m_cLegend;
         bool m_bLegendMove;

         //Data sets:
         bool m_bXDataFileSet;
         int m_iNumDataSets;
         bool m_bSkipSet[3];
          //Define plot modes for data sets
         int m_iPlotMode[3];

         //Bar graph attributes:
         int m_iBarWidth;

        //Audio player members:
        //TAudPlay *AudioPlayer;
        char m_szAudioIniFile[200],m_sFile[200];
        int m_iNumChannels, m_iSampleRate, m_iBitsPerSample;
        int m_iCurrSampleNum;
        TColor m_cWaveSigColor, m_cDataPlotColor, m_cAudioMarkerColor,
               m_cAudioClipMarkerColor;
        int m_iSigUpper, m_iSigLower;
        bool m_bSigTrace,m_bSigRangeDefined;

        //Members for defining clipped regions:
        bool m_bSelected, m_bLastClickWasDouble;
        float m_fClipStartPos, m_fClipEndPos;
        int   m_iClipStartPixPos, m_iClipEndPixPos, m_iCurrPixPos,m_iLastPixPos,
              m_iClipPixWidth, m_iCurrYPixPos, m_iLastYPixPos;
        int m_iClipStartSigHighPixPos,m_iClipStartSigLowPixPos,
            m_iClipEndSigHighPixPos,m_iClipEndSigLowPixPos;
        char m_sClipWaveFile[200], m_sCurrDir[MAXPATH];
        bool m_bAbortSelection,m_bIsWaveFile;
        char m_sFileExt[4];

        TImage *Image1;

        //frequency spectrum:
        int m_iFreqIndex;
        bool m_bHarmonic;  //Flag for plotting frequency harmonics

        //Colours for graphics:
        TColor m_cBackground,m_cGridlines,m_cSignal,m_cSpectrum, m_cHarmonics,
        m_cSFmarker, m_cOrigin;
        bool m_bAnimated,m_bResize,m_bAllowResize;

public:		// User declarations
         __fastcall TGraphForm(TComponent* Owner, bool tMode,
                              int pMode,int logMode,
                              char *xlab, char *ylab, char *gphtitle,
                              float yMin, float yMax, float xMin, float xMax,
                              int xInt, int yInt, bool xGrad, bool yGrad,
                              char *xlabel1,char *ylabel1,char *xlabel2,
                              char *ylabel2,
                              float yMin1,float yMax1,float yMin2,
                              float yMax2,bool grid, int gInt,
                              bool xshow,bool yshow,bool bitmap,
                              TColor monobcolor,char *audfile, char *audInifile,
                              int chans,int samrate, int bps,int formWidth,
                              int formHeight,TBorderStyle bstyle);

        void traceDelay(int timeDelay){Sleep(timeDelay);}
        void setupWindow(int image,int left, int top, int right, int bottom,
                         char *xlabel, char *ylabel);
        void setupImageSize();
        void setupGraphSize();
        void title(char *text);         //Only for mono-trace
        void xAxisText(int image,char *text);
        void yAxisText(int image,char *text);
        void tagOrigin(int image,int left, int posOrig);
        void plotSig(float ySig, float xSig);
        bool plotSig1(float signal);
        void animatedPlot1(float signal,bool stop);
        bool plotSig2(float signal);
        void animatedPlot2(float signal,bool stop);
        void plotAudioMarkerPos(float ySig, float xSig);
        void plotAudioMarkerPos1(float ySig, float xSig);
        void plotClipMarkerPos(float ySig, float xSig);
        void deSelectWaveClip();
        void dispBufferElems(float signal,int buffMode);
        int positionPixel(int image,float ymin,float ymax,float sig,int pixelMode=0);
        float getXValFromPixPos(int npixels);
        void text(int image,float x,float y,char *caption);
        void legend(int l,int t,int nsets,char *t1,char *t2, char *t3,
                    TColor c1,TColor c2, TColor c3,TColor bck,
                    int pm1,int pm2,int pm3);
        void setDataSymbol(int left, int top, int pmode,int shift);
        void grid(int xInt, int yInt);
        void graduations(); //Only for mono-trace
        void pixelVerLine(int x, int pixelIncr);
        void plotVerLine(float ySig,float xSig);
        void pixelAudioMarkerLine(int x, int pixelIncr);
        void pixelClipMarkerLine(int x, int pixelIncr);
        void pixelHorMarkerLine(int x, int pixelIncr);
        void pixelHorLine(int y, int pixelIncr);
        void verLines(int xInterval);
        void horLines(int yInterval);
        void drawOriginLine();
        void drawHorLine(float pos);
        void drawVerLine(float pos);
        void drawBar(float y1,float y2,float x, int bwidth, TColor color);
        void drawArrow(float x, float y, int h, int w, TColor color, char *type);
        void xAxisScale(); //Only for mono-trace
        void yAxisScale(); //Only for mono-trace
        int getExponent(float num);
        void axisScales() { xAxisScale(); yAxisScale();} //Only for mono-trace        void axisDefaults(int xdivs,int ydivs,char *xspec, char *yspec);
        void changePlotMode(int p) {plotMode = p;}
        void changeLogPlotMode(int p) {logPlotMode = p;}
        void xPlotIncr(int xincr){xincrement = xincr;}
        void circleSize(int cSize){cirSize = cSize;}
        void setLinePlotFlag(int flg) {lineFlag = flg;}//Only use with single trace
        void initialisePizPos(){counter1=left1;counter2=left2;}
        void axisDefaults(int xdivs,int ydivs,char *xspec, char *yspec);
        void setMonoGraphBackground(TColor color);
        void setxMinLinear(int min){xminLinear=min;}
        int getxMinLinear(){return xminLinear;}
        void setxMaxLinear(int max){xmaxLinear=max;}
        int getxMaxLinear(){return xmaxLinear;}
        void setxMin(int min){xmin=min;}
        void setxMax(int max){xmax=max;}
        float getxMax(){return xmax;}
        float getxMin(){return xmin;}
        void setyMin(int min){ymin1=min;}
        void setyMax(int max){ymax1=max;}
        float getyMin(){return ymin1;}
        float getyMax(){return ymax1;}
        void setGridFlg(bool flg){m_bGrid==flg;}
        bool getGridFlg(){return m_bGrid;}
        void setLegend(bool flg){m_bLegendOn=flg;}
        void setDataSetText(char *t1, char*t2, char *t3);
        void setDataSetColors(TColor c1,TColor c2,TColor c3);
        void setLegendBackColor(TColor c){m_cLegend=c;}
        void setNumDataSets(int n){m_iNumDataSets=n;}
        void setXDataFileSetFlg(bool flg){m_bXDataFileSet = flg;}
        void setSkipSetFlags(bool f1,bool f2,bool f3);
        void setDataPlotModes(int index1,int index2,int index3);
        void setBarWidth(int width){m_iBarWidth=width;}
        void setLegendPosition(int l,int t);
        void setxgrad(bool flg){xgrad=flg;}
        bool getxgrad(){return xgrad;}
        void redrawCanvas(int width,int height);
        void redrawCanvas1(int width,int height);
        void redrawWaveData();
        void redrawWaveData(floatvec& data);
        void redrawData();
        void redrawData1();
        void redrawData(floatvec& data);
        unsigned long int readWaveHeaderFile(FILE *fp);
        void setFormWidth(int width){Width=width;}
        void setFormHeight(int height){Height=height;}
        int getFormWidth(){return Width;}
        int getFormHeight(){return Height;}
        void setDataPlotColor(TColor color){m_cDataPlotColor=color;}
        TColor getDataPlotColor(){return m_cDataPlotColor;}
        void setPenColor(){Image1->Canvas->Pen->Color=m_cDataPlotColor;}
        TImage* getImage(){return Image1;}
        bool getHarmonicsFlag(){return m_bHarmonic;}
        void setHarmonicsFlag(bool flg){m_bHarmonic=flg;}
        //Utility functions for setting colours for graphic plots:
        TColor getBackground(){return m_cBackground;}
        void setBackground(TColor col){m_cBackground=col;}
        TColor getGridLines(){return m_cGridlines;}
        void setGridLines(TColor col){m_cGridlines=col;}
        TColor getSignal(){return m_cSignal;}
        void setSignal(TColor col){m_cSignal=col;}
        TColor getSpectrum(){return m_cSpectrum;}
        void setSpectrum(TColor col){m_cSpectrum=col;}
        TColor getHarmonics(){return m_cHarmonics;}
        void setHarmonics(TColor col){m_cHarmonics=col;}
        TColor getSFMarker(){return m_cSFmarker;}
        void setSFMarker(TColor col){m_cSFmarker=col;}
        TColor getOrigin(){return m_cOrigin;}
        void setOrigin(TColor col){m_cOrigin=col;}
        TColor getAudioMarker(){return m_cAudioMarkerColor;}
        void setAudioMarker(TColor col){m_cAudioMarkerColor=col;}
        void setAnimationMode(bool flg){m_bAnimated=flg;}
        bool getAnimationMode(){return m_bAnimated;}
        void AllowResize(bool flg){m_bAllowResize=flg;}

        //Audio members:
        bool checkForWaveFile();
        bool checkForIFTFile();
        void setWavefileFlag(bool flg){m_bIsWaveFile=flg;}
        bool getWaveFileFlag(){return m_bIsWaveFile;}
        void setWaveFileName(char *fname){strcpy(m_sFile,fname);}
        char* getWaveFileName(){return m_sFile;}
        void setSampleRate(int sr){m_iSampleRate=sr;}
        int getSampleRate(){return m_iSampleRate;}


protected:
   void __fastcall MMmciNotify(TMessage Message);



       BEGIN_MESSAGE_MAP
         MESSAGE_HANDLER(MM_MCINOTIFY,TMessage,MMmciNotify)
       END_MESSAGE_MAP(TForm)


};
//---------------------------------------------------------------------------
extern PACKAGE TGraphForm *GraphForm;
//---------------------------------------------------------------------------
#endif
