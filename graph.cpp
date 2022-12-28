//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "graph.h"
#include "global.hpp"
#include <stdio.h>
#include <math.h>
#include "wavetools.hpp"
#include <string.h>
#include <printers.hpp>
#include "Easy.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TGraphForm *GraphForm;

#ifdef ORI_WIN_CODE
//---------------------------------------------------------------------------
  //Contructor with a varying form size request
__fastcall TGraphForm::TGraphForm(TComponent* Owner,
                                  bool tMode,int pMode,int logMode,
                                  char *xlab, char *ylab, char *gphtitle,
                                  float yMin,float yMax,float xMin, float xMax,
                                  int xInt, int yInt, bool xGrad, bool yGrad,
                                  char *xlabel1, char *ylabel1, char *xlabel2,
                                  char *ylabel2,
                                  float yMin1,float yMax1,float yMin2,
                                  float yMax2,bool grid, int gInt,
                                  bool xshow,bool yshow,bool bitmap,
                                  TColor monobcolor,char *audfile, char *audInifile,
                                  int chans,int samrate, int bps,int formWidth,
                                  int formHeight,TBorderStyle bstyle,bool graphBorder,bool gphMenu)
        : TForm(Owner)
{

  BorderStyle = bstyle;

  //Define form color (to diferentiate from menu)
  //Will normally be the standard style bsSizeable
  //Will be bsNone if has no border - the case for embedded form in another visual app
  if(BorderStyle != bsNone)
  {
    Color = clInactiveBorder;  //(Default: clBtnFace)
  }
  else
  {
    Color = 0x5d5d5d;//0x3C3C3C;
  }

  m_bAnimated = false;
  m_bResize = false;
  m_bDrawBorder = graphBorder;

   //Instantiate image object:
   Image1 = new TImage(this);
   Image1->Parent = this;

   ymin1 = yMin;
   ymax1 = yMax;
   xmin = xMin;
   xmax = xMax;

   //Save linear definitions (in case transformed to log values):
   xminLinear = xmin;
   xmaxLinear = xmax;

   //Get flag for plotting harmonics on frequency spectrum
   m_bHarmonic = false;

   //Disable timer for printing progress:
   PrintTimer->Enabled = false;
   PrintTimer->Interval = 50;
   printer = NULL;
   //prntDlg = NULL;

   //Set audio player members:
   //AudioPlayer = NULL;
   strcpy(m_szAudioIniFile,audInifile);
   strcpy(m_sFile,audfile);
   m_iNumChannels = chans;
   m_iSampleRate = samrate;
   m_iBitsPerSample = bps;

   //For clipping (selecting) regions of graph
   m_cAudioClipMarkerColor = clLime;//clAqua;
   m_bLastClickWasDouble = false;

   //Disable playback functionality if file NOT a wave file:
   if(checkForWaveFile())
   {
      m_bIsWaveFile = true;
      Play1->Enabled = true;
   }
   else
   {
      m_bIsWaveFile = false;
      Play1->Enabled = false;
   }

   if(checkForIFTFile())
   {
      m_bIsWaveFile = true;
      Play1->Enabled = false;
   }

  //Redefine form title:
  Caption = "UtterSense Graphics";

   //Check for bitmap option:
   Bitmap = bitmap;

   //Define plot mode:
   plotMode = pMode;  //0:line, 1:pixel, 2:circle, 3:rectangle, 4: bar graph
   logPlotMode = logMode;  //0: default - no log plotting
                          //1: log plot: x axis
                          //2: log plot: y axis
                          //3: log plot: x and y axis

   //Set mode:
   monoMode = tMode;
   if(monoMode == true)
   {
      Image2->Visible = false;
      image = 1;


      //Re-initialise to requested form dimensions:
      Height = formHeight;
      Width = formWidth;
      //Position  top left corner:
      Top = 0;
      Left = 0;
      //Store original form dimensions on instantiation:
      m_iFormHeight = formHeight;
      m_iFormWidth = formWidth;

      //Set Allow resize flag:
      m_bAllowResize = true;

      //Set up dimensions of image
      setupImageSize();

      xincrement = 1; //Default value for pixel plot advances
      cirSize = 5;    //Initialise circle size for circle plots
      setLinePlotFlag(0);

      //Set  axis numbering defaults:
      showXAxisNums = xshow;
      showYAxisNums = yshow;

      //Set title:
      strcpy(Title,gphtitle);
      //Set axis labels:
      strcpy(xLabel,xlab);
      strcpy(yLabel,ylab);

      //Set flags for graduations:
      xgrad = xGrad;
      ygrad = yGrad;
      xnoDivs = xInt;
      ynoDivs = yInt;

      if(logPlotMode == 1)
      {
         xmin = log10(xmin);
         if(xmax!= 0)
            xmax = log10(xmax);
      }
      if(logPlotMode == 2)
      {
         ymin1 = log10(ymin1);
         if(ymax1!= 0)
            ymax1 = log10(ymax1);
      }
      if(logPlotMode == 3)
      {
         ymin1 = log10(ymin1);
         if(ymax1!= 0)
            ymax1 = log10(ymax1);
         xmin = log10(xmin);
         if(xmax!= 0)
            xmax = log10(xmax);
      }

     //Set up display graphic area:
     setupGraphSize();

      //Color background for mon graph:
      TRect rect(xmargin,ymargin,Image1->Width-xmargin,
                 Image1->Height-ymargin);
      MonoGraphRectangle = rect;
      setMonoGraphBackground(monobcolor);
      m_cMonoBackCol = monobcolor;


      setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                  Image1->Height-ymargin,xLabel,yLabel);






     //Draw line through origin:
     drawOriginLine();

     counter1 = 0.0;
     strcpy(xfield,"5.1");
     strcpy(yfield,"5.1");



     //Draw graduations:
     graduations();
     //Draw axis scales:
     if(!Bitmap) axisScales();

     Image1->Show();

   }
   else
   {
      //Set up form for dual trace:
      image = 1;

      xmin = xMin;
      xmax = xMax;
      ymin1 = yMin1;
      ymax1 = yMax1;
      ymin2 = yMin2;
      ymax2 = yMax2;

      showGrid = grid;
      gridInt = gInt;

      int imageLeftMargin = 10;
      int imageTopMargin = 10;
      Left = 0;
      Top = 0;
      Height = 450;
      Width = 635;
      Image1->Left = imageLeftMargin; //relative to form window
      Image1->Top = imageTopMargin; //relative to form window
      Image1->Height = 190;
      Image1->Width =  Width - 2*imageLeftMargin;
      Image1->Canvas->MoveTo(0,0);

      Image2->Left = imageLeftMargin;
      Image2->Top = 230;
      Image2->Height = Image1->Height;
      Image2->Width = Width - 2*imageLeftMargin;
      Image2->Canvas->MoveTo(0,0);

      //Set up outer rectangle window (top trace):
      xmargin = 20;
      ymargin = 20;

      left1 = xmargin;
      top1 = ymargin;
      right1 = Image1->Width-xmargin;
      bottom1 = Image1->Height-ymargin;
      bottom = bottom1;
      setupWindow(image,left1,top1,right1,bottom1,xlabel1,ylabel1);


      //Draw line through origin (top trace):
      C1 = ( ymax1/(ymax1-ymin1) )* (bottom1-top1);
      C1 = C1 + ymargin;  //Add margin offset
      Image1->Canvas->MoveTo(left1,C1);
      Image1->Canvas->LineTo(right1,C1);
      tagOrigin(image,left1,C1);


      //Set up outer rectangle window (bottom trace):
      image = 2;
      xmargin = 20;
      ymargin = 20;

      left2 = xmargin;
      top2 =  ymargin;
      right2 = Image2->Width-xmargin;
      bottom2 = Image2->Height-ymargin;
      bottom = bottom2;
      setupWindow(image,left2,top2,right2,bottom2,xlabel2,ylabel2);


      //Draw line through origin (bottom trace):
      C2 = ( ymax2/(ymax2-ymin2) )* (bottom2-top2);
      C2 = C2 + ymargin;   //Add yaxis margin
      Image2->Canvas->MoveTo(left2,C2);
      Image2->Canvas->LineTo(right2,C2);
      tagOrigin(image,left2,C2);

      counter1 = left1;
      counter2 = left2;
      buffCount1 = 0;
      buffCount2 = 0;
      for(int j=0;j<WINDOW_PIXEL_LENGTH;j++)
      {
         buffer1[j] = 0.0;
         buffer2[j] = 0.0;
      }//j
      xincrement = 1; //Default value for pixel plot advances
      cirSize = 5;    //Initialise circle size for circle plots
      setLinePlotFlag(0);


   }

   ymaxVal1 = ymin1;
   ymaxVal2 = ymin2;

   m_bLegendMove = true;
   //Set flag for disabling Graph Form Menu:
   m_bMenuDisable = gphMenu;
   if(m_bMenuDisable)
   {
	   if(MainMenu1 != NULL)
	   {
		  delete MainMenu1;
		  MainMenu1 = NULL;
	   }
   }


}

//---------------------------------------------------------------------------
void TGraphForm::setDataSetText(char *t1, char*t2, char *t3)
{
   strcpy(m_sLegendText1,t1);
   strcpy(m_sLegendText2,t2);
   strcpy(m_sLegendText3,t3);

}//setDataSetText

void TGraphForm::setSkipSetFlags(bool f1,bool f2,bool f3)
{
   m_bSkipSet[0] = f1;
   m_bSkipSet[1] = f2;
   m_bSkipSet[2] = f3;

}//setSkipSetFlags


void TGraphForm::setDataPlotModes(int index1,int index2,int index3)
{
   m_iPlotMode[0] = index1;
   m_iPlotMode[1] = index2;
   m_iPlotMode[2] = index3;

}//setDataPlotmodes



void TGraphForm::setDataSetColors(TColor c1,TColor c2,TColor c3)
{
   m_cDataSet[0] = c1;
   m_cDataSet[1] = c2;
   m_cDataSet[2] = c3;

}//setDataSetColors


void TGraphForm::setLegendPosition(int l,int t)
{
   m_iLegendLeftPos = l;
   m_iLegendTopPos = t;

}//setLegendPosition


void TGraphForm::setupImageSize()
{

   if(BorderStyle != bsNone)
   {
      //Adjust image margins accordingly:
      int imageLeftMargin = 10;
      int imageTopMargin = 15;
      int hMenuBar = 5;//30;    //Estimate on menu bar height


      Image1->Width = Width - 3*imageLeftMargin;
      Image1->Left =  (float)( ( (float)Width - (float)Image1->Width)/2.0);
      Image1->Top =  10 + hMenuBar;
      Image1->Height = Height - Image1->Top - 75;
   }
   else
   {
      //Form embedded in another visual app - with no border

      //Adjust image margins accordingly:
      int imageLeftMargin = 2;
      int imageTopMargin = 1;
      int hMenuBar = 5;//30;    //Estimate on menu bar height


      Image1->Width = Width - 2*imageLeftMargin;
      Image1->Left = imageLeftMargin;
      Image1->Top =  imageTopMargin;
      Image1->Height = Height - 25*imageTopMargin;

   }


}//setupImageSize
//---------------------------------------------------------------------------

void TGraphForm::setupGraphSize()
{
   if(BorderStyle != bsNone)
   {
      //Set up outer rectangle window:
      xmargin = 40;
      ymargin = 30;

      left = xmargin;
      left1 = left;
      top = ymargin;
      top1 = top;
      right = Image1->Width-xmargin;
      right1 = right;
      bottom = Image1->Height-ymargin;
      bottom1 = bottom;
   }
   else
   {
      //Set up outer rectangle window:
      xmargin = 10;
      ymargin = 25;

      left = xmargin;
      left1 = left;
      top = ymargin;
      top1 = top;
      right = Image1->Width-xmargin;
      right1 = right;
      bottom = Image1->Height-ymargin;
      bottom1 = bottom;

   }


}//setupGraphSize
//---------------------------------------------------------------------------

void TGraphForm::drawOriginLine()
{
   if(logPlotMode == 0 || logPlotMode == 1)
  {
      C1 = ( ymax1/(ymax1-ymin1) )* (bottom-top);
      C1 = C1 + ymargin;  //Add margin offset
      Image1->Canvas->MoveTo(left,C1);
      Image1->Canvas->LineTo(right,C1);
      tagOrigin(image,left,C1);
  }

}//drawOriginLine


void TGraphForm::drawHorLine(float pos)
{

      int width = (bottom-top) + 2*ymargin;
      C1 = ( (pos-ymin1)/(ymax1-ymin1) )* (bottom-top);
      C1 = C1 + ymargin;  //Add margin offset
      Image1->Canvas->MoveTo(left,width-C1);
      Image1->Canvas->LineTo(right,width-C1);


}//drawHorLine


void TGraphForm::drawVerLine(float pos)
{

      //int width = (right-left) + 2*xmargin;
      C1 = ( (pos-xmin)/(xmax-xmin) )* (right-left);
      C1 = C1 + xmargin;  //Add margin offset
      Image1->Canvas->MoveTo(C1,bottom);
      Image1->Canvas->LineTo(C1,top);


}//drawVerLine


void TGraphForm::drawBar(float y1,float y2,float x, int bwidth, TColor color)
{
      int X,X1,Y1,X2,Y2;

      int width = (bottom-top) + 2*ymargin;

      Y1 = ( (y1-ymin1)/(ymax1-ymin1) )* (bottom-top);
      Y2 = ( (y2-ymin1)/(ymax1-ymin1) )* (bottom-top);
      //Add margins
      Y1 = Y1 + ymargin;
      Y2 = Y2 + ymargin;
      Y1 = width-Y1;
      Y2 = width-Y2;

      //Get x coordinates for rectangle
      //X1 = ( ((x-bwidth/2.0) - xmin)/(xmax-xmin) )* (right-left);
      //X1 = X1 + xmargin;  //Add margin offset
      //X2 = ( ((x+bwidth/2.0) - xmin)/(xmax-xmin) )* (right-left);
      //X2 = X2 + xmargin;  //Add margin offset
      X = ( (x-xmin)/(xmax-xmin) )* (right-left);
      X1 = X - bwidth/2.0;
      X1 = X1 + xmargin;
      X2 = X + bwidth/2.0;
      X2 = X2 + xmargin;

      TRect rect(X1,Y2,X2,Y1);
      //Draw bar:
      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);

      //Reset brush to backgorund color:
      Image1->Canvas->Brush->Color = m_cMonoBackCol;


}//drawBar



void TGraphForm::drawArrow(float x, float y, int h, int w, TColor color, char *type)
{

   int X,Y;

   int width = (bottom-top) + 2*ymargin;

   Y = ( (y-ymin1)/(ymax1-ymin1) )* (bottom-top);
   //Add margins
   Y = Y  + ymargin;
   Y = width-Y;

   X = ( (x-xmin)/(xmax-xmin) )* (right-left);
   X = X + xmargin;

   //Define relative proportions of arrow head and arrow body:
   int arrowhead_h = h;
   int arrowhead_w = h*pow(5,0.5)/2.0;
   int arrowbody_w = 2* arrowhead_w;
   int arrowbody_h = 0.6*arrowhead_h;


   if( strcmp(type,"rARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y-h/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->MoveTo(X-h*pow(5,0.5)/2.0,Y-h/2.0);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X - h*pow(5,0.5)/4.0,Y, color,fsBorder);

      //Draw the arrow body:
       TRect rect( X - arrowhead_w - arrowbody_w, Y-arrowbody_h/2.0,
                  X - arrowhead_w, Y+arrowbody_h/2.0 );

      //Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);


   }//rARROW
   else if( strcmp(type,"lARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y-h/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->MoveTo(X+h*pow(5,0.5)/2.0,Y-h/2.0);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X + h*pow(5,0.5)/4.0,Y, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X + arrowhead_w + arrowbody_w, Y-arrowbody_h/2.0,
                  X + arrowhead_w, Y+arrowbody_h/2.0 );

      //Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);


   }//lARROW
   else if( strcmp(type,"uARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X-h/2.0,Y+h*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(X+h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X,Y+h*pow(5,0.5)/4.0, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X - arrowbody_h/2.0, Y + arrowhead_w,
                  X + arrowbody_h/2.0, Y + arrowhead_w + arrowbody_w );


      Image1->Canvas->FillRect(rect);


   }//uARROW
   else if( strcmp(type,"dARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X-h/2.0,Y-h*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(X+h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X,Y-h*pow(5,0.5)/4.0, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X - arrowbody_h/2.0, Y - arrowhead_w - arrowbody_w,
                  X + arrowbody_h/2.0, Y - arrowhead_w );

      Image1->Canvas->FillRect(rect);


   }//dARROW



}//drawArrow


void TGraphForm::drawBitmap(int image,float x,float y, std::string bitmapfile)
{

   int ypixel = positionPixel(image,ymin1,ymax1,y);
   int xpixel = positionPixel(image,xmin,xmax,x,1);

   //Convert to UniCode String for Canvas->Draw method:
   int len = bitmapfile.length();
   String bmpfile(bitmapfile.c_str(),len);
   Graphics::TBitmap *bmp = new Graphics::TBitmap;
	try
	{
		bmp->LoadFromFile(bmpfile);
		getImage()->Canvas->Draw(xpixel, ypixel, bmp);
	}
	__finally
	{
		delete bmp;
	}


}//drawBitmap



void TGraphForm::circle(int image,float x1,float y1,float x2,float y2)
{

   int x1pixel = positionPixel(image,xmin,xmax,x1,1);
   int y1pixel = positionPixel(image,ymin1,ymax1,y1,0);
   int x2pixel = positionPixel(image,xmin,xmax,x2,1);
   int y2pixel = positionPixel(image,ymin1,ymax1,y2,0);

	if(image == 1)Image1->Canvas->Ellipse(x1pixel,y1pixel,x2pixel,y2pixel);
	else if(image == 2)Image2->Canvas->Ellipse(x1pixel,y1pixel,x2pixel,y2pixel);



}//circle



void TGraphForm::setMonoGraphBackground(TColor color)
{
   Image1->Canvas->Brush->Color = color;
   Image1->Canvas->FillRect(MonoGraphRectangle);
   m_cMonoBackCol = color;

}//setMonoGraphBackGround

//---------------------------------------------------------------------------
void TGraphForm::setupWindow(int image,int left,int top,int right,
                             int bottom, char *xlabel, char *ylabel)
{
   if(image == 1)
   {
	  if(m_bDrawBorder)
	  {
		Image1->Canvas->Pen->Width = 1;
		Image1->Canvas->Pen->Color = clBlack;
		Image1->Canvas->Rectangle(left,top,right,bottom);
	  }
	  //Set font size for labels:
	  Image1->Canvas->Font->Color = clBlack;
	  Image1->Canvas->Font->Height = 18;

   }
   else
   {
      if(m_bDrawBorder)
	  {
		Image2->Canvas->Pen->Width = 1;
		Image2->Canvas->Pen->Color = clBlack;
		Image2->Canvas->Rectangle(left,top,right,bottom);
	  }
	  //Set font size for labels:
	  Image2->Canvas->Font->Color = clBlue;
	  Image2->Canvas->Font->Height = 18;

   }
   //Print title and axis labels:
   title(Title);
   xAxisText(image,xlabel);
   yAxisText(image,ylabel);


}//setupWindow


void TGraphForm::tagOrigin(int image,int left,int posOrig)
{

   //Mark horizontal position of origin:
   if(image == 1)
   {  //top trace
      Image1->Canvas->MoveTo(left-5,C1);
      Image1->Canvas->LineTo(left,C1);
   }
   else if(image == 2)
   { //bottom trace
     Image2->Canvas->MoveTo(left-5,C2);
     Image2->Canvas->LineTo(left,C2);

   }

}//tagOrigin



void TGraphForm::title(char *text)
{
   String str(text);
   int len = Image1->Width;
   int txtWidth = Image1->Canvas->TextWidth(str);
   int centralPos = (len-txtWidth)/2;
   int horPos = top-25;
   Image1->Canvas->Brush->Color = clWhite;
   Image1->Canvas->TextOut(centralPos,horPos,str);


}//title


void TGraphForm::xAxisText(int image,char *text)
{
   if(image == 1)
   {
      String str(text);
      int len = Image1->Width;
      int txtWidth = Image1->Canvas->TextWidth(str);
      int centralPos = (len-txtWidth)/2;
      int horPos = bottom1+15;
      if(monoMode == false)
      {
         horPos = bottom1+5;
         Image1->Canvas->Font->Height = 10;
      }
      Image1->Canvas->TextOut(centralPos,horPos,str);
   }
   else if(image == 2)
   {
      String str(text);
      int len = Image2->Width;
      int txtWidth = Image2->Canvas->TextWidth(str);
      int centralPos = (len-txtWidth)/2;
      int horPos = bottom2+5;
      Image2->Canvas->Font->Height = 10;
      Image2->Canvas->TextOut(centralPos,horPos,str);


   }

}//xAxisText


void TGraphForm::yAxisText(int image,char *text)
{
   if(image == 1)
   {
      String str(text);
      int len = bottom1-top1;
      int txtHeight = Image1->Canvas->TextHeight(str);
      int verLen = txtHeight*strlen(text);
      int centralPos = (len-verLen)/2;

      int pos = centralPos + ymargin;
      for(int i=0;i<strlen(text);i++)
      {
         String chr(text[i]);
         Image1->Canvas->TextOut(5,pos,chr);
         pos = pos+15;
      }//i
   }
   else if(image == 2)
   {
      String str(text);
      int len = bottom2-top2;
      int txtHeight = Image2->Canvas->TextHeight(str);
      int verLen = txtHeight*strlen(text);
      int centralPos = (len-verLen)/2;

      int pos = centralPos + ymargin;
      for(int i=0;i<strlen(text);i++)
      {
         String chr(text[i]);
         Image2->Canvas->TextOut(5,pos,chr);
         pos = pos+15;
      }//i

   }


}//yAxisText


void TGraphForm::plotAudioMarkerPos(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if((Image1->Canvas->Pixels[xpixel][ypixel]==m_cMonoBackCol)
             || (Image1->Canvas->Pixels[xpixel][ypixel]==m_cAudioClipMarkerColor) )
         {
            //Only write pixel if not in wave signal:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
         (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {
         //Erasing the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cAudioMarkerColor )
         {
            //Only erase pixel if audio marker color:
            Image1->Canvas->LineTo(xpixel,ypixel+1);

         }
      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotAudioMarkerPos (plot current marker for audio play position)


void TGraphForm::plotAudioMarkerPos1(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(!m_bSigRangeDefined)
         {
            //check for white signal:
            int tpixel = positionPixel(Image,ymin1,ymax1,ymin1);
            if( !(ypixel==tpixel)) //Only do after first pixel -for white signal may get confused with canvas
            {
               if(!m_bSigTrace) //Not got to lower bound of signal yet:
               {
                  if(Image1->Canvas->Pixels[xpixel][ypixel]==getDataPlotColor())
                  //if(Image1->Canvas->Pixels[xpixel][ypixel]==clNavy)
                  {
                     m_iSigLower = ypixel;
                     m_bSigTrace = true;
                  }

               }
               else    //Got to top of signal - now find upper y coordinate
               {
                  if(Image1->Canvas->Pixels[xpixel][ypixel]==m_cMonoBackCol)
                  {
                     m_iSigUpper = ypixel;
                     m_bSigRangeDefined = true;
                  }

               }
            }
         }
         //Write pixel line:
         Image1->Canvas->LineTo(xpixel,ypixel+1);

      }
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
         (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {

         //Erasing the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);

         if( (ypixel < m_iSigLower) && (ypixel > m_iSigUpper) )
         {
            //Repaint signal color:
            Image1->Canvas->Pen->Color = getDataPlotColor();
            Image1->Canvas->LineTo(xpixel,ypixel+1);
            //Reset to background color
            if(!m_bSelected)
               Image1->Canvas->Pen->Color = m_cMonoBackCol;
             else
               Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;


         }
         else
         {
            //Repaint background color
            if(!m_bSelected)
               Image1->Canvas->Pen->Color = m_cMonoBackCol;
             else
               Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }

      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotAudioMarkerPos1 (plot current marker for audio play position)




void TGraphForm::plotClipMarkerPos(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio clip position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cMonoBackCol )
         {
            //Only write pixel if not in wave signal:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
         //Erasing the audio clip position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cAudioClipMarkerColor )
         {
            //Only erase pixel if audio clip marker color:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotClipMarkerPos (plot current marker for clip position)



void TGraphForm::plotVerLine(float ySig,float xSig)
{

   //This routine plots a vertical line (along +yaxis) of given height at x position specified

   int Image = 1;  //Set for single graph plot

   if(logPlotMode == 1) xSig = log10(xSig);
   if(logPlotMode == 2) ySig = log10(ySig);
   if(logPlotMode == 3)
   {
      xSig = log10(xSig);
      ySig = log10(ySig);
   }

   int yminpixel = positionPixel(Image,ymin1,ymax1,0.0);
   int ymaxpixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   Image1->Canvas->MoveTo(xpixel,yminpixel);
   Image1->Canvas->LineTo(xpixel,ymaxpixel);

   //Reset pen position
   //Image1->Canvas->MoveTo(xpixel,yminpixel);

}//plotVerLine




void TGraphForm::plotSig(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   if(logPlotMode == 1) xSig = log10(xSig);
   if(logPlotMode == 2) ySig = log10(ySig);
   if(logPlotMode == 3)
   {
      xSig = log10(xSig);
      ySig = log10(ySig);
   }
   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);
   if(plotMode == 2)
   {
      int shift = cirSize/2.0;
      //Image1->Canvas->Ellipse(xpixel,ypixel,xpixel+cirSize,
      //                        ypixel+cirSize);
      Image1->Canvas->Ellipse(xpixel-shift,ypixel,xpixel+cirSize-shift,
                              ypixel+cirSize);

   }
   if(plotMode == 3)
   {
      int shift = cirSize/2.0;
      //Image1->Canvas->Rectangle(xpixel,ypixel,xpixel+cirSize,
      //                        ypixel+cirSize);
      Image1->Canvas->Rectangle(xpixel-shift,ypixel,xpixel+cirSize-shift,
                                ypixel+cirSize);
   }
   if(plotMode == 4)
   {
       //Plot a bar graph
       drawBar(ymin1,ySig,xSig,m_iBarWidth,m_cDataSet[0]);


   }
   if(plotMode == 1)
   {
      int pixelSize = 1;
      Image1->Canvas->MoveTo(xpixel,ypixel);
      Image1->Canvas->LineTo(xpixel,ypixel+1);
   }
   if(plotMode == 0)
   {
      if(lineFlag>0)
      {
         Image1->Canvas->MoveTo(sigStore1,sigStore2);
         Image1->Canvas->LineTo(xpixel,ypixel);
      }
      sigStore1 = xpixel;
      sigStore2 = ypixel;
      lineFlag = 1;
   }
   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotSig  (for single screen plotting)


bool TGraphForm::plotSig1(float signal)
{
   int Image = 1;  //Set for top trace:

   if(counter1 >= left1+WINDOW_PIXEL_LENGTH) return false;
   int ypixel = positionPixel(Image,ymin1,ymax1,signal);
   if(ypixel >= bottom1) return false;
   if(plotMode == 2)
   {
     Image1->Canvas->Ellipse(counter1,ypixel,counter1+cirSize,
                             ypixel+cirSize);
   }
   else if(plotMode == 1)
   {
      int pixelSize = 1;
      Image1->Canvas->MoveTo(counter1,ypixel);
      Image1->Canvas->LineTo(counter1+pixelSize,ypixel+pixelSize);
   }
   else if(plotMode == 0)
   {
     if(counter1>left1)
      {
         Image1->Canvas->MoveTo(counter1-1,sigStore1);
         Image1->Canvas->LineTo(counter1,ypixel);
      }
      sigStore1 =ypixel;


  }
  counter1 = counter1+xincrement;
  if(signal >ymaxVal1) ymaxVal1 = signal;

  return true;

}//plotSig1 (trace 1 of dual CRO)


void TGraphForm::animatedPlot1(float signal,bool stop)
{

   buffer1[WINDOW_PIXEL_LENGTH-1] = signal;
   dispBufferElems(signal,1);

   buffCount1++;
   if(buffCount1<WINDOW_PIXEL_LENGTH) return;
   Image1->Canvas->Pen->Color = clNavy;
   for(int j=0;j<WINDOW_PIXEL_LENGTH;j++)
   {
      plotSig1(buffer1[j]);
   }//j
   initialisePizPos();

   //Update view:
   Sleep(0);
   Update();

   if(!stop)
   {
      //Clear graph view:
      TRect newRect = Rect(left1+1,top1+1,right1-1,bottom1-1);
      Image1->Canvas->Brush->Color = clWhite;
      Image1->Canvas->FillRect(newRect);
      Image1->Canvas->Pen->Color = clBlack;
      Image1->Canvas->MoveTo(left1,C1);
      Image1->Canvas->LineTo(right1,C1);
      if(showGrid) grid(gridInt,gridInt);
   }

}//animatedPlot1


bool TGraphForm::plotSig2(float signal)
{
   int Image = 2;  //Set for top trace:

   if(counter2 >= left2+WINDOW_PIXEL_LENGTH) return false;
   int ypixel = positionPixel(Image,ymin2,ymax2,signal);
   if(ypixel >= bottom2) return false;
   if(plotMode == 2)
   {
     Image2->Canvas->Ellipse(counter2,ypixel,counter2+cirSize,
                             ypixel+cirSize);
   }
   else if(plotMode == 1)
   {
      int pixelSize = 1;
      Image2->Canvas->MoveTo(counter2,ypixel);
      Image2->Canvas->LineTo(counter2+pixelSize,ypixel+pixelSize);
   }
   else if(plotMode == 0)
   {
     if(counter2>left2)
      {
         Image2->Canvas->MoveTo(counter2-1,sigStore2);
         Image2->Canvas->LineTo(counter2,ypixel);
      }
      sigStore2 =ypixel;


  }
  counter2 = counter2+xincrement;
  if(signal >ymaxVal2) ymaxVal2 = signal;
  return true;

}//plotSig2   (trace 2 of CRO)


void TGraphForm::animatedPlot2(float signal,bool stop)
{

   buffer2[WINDOW_PIXEL_LENGTH-1] = signal;
   dispBufferElems(signal,2);

   buffCount2++;
   if(buffCount2<WINDOW_PIXEL_LENGTH) return;
   Image2->Canvas->Pen->Color = clNavy;
   for(int j=0;j<WINDOW_PIXEL_LENGTH;j++)
   {
      plotSig2(buffer2[j]);
   }//j
   initialisePizPos();

   //Update view:
   Sleep(0);
   Update();

   if(!stop)
   {
      //Clear graph view:
      TRect newRect = Rect(left2+1,top2+1,right2-1,bottom2-1);
      Image2->Canvas->Brush->Color = clWhite;
      Image2->Canvas->FillRect(newRect);
      Image2->Canvas->Pen->Color = clBlack;
      Image2->Canvas->MoveTo(left2,C1);
      Image2->Canvas->LineTo(right2,C1);
      if(showGrid) grid(gridInt,gridInt);
   }


}//animatedPlot2


void TGraphForm::dispBufferElems(float signal,int buffMode)
{
   for(int i=0;i<WINDOW_PIXEL_LENGTH-1;i++)
   {
     if(buffMode == 1) buffer1[i] = buffer1[i+1];
     else
      buffer2[i] = buffer2[i+1];
   }
   if(buffMode == 1) buffer1[WINDOW_PIXEL_LENGTH-1] = signal;
   else
      buffer2[WINDOW_PIXEL_LENGTH-1] = signal;


}//displayBufferElems



int TGraphForm::positionPixel(int image,float ymin,float ymax,float sig,
                              int pixelMode)
{
   int X,y;
   float R = ymax/(ymax-ymin);
   if(ymax==0) R = ymin/(ymin-ymax);
   if(pixelMode == 0)
   {                    //positioning along y axis
      if(image == 1) X = bottom1 - top1;
      else if(image == 2)
        X = bottom2 - top2;
      if(ymax==0) y = (fabs(sig)/(-ymin))*R*X;
      else
         y = (fabs(sig)/ymax)*R*X;
      if(sig>=0.0)
      {
         if(ymax != 0) y = (R*X)-y;
      }
      else
      {
         if(ymax != 0)
            y = y+(R*X);
      }
      y += ymargin;
   }//if
   if(pixelMode == 1)
   {                //positioning along x axis
      float R = ymax/(ymax-ymin);
      //if(ymax==0) R = ymin/(ymin-ymax);
      R = 1-R;
      if(image == 1) X = right1 - left1;
      else if(image == 2)
         X = right2 - left2;
      float orig = R*X;
      if(sig>=0.0)
      {
         if(ymax==0) y = (fabs(sig)/(-ymin))*(X-orig);
         else
            y = orig + (fabs(sig)/ymax)*(X-orig);
      }
      if(sig<0.0)
      {
         y = orig - (fabs(sig)/fabs(ymin))*orig;
      }
      y+= xmargin;
   }

   return y;

}//positionPixel


float TGraphForm::getXValFromPixPos(int npixels)
{

   int X;
   int XVal;

   if(image == 1)
   {
      X = right1 - left1;


      if(xmin==0)
      {
         XVal = ((npixels-xmargin)*xmax )/X;
         return XVal;
      }
      else if(xmax==0)
      {
        XVal = xmin - ((npixels-xmargin)*xmin )/X;
        return XVal;
      }
      else
      {
         float D = fabs(xmin) + fabs(xmax);
         float R = fabs(xmin)/D ;
         XVal = ((npixels-xmargin)*D)/X;
         if(XVal <= R*D)
         {
            XVal = xmin + XVal;
         }   
         else
         {
            XVal = XVal - R*D;
         }
         return XVal;
      }

   }


}//getXValueFromPixPos



void TGraphForm::text(int image,float x,float y,char *caption)
{
   String str(caption);

   int ypixel = positionPixel(image,ymin1,ymax1,y);
   int xpixel = positionPixel(image,xmin,xmax,x,1);
   if(image == 1)Image1->Canvas->TextOut(xpixel,ypixel,str);
   else if(image == 2)Image2->Canvas->TextOut(xpixel,ypixel,str);

}//text


void TGraphForm::legend(int l,int t,int nsets,char *t1,char *t2, char *t3,
                        TColor c1,TColor c2, TColor c3, TColor bck,
                        int pm1,int pm2,int pm3)
{
   char mes1[100], mes2[100],mes3[100];

   sprintf(mes1,t1);
   sprintf(mes2,t2);
   sprintf(mes3,t3);
   String Mes1(mes1);
   String Mes2(mes2);
   String Mes3(mes3);

   //int left = 50;
   //int top = 50;
   int left = l;
   int top = t;

   int shift = 90;   //Define shift to right for putting data plot symbol

   if(monoMode == false)
   {
      left = 30;
      top = 30;
   }

   //Set font size for labels:
   //Image1->Canvas->Font->Color = clNavy;
   Image1->Canvas->Font->Height = 10;
   //Write legend border:
   Image1->Canvas->Brush->Color = bck;
   Image1->Canvas->Rectangle(left-5,top-5,left+100,top+40);

   //Write captions:
   //Image1->Canvas->TextOut(left,top,Mes1);
   //Image1->Canvas->TextOutA(left,top+20,Mes2);
   
   if(nsets == 1)
   {
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

   }
   if(nsets == 2)
   {
     //First data set:
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

      //Second data set:
     Image1->Canvas->Font->Color = c2;
     Image1->Canvas->TextOut(left,top+12,Mes2);
     Image1->Canvas->Pen->Color = c2;
     setDataSymbol(left,top+12,pm2,shift);
   }
   if(nsets == 3)
   {
     //First data set:
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

     //Second data set:
     Image1->Canvas->Font->Color = c2;
     Image1->Canvas->TextOut(left,top+12,Mes2);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c2;
     setDataSymbol(left,top+12,pm2,shift);

     //Third data set:
     Image1->Canvas->Font->Color = c3;
     Image1->Canvas->TextOut(left,top+24,Mes3);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c3;
     setDataSymbol(left,top+24,pm3,shift);
   }

   //Legend for second trace (if dual trace):
   if(monoMode == false)
   {
      sprintf(mes1,"yaxisMax:  %5.1f",ymax2);
      sprintf(mes2,"ymaxValue: %5.1f",ymaxVal2);
      String Mes1(mes1);
      String Mes2(mes2);

      //Set font size for labels:
      Image2->Canvas->Font->Color = clNavy;
      Image2->Canvas->Font->Height = 10;
      //Write legend border:
      Image2->Canvas->Rectangle(left-5,top-5,left+100,top+40);
      //Write captions:
      Image2->Canvas->TextOut(left,top,Mes1);
      Image2->Canvas->TextOutA(left,top+20,Mes2);

   }

}//legend

void TGraphForm::setDataSymbol(int left, int top, int pmode,int shift)
{

        if(pmode == 2)  //Circle symbol data plot
        {
            Image1->Canvas->Ellipse(left+shift,top+4,left+shift+cirSize,
                                 top+4+cirSize);
        }
        if(pmode == 3)  //Rectangle symbol data plot
        {
           Image1->Canvas->Rectangle(left+shift,top+4,left+shift+cirSize,
                                   top+4+cirSize);
        }
        if(pmode == 0)  //Line plot
        {
             Image1->Canvas->Rectangle(left+(shift-8),top+5,left+(shift-8)+14,
                                   top+5+1);

        }

}//setDataSymbol

void TGraphForm::grid(int xInterval,int yInterval)
{
   verLines(xInterval);
   horLines(yInterval);

}//grid


void TGraphForm::graduations()
{
   int Image = 1;   //Set for mono graph plot:
   //Obtain vertical graduations:
   if(logPlotMode == 1 || logPlotMode == 3)
   {
      //log graduations:
      for(int i=xmin;i<=xmax-1; i+=1)
      {
          int x = positionPixel(Image,xmin,xmax,i,1);
          pixelVerLine(x,4);
          for(int n=2;n<10;n++)
          {
            float pos = log10( n*pow10(i) );
            x = positionPixel(Image,xmin,xmax,pos,1);
            pixelVerLine(x,4);
          }//n
      }//i

   }
   else
   {  //linear graduations
      if(xgrad)
      {
         float interval = (xmax-xmin)/xnoDivs;
         float pos = xmin;
         for(int i=0;i<=xnoDivs;i++)
         {
            int x = positionPixel(Image,xmin,xmax,pos,1);
            pixelVerLine(x,4);
            pos += interval;
         }//i
      }//if

   }

    //Obtain horizontal graduations:
   if(logPlotMode == 2 || logPlotMode == 3)
   {
      for(int i=ymin1;i<=ymax1-1;i+=1)
      {
         int y = positionPixel(Image,ymin1,ymax1,i,0);
         pixelHorLine(y,3);
         for(int n=2;n<10;n++)
         {
            float pos = log10( n*pow10(i) );
            y = positionPixel(Image,ymin1,ymax1,pos,0);
            pixelHorLine(y,3);
         }//n
      }//i
   }
   else
   {
      if(ygrad)
      {
         float interval = (ymax1-ymin1)/ynoDivs;
         float pos = ymin1;
         for(int i=0;i<=ynoDivs;i++)
         {
            int y = positionPixel(Image,ymin1,ymax1,pos,0);
            pixelHorLine(y,3);
            pos += interval;
         }//i
      }//if
   }

}//graduations


void TGraphForm::pixelVerLine(int x, int pixelIncr)
{
   for(int j=top;j<=bottom;j+=pixelIncr)
   {
      Image1->Canvas->MoveTo(x,j);
      Image1->Canvas->LineTo(x,j+1);
   }//j

}//pixelVerLine


void TGraphForm::pixelAudioMarkerLine(int x, int pixelIncr)
{

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=top;j<=bottom;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(x,j);
            if((Image1->Canvas->Pixels[x][j]==m_cMonoBackCol)
                || (Image1->Canvas->Pixels[x][j]==m_cAudioClipMarkerColor) )
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
         }//j
      }//if
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
            (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {
          //Erasing the audio position marker:
          for(int j=top;j<=bottom;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]== m_cAudioMarkerColor)
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
          }//j

      }
   }//if(plotMode==1)

}//pixelAudioMarkerLine




void TGraphForm::pixelClipMarkerLine(int x, int pixelIncr)
{
   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=top;j<=bottom;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]==m_cMonoBackCol )
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
         }//j
      }//if
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
          //Erasing the audio position marker:
          for(int j=top;j<=bottom;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]== m_cAudioClipMarkerColor)
            {
               //Only erase pixel if audio clip marker color:
                Image1->Canvas->LineTo(x,j+1);
            }
          }//j

      }


   }//if(plotMode==1)


}//pixelClipMarkerLine



void TGraphForm::pixelHorMarkerLine(int y, int pixelIncr)
{

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=left;j<=right;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(j,y);
            if(Image1->Canvas->Pixels[j][y]==m_cMonoBackCol )
            {
               //Only write pixel if not in signal:
               Image1->Canvas->LineTo(j+1,y);
            }
         }//j
      }//if
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
          //Erasing the audio position marker:
          for(int j=left;j<=right;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(j,y);
            if(Image1->Canvas->Pixels[j][y]== m_cAudioClipMarkerColor)
            {
               //Only erase pixel if audio clip marker color:
               Image1->Canvas->LineTo(j+1,y);
            }
          }//j

      }

   }//if(plotMode==1)


}//pixelHorMarkerLine


void TGraphForm::pixelHorLine(int y, int pixelIncr)
{
      for(int j=left;j<=right;j+=pixelIncr)
      {
         Image1->Canvas->MoveTo(j,y);
         Image1->Canvas->LineTo(j+1,y);
      }//j

}//pixelHorLine


void TGraphForm::verLines(int xInterval)
{
   for(int i=left1;i<=right1;i+=xInterval)
   {
      Image1->Canvas->MoveTo(i,top1);
      Image1->Canvas->LineTo(i,bottom1);
   }//i
   if(monoMode == false)
   {
     for(int i=left2;i<=right2;i+=xInterval)
     {
      Image2->Canvas->MoveTo(i,top2);
      Image2->Canvas->LineTo(i,bottom2);
     }//i

   }

}//verLines


void TGraphForm::horLines(int yInterval)
{
   //Produce horizontal lines at yInterval on each side of origin:
   for(int i=C1;i>=top1;i-=yInterval)
   {
      Image1->Canvas->MoveTo(left1,i);
      Image1->Canvas->LineTo(right1,i);
   }//i

   for(int i=C1;i<=bottom1;i+=yInterval)
   {
      Image1->Canvas->MoveTo(left1,i);
      Image1->Canvas->LineTo(right1,i);
   }//i

   if(monoMode == false)
   {
      for(int i=C2;i>=top2;i-=yInterval)
      {
         Image2->Canvas->MoveTo(left2,i);
         Image2->Canvas->LineTo(right2,i);
      }//i

      for(int i=C2;i<=bottom2;i+=yInterval)
      {
         Image2->Canvas->MoveTo(left2,i);
         Image2->Canvas->LineTo(right2,i);
      }//i
   }

}//horLines


void TGraphForm::xAxisScale()
{
   if(showXAxisNums)
   {
      int Image = 1;  //Set for mono graph:

      char numtext[3], powtext[3];

      sprintf(numtext,"%d",10);
      int horPos = (bottom-top)+12;

      //Obtain x axis scale:
      if(logPlotMode == 1 || logPlotMode == 3)
      {
         for(int i=xmin;i<=xmax;i+=1)
         {
            int x = positionPixel(Image,xmin,xmax,i,1);
            sprintf(powtext,"%d",i);
            x = x-30;
            Image1->Canvas->TextOut(x+15,horPos+25,numtext);
            Image1->Canvas->TextOut(x+28,horPos+20,powtext);
         }//i

      }
      else
      {
         int x,exp;
         char floatnumtext[6], field[6];
         char *s1 = "%", *s2 = "f";
         strcpy(field,s1);

         exp =  getExponent(xmax);
         strcpy(xfield,"5.2");   //temp
         strcat(field,xfield);
         strcat(field,s2);

         float interval = (xmax-xmin)/xnoDivs;
         float pos = xmin;

         Image1->Canvas->Font->Size = 8;

         for(int i=0;i<=xnoDivs;i++)
         {
            x = positionPixel(Image,xmin,xmax,pos,1);
            float posx = pos;
            for(int k=0;k<abs(exp);k++)
            {
               if(xmax > 1)
               {
                  posx/= 10;
               }
               else
               {
                 posx*= 10.0;
               }

            }
            sprintf(floatnumtext,field,posx);
            x = x-30;
            Image1->Canvas->TextOut(x+15,horPos+20,floatnumtext);
            pos += interval;

         }//i


         //Label with correct power of 10:
        Image1->Canvas->TextOut(x+45,horPos+20,"E");
         char tmp[10];
         itoa(exp,tmp,10);
         if(exp > 0)
         {
            char tempStr[20];
            sprintf(tempStr,"%s%s","+",tmp);
            strcpy(tmp,tempStr);
         }
         Image1->Canvas->TextOut(x+52,horPos+20,tmp);
      }
   }//if(ShowXAxisNums)

}//xAxisScale

int TGraphForm::getExponent(float num)
{
   int exp;
   exp = 0;
   bool check = true;
   if(num == 0.0)
       return 0;

   if(num < 0.0 )
   {
      //Get magnitude value for exponent calcs:
      num = -1.0*num;
   }

   if(num < 1.0)
   {
      while(check)
      {
        num = num*10.0;
        exp -= 1;
        if(num > 1.0)
        {
            check = false;
        }
      }//while
   }//if
   else
   {
      while(check)
      {
        num = num/10.0;
        if(num < 1.0)
        {
            check = false;
        }
        else
        {
            exp += 1;
        }
      }//while

   }

   return exp;

}//getExponent

void TGraphForm::yAxisScale()
{

   if(showYAxisNums)
   {
      int Image = 1;  //set for mono graph:

      char numtext[3], powtext[3], error[20];

      sprintf(numtext,"%d",10);
      int verPos = left-85;

      //Obtain y axis scale:
      if(logPlotMode == 2 || logPlotMode == 3)
      {
         for(int i = ymin1;i<=ymax1;i+=1)
         {
            int y = positionPixel(Image,ymin1,ymax1,i,0);
            sprintf(powtext,"%d",i);
            Image1->Canvas->TextOut(verPos+60,y-5,numtext);
            Image1->Canvas->TextOut(verPos+75,y-10,powtext);
         }//i
      }
      else
      {
         int y,exp;
         char floatnumtext[6], field[6];
         char *s1 = "%", *s2 = "f";
         strcpy(field,s1);

         exp =  getExponent(ymax1);
         strcpy(yfield,"5.2");   //temp
         strcat(field,yfield);
         strcat(field,s2);

         float interval = (ymax1-ymin1)/ynoDivs;
         float pos = ymin1;

         Image1->Canvas->Font->Size = 8;

         for(int i=0;i<=ynoDivs;i++)
         {
            y = positionPixel(Image,ymin1,ymax1,pos,0);
            float posy = pos;
            for(int k=0;k<abs(exp);k++)
            {
               if(ymax1 > 1)
               {
                  posy/= 10;
               }
               else
               {
                 posy*= 10.0;
               }

            }
            sprintf(floatnumtext,field,posy);
            Image1->Canvas->TextOut(verPos+60,y-6,floatnumtext);
            pos += interval;

         }//i

         //Label with correct power of 10:
         Image1->Canvas->TextOut(verPos+63,y-21,"E");
         char tmp[10];
         itoa(exp,tmp,10);
         if(exp > 0)
         {
            char tempStr[20];
            sprintf(tempStr,"%s%s","+",tmp);
            strcpy(tmp,tempStr);
         }
         Image1->Canvas->TextOut(verPos+70,y-21,tmp);

         //Reset to default value:
         //Image1->Canvas->Font->Size = 11;

      }
   }//if(showYAxisNums)

}//yAxisScale


void TGraphForm::axisDefaults(int xdivs,int ydivs, char *xspec,
                              char *yspec)
{
   xnoDivs = xdivs;   //Redefine no. scale divs. along x-axis
   ynoDivs = ydivs;   //Redefine no. scale divs. along y-axis
   strcpy(xfield,xspec);  //Redefine field specification: x scaling
   strcpy(yfield,yspec);  //Redefine field specification: y scaling

}//axisDefaults



void __fastcall TGraphForm::Print1Click(TObject *Sender)
{


  TPrintDialog *printdlg = new TPrintDialog(this);
  printdlg->Options << poPageNums << poSelection;

  char tmp[10];



  TPrinter *Prntr = Printer();
  printer = Prntr;

  if( printdlg->Execute() )
  {

     PrintTimer->Enabled = true;
     Prntr->BeginDoc();


      Prntr->Canvas->Draw(0,0,Image1->Picture->Graphic);

      //Printer()->Canvas->TextOut(0, 0, "Hello, world");

      Prntr->EndDoc();


  }


}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Save1Click(TObject *Sender)
{

   //Save graph to file:
   char old_dir[MAXPATH],new_dir[MAXPATH];

   char fileName[100];
   char *ps;

   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);


   if( (!strcmp(m_sFileExt,"spe")) || (!strcmp(m_sFileExt,"txt")) || (!strcmp(m_sFileExt,"dat")) )
   {
       strcpy(fileName,"");// Let user define filename in save dialog box
   }
   else
   {
      bool search = true;
      ps = strchr(Title,'.');
      while(search)
      {
        ps--;
        if( strchr(ps,'\\') )
        {
            search = false;
            ps++;
            strcpy(fileName,ps);
            bool scan = true;
            int i=0;
            while( scan )
            {
              if(fileName[i] == '.')
              {
                 scan = false;
                 fileName[i+1] = 'b';
                 fileName[i+2] = 'm';
                 fileName[i+3] = 'p';
                 fileName[i+4] = '\0';
              }
              else
              {
               i++;
              }
            }//while
        }
      }//while
   }

   SaveDialog1->DefaultExt = "bmp";
   SaveDialog1->Filter = "bmp";
   SaveDialog1->Title = "Save as bitmap file ...";
   SaveDialog1->FileName = fileName;
   if( SaveDialog1->Execute())
   {

      //Save bit map image to executable directory:
      Image1->Picture->SaveToFile(SaveDialog1->FileName);
      char mes[100];
      sprintf(mes,"%s%s","A bitmap image for this graphic has been saved as\n",
      SaveDialog1->FileName);
      MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
   }

   //Change back to original directory:
   if(chdir(old_dir) == -1)
   {
      MessageBox(NULL,"Problem changing to old dir!","",0);
   }

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::PrintTimerTimer(TObject *Sender)
{
   /*
   if(prntDlg == NULL)
   {
      prntDlg = new TPrintProgDlg(this);
      prntDlg->Show();
   }
   */
   if(printer->Printing)
   {
       char tmp[10];
       //prntDlg->Label5->Caption = "printing...";
       itoa(printer->PageNumber,tmp,10);
       //prntDlg->Label6->Caption = tmp;
       //prntDlg->Label7->Caption = printer->Printers->Strings[printer->PrinterIndex];
       strcpy(m_sPrinterName, (printer->Printers->Strings[printer->PrinterIndex]).c_str() );

   }
   else
   {
     //prntDlg->Label5->Caption = "completed.";
     //prntDlg->Label7->Caption = m_sPrinterName;
     if(printer != NULL)
     {
       printer = NULL;

     }
     //if(prntDlg != NULL)
     //{
       //prntDlg = NULL;
     //}

     PrintTimer->Enabled = false;

   }

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Exit1Click(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormResize(TObject *Sender)
{

  if(m_bAllowResize)
  {

      /* ------------------OLD VERSION -----------------------------------
      if(!Visible)
      {
          //Do nothing - form being created
      }
      else
      {
         if( (Height == m_iFormHeight) && (Width == m_iFormWidth) )
         {



         }
         else
         {

            if( !strcmp(m_sFile,"filcoeffs.txt") )
            {
               //Prevent resizing for this file:
               Height = m_iFormHeight;
               Width = m_iFormWidth;
               return;

            }

            //Redraw image:
            m_bResize = true;
            redrawCanvas(Width,Height);
            if(m_bIsWaveFile)
            {
               //Replot data:
               redrawWaveData();
            }
            else
            {
               redrawData();
            }
            m_bResize = false;
         }
       }
       ----------------------------------------------------------------- */

       if(!Visible)
      {
          //Do nothing - form being created
      }
      else
      {
         if( (Height == m_iFormHeight) && (Width == m_iFormWidth) )
         {



         }
         else
         {
            //Redraw image:
            m_bResize = true;

            if(m_bIsWaveFile)
            {
               //Replot data:
                redrawCanvas(Width,Height);
               redrawWaveData();
            }
            else
            {
               redrawCanvas1(Width,Height);
               redrawData1();
            }
            m_bResize = false;
         }
       }




  }
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Play1Click(TObject *Sender)
{

   int minSampleSize = 2;

   if(m_cAudioMarkerColor == clLime)   //Redefine to avoid problmes with audio clipper color
       m_cAudioMarkerColor = clRed;

   if(!m_bSelected)    //No area selected - play complete wavefile
   {


      //Check sample size:
      if(xmax < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

      //Enable graphic playback timer:
      Playback->Enabled = true;
      //Define interval for updating:
      int totSamples =  xmax;
      float timeper = (float)totSamples/(float)m_iSampleRate;
      if(timeper <= 3)
      {
         Playback->Interval = (timeper/10.0)*1000.0;
      }
      else
      {
         //Playback->Interval = ((timeper/10.0)*1000.0)/2.0;
         Playback->Interval = 500; //0.1sec
      }



      m_iCurrSampleNum = 0;
      changePlotMode(1); //pixel plot
      //m_cAudioMarkerColor = clRed;
      //m_cWaveSigColor = clBlack;


      /*
      //Commence play of audio wavefile
      TAudPlay *audioPlayer = new TAudPlay(this, Handle,
                                            m_iNumChannels,
                                            m_iSampleRate,
                                            m_iBitsPerSample,
                                            m_szAudioIniFile,Top,Left,
                                            true  //miniplayer toggle flag
                                            );
      AudioPlayer = audioPlayer;
      audioPlayer->SetPlayFromFileFlag(true);
      audioPlayer->SetWaveFileName(m_sFile);
      audioPlayer->Visible = false;
      audioPlayer->Playing();
      */
   }//if(!m_bSelected)
   else                       //Play selected region of wavefile
   {


      //Re-assign clip marker postions - start marker may have been dropped last
      if(m_fClipEndPos < m_fClipStartPos)
      {
         float tmp1;
         int tmp2;
         tmp1 = m_fClipStartPos;
         m_fClipStartPos = m_fClipEndPos;
         m_fClipEndPos = tmp1;
         tmp2 = m_iClipStartPixPos;
         m_iClipStartPixPos = m_iClipEndPixPos;
         m_iClipEndPixPos = tmp2;


      }

      //Check sample size:
      if(m_fClipEndPos - m_fClipStartPos < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

      //Get current directory:
      strcpy(m_sCurrDir,"X:\\");
      m_sCurrDir[0] = 'A' + getdisk();
      getcurdir(0,m_sCurrDir+3);

      sprintf(m_sClipWaveFile,"%s%s",m_sCurrDir,"\\clipped.wav");

     //Obtain wavefile for clipped region:
      floatvec clippedBuffer;
      clippedBuffer.reDim( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
      WAVETOOLS wavTools(m_iNumChannels,m_iSampleRate,
                         m_iBitsPerSample,0);

      wavTools.readWaveFile(m_sFile);
      //Get required region:
      clippedBuffer = wavTools.clipWaveFile(wavTools.getWaveBuffer(),
                                            (int) m_fClipStartPos,
                                            (int) m_fClipEndPos    );
     wavTools.upDateWaveBuffer(clippedBuffer);
     wavTools.setWaveFileName(m_sClipWaveFile);
     wavTools.makeWaveFile();



     //Enable graphic playback timer:
      Playback->Enabled = true;
      //Define interval for updating:

      int totSamples = ( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
      float timeper = (float)totSamples/(float)m_iSampleRate;
      if(timeper <= 3)
      {
         Playback->Interval = (timeper/10.0)*1000.0;
      }
      else
      {
         Playback->Interval = ((timeper/10.0)*1000.0)/2.0;
      }

      m_iCurrSampleNum = (int) m_fClipStartPos;
      changePlotMode(1); //pixel plot
      m_cAudioMarkerColor = clRed;
      //m_cWaveSigColor = clBlack;


      /*
      //Commence play of audio wavefile
      TAudPlay *audioPlayer = new TAudPlay(this, Handle,
                                            m_iNumChannels,
                                            m_iSampleRate,
                                            m_iBitsPerSample,
                                            m_szAudioIniFile,Top,Left,
                                            true  //miniplayer toggle flag
                                            );
      AudioPlayer = audioPlayer;
      audioPlayer->SetPlayFromFileFlag(true);
      audioPlayer->SetWaveFileName(m_sClipWaveFile);
      audioPlayer->Visible = false;
      audioPlayer->Playing();
      */

   }

}


unsigned long int TGraphForm::readWaveHeaderFile(FILE *fp)
{

   char a;
   unsigned long int size;
   unsigned int numberOfSamples;
   unsigned short wFormatTag,nBlockAlign,wBitsPerSample,nChannels,cbSize;
   unsigned long int nSamplesPerSec,nAvgBytesPerSec;
   short dataSample;


   //Read general file tag type (e.g. RIFF) and file size:
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index
   fread(&size,sizeof(unsigned long int),1,fp);

   //Read specific RIFF file type (e.g. WAVE):
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index

   //Read file format tag:
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index

   //Read 'fmt' info size:
   fread(&size,sizeof(unsigned long int),1,fp);

   //Read struct wave_format_tag (WAVEFORMATEX)
   fread(&wFormatTag,sizeof(unsigned short),1,fp);
   fread(&nChannels,sizeof(unsigned short),1,fp);
   fread(&nSamplesPerSec,sizeof(unsigned long int),1,fp);
   fread(&nAvgBytesPerSec,sizeof(unsigned long int),1,fp);
   fread(&nBlockAlign,sizeof(unsigned short),1,fp);
   fread(&wBitsPerSample,sizeof(unsigned short),1,fp);
   fread(&cbSize,sizeof(unsigned short),1,fp);

   m_iSampleRate =  nSamplesPerSec;


   if( cbSize == 0 )
   {
      //Process for normal 'data' or  'fact'/'data' headers
      fread(&a,sizeof(char),1,fp);
      if(a == 'd')
      {
         //Process for normal 'data' header
         for(int index=0;index<3;index++)
         {
            fread(&a,sizeof(char),1,fp);
         }//index
         //Read data size:
         fread(&size,sizeof(unsigned long int),1,fp);


         return size;

      }//if
      else if(a == 'f')
      {
         //Process for  'fact' /'data' combination header
         for(int index=0;index<15;index++)
         {
            fread(&a,sizeof(char),1,fp);
         }//index
         //Read data size:
         fread(&size,sizeof(unsigned long int),1,fp);

         return size;

      }//if
      else
      {
        //Format structure not supported
        MessageBox(NULL,"Audio file format not supported.",BEST_MESSAGE,
                   MB_ICONINFORMATION);

      }
   }
   else
   {
      //Process for UtterSense Application format (earlier versions):
      //Next two bytes should be 't' and 'a'  (first two chars read with cbSize)
      //Read in remainder of 'data' header:
      for(int index=0;index<2;index++)
      {
         fread(&a,sizeof(char),1,fp);
      }//index

      //Read data:
      //Read data size:
      fread(&size,sizeof(unsigned long int),1,fp);
      return size;


   }

}//readWaveHeaderFile


void TGraphForm::redrawCanvas(int width,int height)
{
   //Redraw as a result of form resizing:

   //Clear existing picture:
   if(Image1 == NULL)
   {
      return;
   }
   else
   {
      if(monoMode == true)
      {
         Image2->Visible = false;
         image = 1;
         //Instantiate new image object:

         //Only redraw if we need to resize? - then we can get rid of flashing
         //when changing in animated mode.
         if(m_bResize)
         {
            delete Image1;
            Image1 = new TImage(this);
            Image1->Parent = this;
         }

         //Set up dimensions of image
         setupImageSize();

         Image1->Canvas->MoveTo(0,0);

         Image1->Canvas->Brush->Color = clWhite;
         TRect rect(0,0,Image1->Width,Image1->Height);
         Image1->Canvas->FillRect(rect);


         xincrement = 1; //Default value for pixel plot advances
         cirSize = 5;    //Initialise circle size for circle plots
         setLinePlotFlag(0);

         //Stet up graphic display area:
         setupGraphSize();

         //Color background for mon graph:
         TRect rect1(xmargin,ymargin,Image1->Width-xmargin,
                    Image1->Height-ymargin);
         MonoGraphRectangle = rect1;
         setMonoGraphBackground(m_cMonoBackCol);


         setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                     Image1->Height-ymargin,xLabel,yLabel);


        //Draw line through origin:
        if(logPlotMode == 0 || logPlotMode == 1)
        {
            C1 = ( ymax1/(ymax1-ymin1) )* (bottom-top);
            C1 = C1 + ymargin;  //Add margin offset
            Image1->Canvas->MoveTo(left,C1);
            Image1->Canvas->LineTo(right,C1);
            tagOrigin(image,left,C1);
        }
        counter1 = 0.0;
        strcpy(xfield,"5.1");
        strcpy(yfield,"5.1");
        //Draw graduations:
        if(getGridFlg())
         graduations();
        //Draw axis scales:
        if(!Bitmap) axisScales();

      }

   }
}//redrawCanvas




void TGraphForm::redrawCanvas1(int width,int height)
{
   //Redraw as a result of form resizing:

   //Clear existing picture:
   if(Image1 == NULL)
   {
      return;
   }
   else
   {
      if(monoMode == true)
      {
         Image2->Visible = false;
         image = 1;
         //Instantiate new image object:

         //Only redraw if we need to resize? - then we can get rid of flashing
         //when changing in animated mode.
         if(m_bResize)
         {
            delete Image1;
            Image1 = new TImage(this);
            Image1->Parent = this;
         }

         //Set up dimensions of image
         setupImageSize();

         Image1->Canvas->MoveTo(0,0);

         Image1->Canvas->Brush->Color = clWhite;
         TRect rect(0,0,Image1->Width,Image1->Height);
         Image1->Canvas->FillRect(rect);


         xincrement = 1; //Default value for pixel plot advances
         cirSize = 5;    //Initialise circle size for circle plots
         setLinePlotFlag(0);

         //Stet up graphic display area:
         setupGraphSize();

         //Color background for mon graph:
         TRect rect1(xmargin,ymargin,Image1->Width-xmargin,
                    Image1->Height-ymargin);
         MonoGraphRectangle = rect1;
         setMonoGraphBackground(m_cMonoBackCol);


         setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                     Image1->Height-ymargin,xLabel,yLabel);


        //Define grid color:
      setDataPlotColor(m_cGridlines);
      setPenColor();
      graduations();
      //Redraw origin in required pen color:
      setDataPlotColor(m_cOrigin);
      setPenColor();
      drawOriginLine();

      //Redraw axis scales:
      axisScales();

      //Redraw legend, if required:
      if(m_bLegendOn)
      {
         //float ratio = (float)m_iLegendLeftPos/(float)m_iFormWidth;
         //int newleft = ratio*Width;
         legend(m_iLegendLeftPos,m_iLegendTopPos,m_iNumDataSets,
                m_sLegendText1, m_sLegendText2,m_sLegendText3,
                m_cDataSet[0],m_cDataSet[1],m_cDataSet[2],m_cLegend,
                m_iPlotMode[0],m_iPlotMode[1],m_iPlotMode[2]);
      }

      //Set color for plotting data:
      setDataPlotColor(m_cSpectrum);
      setPenColor();

      }

   }
}//redrawCanvas1



void TGraphForm::redrawData()
{
   floatmat data;
   int featureIndex, numFeatures;
   int endWordPos;
   char tmp[10], tmp1[10];

   //Plot recording data:
   if( (!strcmp(m_sFileExt,"txt")) )
   {
      data.textLoad(m_sFile);
   }
   else if( (!strcmp(m_sFileExt,"spe")) )
   {
      data.textLoad(m_sFile);
   }
   else if( (!strcmp(m_sFileExt,"fft")) )
   {
      data.textLoad(m_sFile);
      //Get square magnitude from complex data

      float r1,r2,max;
      data.SetPtrBegin();
      max = 0.0;
      for(int i=0;i<data.rows();i++)
      {
        r1 = *(data.curPtr());
        data++;
        r2 = *(data.curPtr());
        *(data.curPtr())= sqrt( pow( r1,2) + pow(r2,2) );
        if(*(data.curPtr()) > max)
           max = *(data.curPtr());
        data++;

      }//i
      //Normalise for plotting:
      if(max!=0.0)
      {
         data.normalize(0.75*(1.0/max));
      }

   }
   else
   {
      data.load(m_sFile);
   }

   if( !strcmp(m_sFileExt,"txt") )     //Plot r(t) file
   {
     //Plot data:
         //Define data plot color:
         setDataPlotColor(clWhite);
         setPenColor();

         data.SetPtrBegin();
         float x = 0.0;
         float max = 0.0;
         float yRange = max;
         float xRange = (float)data.rows();

         for(int i=0;i<data.rows();i++)
         {
            plotSig( *data.curPtr(),x);
            x += 1.0;
            if(i<data.rows()-1) data++;
         }//i

   }
   else if( !strcmp(m_sFileExt,"spe") || !strcmp(m_sFileExt,"fft") ) //Plot frequency spectrum
   {
      int sampleSize,xPlotStartVal;

      //Show graph form:
      float x = 0.0;
      float delta_f;
      float harmonic = 0.0;

      sampleSize = data.rows();
      //Define the spectral resolution:
      delta_f = (float) m_iSampleRate/sampleSize;

      //Set color for plotting data:
      setDataPlotColor(getSpectrum());
      setPenColor();

      setSFMarker(m_cSFmarker);


      float samfac = xmaxLinear/(float)m_iSampleRate;

      if(logPlotMode==1)
      {
         //Log plot
         xPlotStartVal = 1;
      }
      else if(logPlotMode==0)
      {
         xPlotStartVal = 0;
      }

      bool plot = true;
      while(plot)
      {
         data.SetPtrBegin();
         for(int j=xPlotStartVal;j<samfac*sampleSize;j++)
         {
            x = j*((float)m_iSampleRate/(float)sampleSize);
            x += harmonic*m_iSampleRate;
            if(j>sampleSize-1) break;
            if(x>1)
            {
               if(harmonic>0)
               {
                  setDataPlotColor(getHarmonics());
                  setPenColor();
               }
               if(x > xmaxLinear)
               {
                 plot = false;
                 break;
               }
               if(!strcmp(m_sFileExt,"spe"))
                  plotVerLine( *data(j,0),x);
               if(!strcmp(m_sFileExt,"fft"))
                  plotVerLine( *data(j,1),x);

            }

         }//j

         if(harmonic == 0.0)
         {
            if( (x >= m_iSampleRate - (1.5)*delta_f) && (x <= m_iSampleRate + (1.5)*delta_f) )
            {
               //Show sampling frequency:
               //Set color for plotting data:
               setDataPlotColor(getSFMarker());
               setPenColor();
               plotVerLine( 0.5*ymin1,x);
               plotVerLine( -0.5*ymin1,x);
               //Reset color for plotting data:
               setDataPlotColor(getSpectrum());
               setPenColor();
            }
         }
         if(m_bHarmonic == false) plot = false;
         harmonic+= 1.0;
      }//while

      m_bIsWaveFile = false;
      Play1->Enabled = false;

   }
   else
   {   //Plot cepstral coefficients:

      //Get maximum value for scaling:
      numFeatures = data.cols();

      //Hard code on feature index:
      featureIndex = 0;

      //Show graph form:
      float x = 0.0;
      data.SetPtrBegin();
      for(int j=0;j<featureIndex;j++) data++;
      for(int i=0;i<data.rows();i++)
      {
         plotSig( *data.curPtr(),x);
         x += 1.0;
         if(i<data.rows()-1)
         {
            for(int j=0;j<numFeatures;j++) data++;

         }
         //if(i<data.rows()-1) data++;
      }//i
   }

}//redrawData


void TGraphForm::redrawData1()
{
   floatmat m_fmData;

   //Plot first data set:
   m_fmData.load(m_sFile);

   m_fmData.SetPtrBegin();

   if(m_bXDataFileSet)
   {

      for(int i=1;i<=m_iNumDataSets;i++)
      {
         //Set  data plot mode type:
         changePlotMode(m_iPlotMode[i-1]);

         if(m_bSkipSet[i-1] == false)
         {
            //Set plot color:
            setDataPlotColor(m_cDataSet[i-1]);
            setPenColor();

            m_fmData.SetPtrBegin();
            for(int j=0;j<m_fmData.rows();j++)
            {
               float x,y;
               x = *m_fmData.curPtr();
               for(int jj=1;jj<=i;jj++) m_fmData++;
               y = *m_fmData.curPtr();
               plotSig(y,x);
               int numDataSetsLeft = m_iNumDataSets-i;
               for(int jj=1;jj<=numDataSetsLeft+1;jj++) m_fmData++;

            }//j
         }//m_bSkipSet[i-1]

         //Re-initialise to accommodate new line plots:
         setLinePlotFlag(0);
      }//i
   }
   else
   {
      //X data set not defined in dat file - TBD
      //N.B. Thsi can include .wav files

   }

}//redrawData1


void TGraphForm::redrawData(floatvec& data)  //fft only
{

   int sampleSize,xPlotStartVal;

   //Show graph form:
   float x = 0.0;
   float delta_f;
   float harmonic = 0.0;

   sampleSize = data.rows();
   //Define the spectral resolution:
   delta_f = (float) m_iSampleRate/sampleSize;

   //Define grid plot color:
   setDataPlotColor(getGridLines());
   setPenColor();
   graduations();
   //Redraw origin in required pen color:
   setDataPlotColor(getOrigin());
   setPenColor();
   drawOriginLine();

   //Set color for plotting data:
   setDataPlotColor(getSpectrum());
   setPenColor();

   float samfac = xmaxLinear/(float)m_iSampleRate;

   if(logPlotMode==1)
   {
      //Log plot
      xPlotStartVal = 1;
   }
   else if(logPlotMode==0)
   {
      xPlotStartVal = 0;
   }


   bool plot = true;
   while(plot)
   {
      data.SetPtrBegin();
      for(int j=xPlotStartVal;j<samfac*sampleSize;j++)
      {
         x = j*((float)m_iSampleRate/(float)sampleSize);
         x += harmonic*m_iSampleRate;
         if(j>sampleSize-1) break;
         if(x>1)
         {
            if(harmonic>0)
            {
               setDataPlotColor(getHarmonics());
               setPenColor();
            }
            if(x > xmaxLinear)
            {
              plot = false;
              break;
            }
            plotVerLine( *data(j,0),x);

         }

      }//j


      if(harmonic == 0.0)
      {
         if( (x >= m_iSampleRate - (1.5)*delta_f) && (x <= m_iSampleRate + (1.5)*delta_f) )
         {
            //Show sampling frequency:
            //Set color for plotting data:
            setDataPlotColor(getSFMarker());
            setPenColor();
            plotVerLine( 0.5*ymin1,x);
            plotVerLine( -0.5*ymin1,x);
            //Reset color for plotting data:
            setDataPlotColor(getSpectrum());
            setPenColor();
         }
      }
      if(m_bHarmonic == false) plot = false;
      harmonic+= 1.0;
   }//while

   m_bIsWaveFile = false;
   Play1->Enabled = false;

}//redrawData



void TGraphForm::redrawWaveData()
{

   FILE *fp;
   unsigned int numberOfSamples;
   short dataSample;

   if(checkForWaveFile())
   {
      //Read wave file header:
      if( (fp=fopen(m_sFile,"rb")) == NULL)
      {
        MessageBox(NULL,"Cannot open required wave file!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
        return;
      }//if

      //Instantiate graph.cpp and plot recording data:
      numberOfSamples = readWaveHeaderFile(fp);
      numberOfSamples = numberOfSamples/2;
      //N.B.: We are assuming 16-bit resolution here
      //      (2 bytes per sample);
      //numberOfSamples += 1;

      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;
      while( !feof(fp) )
      {
         fread(&dataSample,sizeof(short),1,fp);
         plotSig( (float)dataSample,x);
         x += 1;

      }//while

       fclose(fp);

      m_bIsWaveFile = true;
      Play1->Enabled = true;

   }//if
   if(checkForIFTFile())
   {
      floatvec iftspec;
      iftspec.load(m_sFile);
      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;
      iftspec.SetPtrBegin();
      for(int i=0;i<iftspec.rows();i++)
      {
         plotSig(*(iftspec.curPtr()),x);
         x += 1;
         iftspec++;

      }//i

   }//if

   m_bIsWaveFile = true;
   Play1->Enabled = true;

}//redrawWaveData
//---------------------------------------------------------------------------

void TGraphForm::redrawWaveData(floatvec& data) //Live data on stream
{
      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;

      data.SetPtrBegin();
      for(int i=0;i<data.rows();i++)
      {
         plotSig( *data.curPtr(),x);
         x += 1;
         data++;
      }

      m_bIsWaveFile = true;
      Play1->Enabled = true;

}//redrawWaveData


//---------------------------------------------------------------------------




bool TGraphForm::checkForWaveFile()
{
   char *pstr;
   int len;

   //Check the file extension:
   len = strlen(m_sFile);
   if(len==0) return false;     //No data file defined
   pstr = strchr(m_sFile,'.');
   pstr++;
   if( !strcmp(pstr,"wav") )
   {
      //Wave file flagged:
      return true;
   }
   else
   {
      strcpy(m_sFileExt,pstr);
      return false;
   }


}//checkForWaveFile





//---------------------------------------------------------------------------
bool TGraphForm::checkForIFTFile()
{
   char *pstr;
   int len;

   //Check the file extension:
   len = strlen(m_sFile);
   if(len==0) return false;     //No data file defined
   pstr = strchr(m_sFile,'.');
   pstr++;
   if( !strcmp(pstr,"ift") )
   {
      //Wave file flagged: - treat .ift like a wave file (time domain signal plot)
      strcpy(m_sFileExt,pstr);
      return true;
   }
   else
   {
      strcpy(m_sFileExt,pstr);
      return false;
   }


}//checkForIFTFile

//---------------------------------------------------------------------------



void TGraphForm::deSelectWaveClip()
{
   //Deselect wave file clip:

   //N.B.: This function is called  either by  a left double-click,
   //      OR  a right-click (this is better - more control and less likely
   //      to move mouse    if(m_bSelected)

   if(m_bSelected)
   {
      //Erase start clip marker
      pixelClipMarkerLine(m_iClipStartPixPos,1);

      //Erase end clip marker
      pixelClipMarkerLine(m_iClipEndPixPos,1);

     Image1->Canvas->Brush->Color = m_cMonoBackCol;
     //Fill upper region:
     Image1->Canvas->FloodFill(m_iClipStartPixPos,ymargin+2,m_cAudioClipMarkerColor,fsSurface);
     //Fill lower region:
     Image1->Canvas->FloodFill(m_iClipStartPixPos,Image1->Height-ymargin-2,m_cAudioClipMarkerColor,fsSurface);

      //Remove clip file from hard disc
      remove(m_sClipWaveFile);

      //Reset flags:
      m_bSelected = false;
      m_bLastClickWasDouble =true;

   }//if(m_bSelected)


}//deSelectWaveClip

//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(m_bLegendMove && !m_bIsWaveFile)
  {
	  //NEW CODE HERE------------------------------------------------------
	  //Draw hozizontal marker line if in left margin:
	  //Mouse is down:
	  //Get x value relating to this position:
	  if(X < Image1->Left)
	  {
		 int pixres = 1;//200;

		 m_iCurrYPixPos = Y - Image1->Top;
		 m_iLastYPixPos = m_iCurrYPixPos;

		 changePlotMode(1); //pixel plot
		 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
		 //Draw marker at this point:
		 pixelHorMarkerLine(m_iCurrYPixPos,1);
	  }
	  if( (Y > Image1->Top + Image1->Height) && (X > Image1->Left))
	  {


		if( Shift.Contains(ssLeft) )
		{

			 int pixres = 1;//200;

			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 m_iCurrXPixPos = X;
			 m_iLastXPixPos = m_iCurrXPixPos;
			 m_iClipStartPixPos = X;
			 m_fClipStartPos = getXValFromPixPos(X);
			 pixelClipMarkerLine(X,1);
		}
	  }

	 //END NEW CODE--------------------------------------------------------

	  return;
  }

  if(m_bIsWaveFile)
  {
	   //Initialise abort flag:
	   m_bAbortSelection = false;

	   //Redefine mouse position in terms of Image position:
	   X = X - Image1->Left;

	   //Check to see whether mouse is in graph area:
	   if( X>xmargin && X<Image1->Width-xmargin)
	   {
		  m_bAbortSelection = false;
	   }
	   else
	   {
		  m_bAbortSelection = true;
		  return;
	   }


	   if( Shift.Contains(ssDouble) )
	   {
			//Deselect current selected wave clip
			//deSelectWaveClip();

	   }
	   else  //Single mouse-down click
	   {

			 if( !Shift.Contains(ssLeft) )   //Right-click to perform de-selection
			 {
				//Deselect current selected wave clip
				deSelectWaveClip();

				return;
			 }

			 m_iCurrPixPos = X;
			 m_iLastPixPos = m_iCurrPixPos;

			 if(X <  xmargin ) return;
			  //Drop line to indicate start of wave clip:
			 if( !m_bSelected)
			 {
				changePlotMode(1); //pixel plot
				Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
				//Draw marker at this point:
				//Get x value relating to this position:
				m_iClipStartPixPos = X;
				m_fClipStartPos = getXValFromPixPos(X);

				pixelClipMarkerLine(X,1);

			 }//if( !m_bSelected)

	   }
  }//if(m_bIsWaveFile

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
      if(m_bLegendMove && !m_bIsWaveFile)
	  {
		 //Redraw image:
		 //Get new position for legend:
		 //Check to see if we are using bottom of form for positioning left horizontal position:

		 if(Y >= (Height-80) )
		 {
			//m_iLegendLeftPos = X - Image1->Left + 5;
			m_iLegendLeftPos = X - Image1->Left + 19;
			//m_iLegendTopPos = Y;
			redrawCanvas1(Width,Height);
			redrawData1();
			//m_bLegendMove = false;
		 }
		 //Check to see if we are using left of form for positioning vertical position:
		 if(X <= 80 )
		 {
			//m_iLegendLeftPos = X;
			m_iLegendTopPos = Y - Image1->Top + 5;
			redrawCanvas1(Width,Height);
			redrawData1();
			//m_bLegendMove = false;
		 }



		 return;

	  }
	  if(m_bIsWaveFile)
	  {
		  if(m_bAbortSelection) return;
		  //Redefine mouse position in terms of Image position:
		  X = X - Image1->Left;

			 if(!m_bSelected && m_bLastClickWasDouble)
			 {
				//Performing deselection:
				m_bLastClickWasDouble = false;

			 }
			 else if((!m_bSelected && !m_bLastClickWasDouble))
			 {

				Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
				//Get x value relating to this position:
				m_iClipEndPixPos = X;
				m_fClipEndPos = getXValFromPixPos(X);

				//Re-assign clip marker positions - start marker may have been dropped last
				if(m_fClipEndPos < m_fClipStartPos)
				{
				   float tmp1;
				   int tmp2;
				   tmp1 = m_fClipStartPos;
				   m_fClipStartPos = m_fClipEndPos;
				   m_fClipEndPos = tmp1;
				   tmp2 = m_iClipStartPixPos;
				   m_iClipStartPixPos = m_iClipEndPixPos;
				   m_iClipEndPixPos = tmp2;

				}

				//Check sample interval - if too small abort selection process:
			   if( m_iClipEndPixPos - m_iClipStartPixPos < 3 )
			   {

				   //Erase start clip marker
				   Image1->Canvas->Pen->Color = m_cMonoBackCol;
				   pixelClipMarkerLine(m_iClipStartPixPos,1);

				   //Erase end clip marker
				   pixelClipMarkerLine(m_iClipEndPixPos,1);
				   return;
			   }


				//Flag that clip region has been selected:
				m_bSelected = true;


				//Color selected area between start and end of clip positions:
			   Image1->Canvas->Brush->Color = m_cAudioClipMarkerColor;
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cMonoBackCol,fsSurface);

			   //Fill lower region:
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cMonoBackCol,fsSurface);

			 }//if(!m_bSelected)
			 else if((m_bSelected && !m_bLastClickWasDouble))
			 {
				//Color in new selected area:
				 //Color selected area between start and end of clip positions:
			   Image1->Canvas->Brush->Color = m_cAudioClipMarkerColor;
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cMonoBackCol,fsSurface);

			   //Fill lower region:
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cMonoBackCol,fsSurface);

			 }

	  }//if(m_bIsWaveFile

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TGraphForm::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{

   //NEW CODE----------------------------------------------------------------
   if(m_bLegendMove && !m_bIsWaveFile)
   {
	   if(X < Image1->Left) //In left margin, create horizontal line at Y position
	   {                    //For moving legend only

		  if(Shift.Contains(ssLeft) )
		  {

			 //Erase marker for previous marker position:
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelHorMarkerLine(m_iLastYPixPos,1);

			 //Plot marker for current marker position:
			 m_iCurrYPixPos = Y - Image1->Top;
			 m_iLastYPixPos = m_iCurrYPixPos;
			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 pixelHorMarkerLine(m_iCurrYPixPos,1);

		  }

	   }//if
	   if( (Y > Image1->Top + Image1->Height) && (X > Image1->Left))
	  {

		 if(Shift.Contains(ssLeft) )
		  {

			 int pixres = 1;//200;

			 //Erase marker for previous marker position:
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelHorMarkerLine(m_iLastYPixPos,1);
			 pixelClipMarkerLine(m_iLastXPixPos,1);

			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 m_iCurrXPixPos = X;
			 m_iLastXPixPos = m_iCurrXPixPos;
			 m_iClipStartPixPos = X;
			 m_fClipStartPos = getXValFromPixPos(X);
			 pixelClipMarkerLine(X,1);
		 }
	  }

	  return;


   }
   //END OF NEW CODE-------------------------------------------------------

   if(m_bIsWaveFile)
   {
	   if(m_bAbortSelection) return;


	   //Process for a vertical line at given X position:

	   //Redefine mouse position in terms of Image position:
	   X = X - Image1->Left;

	   if(!m_bSelected && Shift.Contains(ssLeft) )
	   {

		 //Mouse is down and we are still defining selected area:
		 //Get x value relating to this position:
		  int pixres = 1;//200;
		  m_iCurrPixPos = X;

		  //Erase marker for previous marker position:
		  if( (m_iLastPixPos != m_iClipStartPixPos) )
		  {
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelClipMarkerLine(m_iLastPixPos,1);
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

		  }



		  //Plot marker for current marker position:
		  pixelClipMarkerLine(X,1);
		  if(!m_bLegendMove) pixelClipMarkerLine(m_iClipStartPixPos,1);

		  m_iLastPixPos = m_iCurrPixPos;

	   }
	   else if(m_bSelected && Shift.Contains(ssLeft) )
	   {

		  //if( (X > m_iClipStartPixPos) && (X < m_iClipEndPixPos) )
		  //{

			//Area already selected - move area to another location:
			//Get x value relating to this position:

			 m_iCurrPixPos = X;


			 //Deselect current selected wave clip
			 //Erase start clip marker
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;
			 pixelClipMarkerLine(m_iClipStartPixPos,1);
			 //Erase end clip marker
			 pixelClipMarkerLine(m_iClipEndPixPos,1);

			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

			Image1->Canvas->Brush->Color = m_cMonoBackCol;
			//Fill upper region:
			Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cAudioClipMarkerColor,fsSurface);
			//Fill lower region:
			Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cAudioClipMarkerColor,fsSurface);



			//Obtain current marker positions:
			float m_fNewClipStartPos,m_fNewClipEndPos,m_iNewClipStartPixPos,m_iNewClipEndPixPos;

			m_iNewClipStartPixPos = m_iCurrPixPos;
			m_iNewClipEndPixPos =   m_iNewClipStartPixPos + (m_iClipEndPixPos-m_iClipStartPixPos);
			m_fNewClipStartPos = getXValFromPixPos(m_iNewClipStartPixPos);
			m_fNewClipEndPos = getXValFromPixPos(m_iNewClipEndPixPos);


			//Put markers in for current area:
			Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

			pixelClipMarkerLine(m_iNewClipStartPixPos,1);

			pixelClipMarkerLine(m_iNewClipEndPixPos,1);


			//Update start and end positions:
			m_iClipStartPixPos = m_iNewClipStartPixPos;
			m_iClipEndPixPos =   m_iNewClipEndPixPos;
			m_fClipStartPos =  m_fNewClipStartPos;
			m_fClipEndPos =  m_fNewClipEndPos;


			 m_iLastPixPos = m_iCurrPixPos;
		  //}
	   }
   }//if(m_bISWaveFile


}
//---------------------------------------------------------------------------






/*---------------------------------------------------------------------------
//Timer and Message handlers
----------------------------------------------------------------------------*/

void __fastcall TGraphForm::MMmciNotify(TMessage Message)
{

   if(Message.WParam == MCI_NOTIFY_SUCCESSFUL)
   {
      /*
      if(AudioPlayer->GetPauseStatus() == false)
      {
         //Playback has come to an end:
         AudioPlayer->SetDevicePlayStatus(false);
         AudioPlayer->StopPlay();
         if(AudioPlayer->Visible == false)
         {
            //If audioplayer in invisible mode (used for
            //playback with graphic animation)
            delete AudioPlayer;
            AudioPlayer = NULL;
            //playback->Enabled = false;

         }

      }
      else
      {
         //Allow playback to continue after pause
      }
      */
   }
   else if(Message.WParam == MCI_NOTIFY_ABORTED)
   {
      /*
      if(AudioPlayer->ResumePlayStatus() == false)
      {
         //Pause/resume play has not been enacted; abort as normal:
         AudioPlayer->SetDevicePlayStatus(false);
         AudioPlayer->StopModeView();
      }
      else
      {
         //Resume play has just been enacted; this sends an
         //abort message from the previous play command which
         //needs to be ignored. However, reset ResumePlay flag to
         //false:
         AudioPlayer->SetResumePlayStatus(false);
      }
      */
   }
   else if(Message.WParam == MCI_NOTIFY_SUPERSEDED)
   {

   }
   else if(Message.WParam == MCI_NOTIFY_FAILURE)
   {
      MessageBox(NULL,"Device Failure!",BEST_MESSAGE,MB_ICONINFORMATION);
   }

}//MMmciNotify


void __fastcall TGraphForm::PlaybackTimer(TObject *Sender)
{

   //Update current position of speech during audio playback:

   /*
   FILE *fp;
   fp = fopen("test.txt","at");
   fprintf(fp,"%d %d\n",m_iSigLower,m_iSigUpper);
   fclose(fp);
   */

   //Define line pixel resolution:
   int pixres = 100;
   //Offset from bottom of canvas to avoid problems with confuding signal with canvas
   int offset = 1000; //N.B.: This is specific to +-33000 x axis - make general for any axis
   if(!m_bSelected)             //Play back complete recording:
   {
      //First, remove previous marker position:
      if(m_iCurrSampleNum > 0)
      {

         Image1->Canvas->Pen->Color = m_cMonoBackCol;
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            //plotAudioMarkerPos(i,m_iCurrSampleNum);
            plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
       }//if

      //Draw marker at this point:
      Image1->Canvas->Pen->Color = m_cAudioMarkerColor;
      m_iCurrSampleNum += (((float)Playback->Interval)*((float)m_iSampleRate))/1000.0;
      if(m_iCurrSampleNum < xmax)
      {

         m_iSigUpper = 0;
         m_iSigLower = 0;
         m_bSigTrace = false;
         m_bSigRangeDefined = false;
         //Draw marker at this point:
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            //plotAudioMarkerPos(i,m_iCurrSampleNum);
            plotAudioMarkerPos1(i,m_iCurrSampleNum);
         }
      }
      else
      {
         Playback->Enabled = false;
      }
   }//if(!m_bSelected)
   else                              //Selection made: play back required clip
   {

      //First, remove previous marker position:
      if(m_iCurrSampleNum > 0)
      {
         Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            plotAudioMarkerPos(i,m_iCurrSampleNum);
            //plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
       }//if

      //Draw marker at this point:
      Image1->Canvas->Pen->Color = m_cAudioMarkerColor;
      m_iCurrSampleNum += (((float)Playback->Interval)*((float)m_iSampleRate))/1000.0;
      if(m_iCurrSampleNum < (int) m_fClipEndPos)
      {
         m_iSigUpper = 0;
         m_iSigLower = 0;
         m_bSigTrace = false;
         m_bSigRangeDefined = false;
         //Draw marker at this point:
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
      }
      else
      {
         Playback->Enabled = false;
      }


   }



}
//---------------------------------------------------------------------------



void __fastcall TGraphForm::SelectedRegion1Click(TObject *Sender)
{
  //Create wav file out of current selected region:

  char old_dir[MAXPATH],new_dir[MAXPATH];

  //Get current directory:
  strcpy(old_dir,"X:\\");
  old_dir[0] = 'A' + getdisk();
  getcurdir(0,old_dir+3);

  int minSampleSize = 2;

   if(m_bSelected)     //Range is currently selected
   {

      //Check sample size:
      if(m_fClipEndPos - m_fClipStartPos < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

     //Open save dialog box to define file:
     SaveDialog1->DefaultExt = "wav";
     SaveDialog1->Filter = "wav";
     SaveDialog1->Title = "Save selected signal range to ...";
     SaveDialog1->FileName = "*.wav";
     if( SaveDialog1->Execute())
     {

         //Obtain wavefile for clipped region:
         floatvec clippedBuffer;
         clippedBuffer.reDim( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
         WAVETOOLS wavTools(m_iNumChannels,m_iSampleRate,
                            m_iBitsPerSample,0);

         wavTools.readWaveFile(m_sFile);
         //Get required region:
         clippedBuffer = wavTools.clipWaveFile(wavTools.getWaveBuffer(),
                                               (int) m_fClipStartPos,
                                               (int) m_fClipEndPos    );
        wavTools.upDateWaveBuffer(clippedBuffer);
        wavTools.setWaveFileName( (SaveDialog1->FileName).c_str() );
        wavTools.makeWaveFile();
        MessageBox(NULL,"Selected region has been saved as audio file.",
                   BEST_MESSAGE,MB_ICONINFORMATION);
     }
     else
     {

     }

     //Change back to original directory:
     if(chdir(old_dir) == -1)
     {
        MessageBox(NULL,"Problem changing to old dir!","",0);
     }

   }//if(m_bSelected)

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormDblClick(TObject *Sender)
{
   //Double click to move the legend
   if(!m_bIsWaveFile)     //Only operate double clicks for non-wave files:
   {
      m_bLegendMove = true;
   }
}

void __fastcall TGraphForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	//Toggle to delete (close) the forn
	//(for use when requiring screen capture, and the Menu and top border controls
	// are disabled)
    switch( Key )
	{

		case VK_DELETE:
		Close();
		break;

	}


}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// INTEGRATION OF LINUX GRAPHICS MODULE STARTS HERE
#else
//---------------------------------------------------------------------------

// COMMON LINUX METHODS START HERE ------------------------------------------

  //Contructor with a varying form size request
__fastcall TGraphForm::TGraphForm(TComponent* Owner,
                                  int formWidth,int formHeight,
                                  int pMode,int logMode,
                                  char *xlab, char *ylab, char *gphtitle,
                                  float yMin,float yMax,float xMin, float xMax,
                                  int xInt, int yInt, bool xGrad, bool yGrad,
                                  int XAxisPrec, int YAxisPrec,
                                  bool grid, int gInt,
                                  bool xshow,bool yshow,bool ori, TColor ori_color,
                                  bool bitmap,
                                  TColor monobcolor,char *audfile, char *audInifile,
                                  int chans,int samrate, int bps, TBorderStyle bstyle,
                                  bool graphBorder,bool gphMenu)
        : TForm(Owner)
{

    //--------------------------------------------------
    //NB: THIS VERSION ONLY SUPPORTS MONO-MODE
   //--------------------------------------------------

  BorderStyle = bstyle;

  //Define form color (to diferentiate from menu)
  //Will normally be the standard style bsSizeable
  //Will be bsNone if has no border - the case for embedded form in another visual app
  if(BorderStyle != bsNone)
  {
    Color = clInactiveBorder;  //(Default: clBtnFace)
  }
  else
  {
    Color = 0x5d5d5d;//0x3C3C3C;
  }

  m_bAnimated = false;
  m_bResize = false;

  m_bDrawBorder = graphBorder;

   //Instantiate image object:
   Image1 = new TImage(this);
   Image1->Parent = this;

   ymin1 = yMin;
   ymax1 = yMax;
   xmin = xMin;
   xmax = xMax;

   //Save linear definitions (in case transformed to log values):
   xminLinear = xmin;
   xmaxLinear = xmax;

   //Get flag for plotting harmonics on frequency spectrum
   m_bHarmonic = false;

   //Disable timer for printing progress:
   PrintTimer->Enabled = false;
   PrintTimer->Interval = 50;
   printer = NULL;
   //prntDlg = NULL;

   //Set audio player members:
   //AudioPlayer = NULL;
   strcpy(m_szAudioIniFile,audInifile);
   strcpy(m_sFile,audfile);
   m_iNumChannels = chans;
   m_iSampleRate = samrate;
   m_iBitsPerSample = bps;

   //For clipping (selecting) regions of graph
   m_cAudioClipMarkerColor = clLime;//clAqua;
   m_bLastClickWasDouble = false;

   //Disable playback functionality if file NOT a wave file:
   if(checkForWaveFile())
   {
      m_bIsWaveFile = true;
      Play1->Enabled = true;
   }
   else
   {
      m_bIsWaveFile = false;
      Play1->Enabled = false;
   }

   if(checkForIFTFile())
   {
      m_bIsWaveFile = true;
      Play1->Enabled = false;
   }

   strcpy(m_sFile,filename);
   checkForCsvFile();   //Defines the file extension

  //Redefine form title:
  Caption = "UtterSense Graphics";

   //Check for bitmap option:
   Bitmap = bitmap;

   //Define plot mode:
   plotMode = pMode;  //0:line, 1:pixel, 2:circle, 3:rectangle, 4: bar graph
   logPlotMode = logMode;  //0: default - no log plotting
                          //1: log plot: x axis
                          //2: log plot: y axis
                          //3: log plot: x and y axis



   monoMode = true;
   image = 1;

   //Re-initialise to requested form dimensions:
   Height = formHeight;
   Width = formWidth;
   //Position  top left corner:
   Top = 0;
   Left = 0;
   //Store original form dimensions on instantiation:
   m_iFormHeight = formHeight;
   m_iFormWidth = formWidth;

   //Set Allow resize flag:
   m_bAllowResize = true;

   //Set up dimensions of image
   setupImageSize();

   xincrement = 1; //Default value for pixel plot advances
   cirSize = 5;    //Initialise circle size for circle plots
   setLinePlotFlag(0);

   //Set  axis numbering defaults:
   showXAxisNums = xshow;
   showYAxisNums = yshow;

   //Set title:
   strcpy(Title,gphtitle);
   //Set axis labels:
   strcpy(xLabel,xlab);
   strcpy(yLabel,ylab);

   //Set flags for graduations:
   xgrad = xGrad;
   ygrad = yGrad;
   xnoDivs = xInt;
   ynoDivs = yInt;

   showGrid = grid;
   showXGradLines = xGrad;
   showYGradLines = yGrad;


   if(logPlotMode == 1)
   {
      xmin = log10(xmin);
      if(xmax!= 0)
         xmax = log10(xmax);
   }
   if(logPlotMode == 2)
   {
      ymin1 = log10(ymin1);
      if(ymax1!= 0)
         ymax1 = log10(ymax1);
   }
   if(logPlotMode == 3)
   {
      ymin1 = log10(ymin1);
      if(ymax1!= 0)
         ymax1 = log10(ymax1);
      xmin = log10(xmin);
      if(xmax!= 0)
         xmax = log10(xmax);
   }


   //Set up display graphic area:
   setupGraphSize();

   //Color background for mon graph:
    TRect rect(xmargin,ymargin,Image1->Width-xmargin,
               Image1->Height-ymargin);
    MonoGraphRectangle = rect;
    setMonoGraphBackground(monobcolor);
    m_cMonoBackCol = monobcolor;

    setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                Image1->Height-ymargin,xLabel,yLabel);


   counter1 = 0.0;

  // Define decimal precision for number labelling on x and axes:
  char prec = XAxisPrec + '0';
  xfield[0] = '5';
  xfield[1] = '.';
  xfield[2] = prec;


  prec = YAxisPrec + '0';
  yfield[0] = '5';
  yfield[1] = '.';
  yfield[2] = prec;


  //Draw graduations:
  if(showGrid)
  {
      if(m_cMonoBackCol == clBlack)
      {
         graduations(clWhite);
      }
      if(m_cMonoBackCol == clWhite)
      {
         graduations(clBlack);
      }
  }


  // Set default for the line width plot (in pixels)
  line_width = 1;
  lineFlag = 0; //Initiliase for start of line plots

  //Draw axis scales:
  if(!Bitmap) axisScales();

  //Draw origin line:
  show_ori_line = ori;
  origin_color = ori_color;


  if(show_ori_line)
  {
      Image1->Canvas->Brush->Color = origin_color;
      drawOriginLine();
  }



  m_bLegendMove = true;
  //Set flag for disabling Graph Form Menu:
  m_bMenuDisable = gphMenu;
  if(m_bMenuDisable)
  {
     if(MainMenu1 != NULL)
     {
  	     delete MainMenu1;
		 MainMenu1 = NULL;
	 }
  }

  Image1->Show();


}

//---------------------------------------------------------------------------

void TGraphForm::setupGraphSize()
{
   if(BorderStyle != bsNone)
   {
      //Set up outer rectangle window:
      xmargin = 40;
      ymargin = 30;

      left = xmargin;
      left1 = left;
      top = ymargin;
      top1 = top;
      right = Image1->Width-xmargin;
      right1 = right;
      bottom = Image1->Height-ymargin;
      bottom1 = bottom;
   }
   else
   {
      //Set up outer rectangle window:
      xmargin = 10;
      ymargin = 25;

      left = xmargin;
      left1 = left;
      top = ymargin;
      top1 = top;
      right = Image1->Width-xmargin;
      right1 = right;
      bottom = Image1->Height-ymargin;
      bottom1 = bottom;

   }


}//setupGraphSize
//---------------------------------------------------------------------------

void TGraphForm::drawOriginLine()
{
   if(logPlotMode == 0 || logPlotMode == 1)
  {
      C1 = ( ymax1/(ymax1-ymin1) )* (bottom-top);
      C1 = C1 + ymargin;  //Add margin offset
      Image1->Canvas->MoveTo(left,C1);
      Image1->Canvas->LineTo(right,C1);
      tagOrigin(image,left,C1);
  }

}//drawOriginLine


void TGraphForm::drawHorLine(float pos)
{

      int width = (bottom-top) + 2*ymargin;
      C1 = ( (pos-ymin1)/(ymax1-ymin1) )* (bottom-top);
      C1 = C1 + ymargin;  //Add margin offset
      Image1->Canvas->MoveTo(left,width-C1);
      Image1->Canvas->LineTo(right,width-C1);


}//drawHorLine


void TGraphForm::drawVerLine(float pos)
{

      //int width = (right-left) + 2*xmargin;
      C1 = ( (pos-xmin)/(xmax-xmin) )* (right-left);
      C1 = C1 + xmargin;  //Add margin offset
      Image1->Canvas->MoveTo(C1,bottom);
      Image1->Canvas->LineTo(C1,top);


}//drawVerLine


void TGraphForm::setMonoGraphBackground(TColor color)
{
   Image1->Canvas->Brush->Color = color;
   Image1->Canvas->FillRect(MonoGraphRectangle);
   m_cMonoBackCol = color;


}//setMonoGraphBackGround

//---------------------------------------------------------------------------
void TGraphForm::setupWindow(int image,int left,int top,int right,
                             int bottom, char *xlabel, char *ylabel)
{

   if(image == 1)
   {
	  if(m_bDrawBorder)
	  {
		Image1->Canvas->Pen->Width = 1;
		Image1->Canvas->Pen->Color = clBlack;
		Image1->Canvas->Rectangle(left,top,right,bottom);
	  }
	  //Set font size for labels:
	  Image1->Canvas->Font->Color = clBlack;
	  Image1->Canvas->Font->Height = 18;

   }
   else
   {
      if(m_bDrawBorder)
	  {
		Image2->Canvas->Pen->Width = 1;
		Image2->Canvas->Pen->Color = clBlack;
		Image2->Canvas->Rectangle(left,top,right,bottom);
	  }
	  //Set font size for labels:
	  Image2->Canvas->Font->Color = clBlue;
	  Image2->Canvas->Font->Height = 18;

   }
   //Print title and axis labels:
   title(Title);
   xAxisText(image,xlabel);
   yAxisText(image,ylabel);



}//setupWindow


void TGraphForm::tagOrigin(int image,int left,int posOrig)
{

   //Mark horizontal position of origin:
   if(image == 1)
   {  //top trace
      Image1->Canvas->MoveTo(left-5,C1);
      Image1->Canvas->LineTo(left,C1);
   }
   else if(image == 2)
   { //bottom trace
     Image2->Canvas->MoveTo(left-5,C2);
     Image2->Canvas->LineTo(left,C2);

   }

}//tagOrigin



void TGraphForm::title(char *text)
{
   String str(text);
   int len = Image1->Width;
   int txtWidth = Image1->Canvas->TextWidth(str);
   int centralPos = (len-txtWidth)/2;
   int horPos = top-25;
   Image1->Canvas->Brush->Color = clWhite;
   //Image1->Canvas->Font->Style = TFontStyles() << fsBold << fsItalic << fsUnderline;
   Image1->Canvas->Font->Style = TFontStyles() << fsBold;
   Image1->Canvas->TextOut(centralPos,horPos,str);
   //Revert back to default (regular)
   Image1->Canvas->Font->Style = TFontStyles();

}//title


void TGraphForm::xAxisText(int image,char *text)
{
   if(image == 1)
   {
      String str(text);
      int len = Image1->Width;
      int txtWidth = Image1->Canvas->TextWidth(str);
      int centralPos = (len-txtWidth)/2;
      int horPos = bottom1+15;
      if(monoMode == false)
      {
         horPos = bottom1+5;
         Image1->Canvas->Font->Height = 10;
      }
      Image1->Canvas->Font->Style = TFontStyles() << fsBold;
      Image1->Canvas->TextOut(centralPos,horPos,str);
      //Revert back to default (regular)
      Image1->Canvas->Font->Style = TFontStyles();

   }
   else if(image == 2)
   {
      String str(text);
      int len = Image2->Width;
      int txtWidth = Image2->Canvas->TextWidth(str);
      int centralPos = (len-txtWidth)/2;
      int horPos = bottom2+5;
      Image2->Canvas->Font->Height = 10;
      Image1->Canvas->Font->Style = TFontStyles() << fsBold;
      Image2->Canvas->TextOut(centralPos,horPos,str);
      //Revert back to default (regular)
      Image1->Canvas->Font->Style = TFontStyles();


   }

}//xAxisText


void TGraphForm::yAxisText(int image,char *text)
{
   if(image == 1)
   {
      String str(text);
      int len = bottom1-top1;
      int txtHeight = Image1->Canvas->TextHeight(str);
      int verLen = txtHeight*strlen(text);
      int centralPos = (len-verLen)/2;

      int pos = centralPos + ymargin;
      Image1->Canvas->Font->Style = TFontStyles() << fsBold;
      for(int i=0;i<strlen(text);i++)
      {
         String chr(text[i]);

         Image1->Canvas->TextOut(5,pos,chr);
         pos = pos+15;
      }//i
      //Revert back to default (regular)
      Image1->Canvas->Font->Style = TFontStyles();
   }
   else if(image == 2)
   {
      String str(text);
      int len = bottom2-top2;
      int txtHeight = Image2->Canvas->TextHeight(str);
      int verLen = txtHeight*strlen(text);
      int centralPos = (len-verLen)/2;

      int pos = centralPos + ymargin;
      Image1->Canvas->Font->Style = TFontStyles() << fsBold;
      for(int i=0;i<strlen(text);i++)
      {
         String chr(text[i]);
         Image1->Canvas->Font->Style = TFontStyles() << fsBold;
         Image2->Canvas->TextOut(5,pos,chr);
         pos = pos+15;
      }//i
      //Revert back to default (regular)
      Image1->Canvas->Font->Style = TFontStyles();

   }

}//yAxisText

void TGraphForm::plotVerLine(float ySig,float xSig)
{

   //This routine plots a vertical line (along +yaxis) of given height at x position specified

   int Image = 1;  //Set for single graph plot

   if(logPlotMode == 1) xSig = log10(xSig);
   if(logPlotMode == 2) ySig = log10(ySig);
   if(logPlotMode == 3)
   {
      xSig = log10(xSig);
      ySig = log10(ySig);
   }

   int yminpixel = positionPixel(Image,ymin1,ymax1,0.0);
   int ymaxpixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   Image1->Canvas->MoveTo(xpixel,yminpixel);
   Image1->Canvas->LineTo(xpixel,ymaxpixel);

   //Reset pen position
   //Image1->Canvas->MoveTo(xpixel,yminpixel);


}//plotVerLine




void TGraphForm::plotSig(float ySig, float xSig, int color)
{

   if(logPlotMode == 1) xSig = log10(xSig);
   if(logPlotMode == 2) ySig = log10(ySig);
   if(logPlotMode == 3)
   {
      xSig = log10(xSig);
      ySig = log10(ySig);
   }
   int ypixel = positionPixel(1,ymin1,ymax1,ySig,0);
   int xpixel = positionPixel(1,xmin,xmax,xSig,1);


   Image1->Canvas->Pen->Color = color;

   if(plotMode == TRIANGLE_BORDER)
   {
		triangle(xpixel,ypixel,cirSize,color,TRIANGLE_BORDER);
   }
   if(plotMode == TRIANGLE_FILLED)
   {
        Image1->Canvas->Brush->Color = color;       
		triangle(xpixel,ypixel,cirSize,color,TRIANGLE_FILLED);
   }
   if(plotMode == CIRCLE_BORDER)
   {
        int shift = cirSize/2.0;
        Image1->Canvas->Brush->Color = m_cMonoBackCol;
		Image1->Canvas->Ellipse(xpixel-shift,ypixel,xpixel+cirSize-shift,
                                ypixel+cirSize);

   }
   if(plotMode == CIRCLE_FILLED)
   {
      int shift = cirSize/2.0;
      Image1->Canvas->Ellipse(xpixel-shift,ypixel,xpixel+cirSize-shift,
                              ypixel+cirSize);
      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(xpixel+1,ypixel+1,color,fsBorder);

   }
   if(plotMode == SQUARE_BORDER)
   {
      int shift = cirSize/2.0;
      Image1->Canvas->Brush->Color = m_cMonoBackCol;
      Image1->Canvas->Rectangle(xpixel-shift,ypixel,xpixel+cirSize-shift,
                                ypixel+cirSize);

   }
   if(plotMode == SQUARE_FILLED)
   {
      int shift = cirSize/2.0;
      Image1->Canvas->Rectangle(xpixel-shift,ypixel,xpixel+cirSize-shift,
                                ypixel+cirSize);
      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(xpixel+1,ypixel+1,color,fsBorder);
   }

   if(plotMode == PIXEL)
   {
      Image1->Canvas->MoveTo(xpixel,ypixel);
      Image1->Canvas->LineTo(xpixel,ypixel+1);
   }
   if(plotMode == LINE)
   {
      if(lineFlag>0)
      {
         // Check we have an odd number (for symmetry)
         if ( (line_width % 2) == 0 )
         {
				// Reduce width by 1 pixel
				line_width -= 1;
		 }
         int span = (line_width - 1) / 2.0;

         for(int i = -span; i < span+1; i++)
		 {
		 	Image1->Canvas->MoveTo(sigStore1+i,sigStore2+i);
            Image1->Canvas->LineTo(xpixel+i,ypixel+i);
		 }

      }
      sigStore1 = xpixel;
      sigStore2 = ypixel;
      lineFlag = 1;
   }
   if(ySig>ymaxVal1) ymaxVal1 = ySig;


}//plotSig  (for single screen plotting)


int TGraphForm::positionPixel(int image,float ymin,float ymax,float sig,
                              int pixelMode)
{
   int X,y;
   float R = ymax/(ymax-ymin);
   if(ymax==0) R = ymin/(ymin-ymax);
   if(pixelMode == 0)
   {                    //positioning along y axis
      if(image == 1) X = bottom1 - top1;
      else if(image == 2)
        X = bottom2 - top2;
      if(ymax==0) y = (fabs(sig)/(-ymin))*R*X;
      else
         y = (fabs(sig)/ymax)*R*X;
      if(sig>=0.0)
      {
         if(ymax != 0) y = (R*X)-y;
      }
      else
      {
         if(ymax != 0)
            y = y+(R*X);
      }
      y += ymargin;
   }//if
   if(pixelMode == 1)
   {                //positioning along x axis
      float R = ymax/(ymax-ymin);
      //if(ymax==0) R = ymin/(ymin-ymax);
      R = 1-R;
      if(image == 1) X = right1 - left1;
      else if(image == 2)
         X = right2 - left2;
      float orig = R*X;
      if(sig>=0.0)
      {
         if(ymax==0) y = (fabs(sig)/(-ymin))*(X-orig);
         else
            y = orig + (fabs(sig)/ymax)*(X-orig);
      }
      if(sig<0.0)
      {
         y = orig - (fabs(sig)/fabs(ymin))*orig;
      }
      y+= xmargin;
   }

   return y;

}//positionPixel


float TGraphForm::getXValFromPixPos(int npixels)
{

   int X;
   int XVal;

   if(image == 1)
   {
      X = right1 - left1;


      if(xmin==0)
      {
         XVal = ((npixels-xmargin)*xmax )/X;
         return XVal;
      }
      else if(xmax==0)
      {
        XVal = xmin - ((npixels-xmargin)*xmin )/X;
        return XVal;
      }
      else
      {
         float D = fabs(xmin) + fabs(xmax);
         float R = fabs(xmin)/D ;
         XVal = ((npixels-xmargin)*D)/X;
         if(XVal <= R*D)
         {
            XVal = xmin + XVal;
         }
         else
         {
            XVal = XVal - R*D;
         }
         return XVal;
      }

   }


}//getXValueFromPixPos



void TGraphForm::text(int image,float x,float y,char *caption)
{
   String str(caption);

   int ypixel = positionPixel(image,ymin1,ymax1,y);
   int xpixel = positionPixel(image,xmin,xmax,x,1);
   if(image == 1)Image1->Canvas->TextOut(xpixel,ypixel,str);
   else if(image == 2)Image2->Canvas->TextOut(xpixel,ypixel,str);

}//text





void TGraphForm::triangle(int x, int y, int size, TColor color,int type)
{

	if(type == TRIANGLE_BORDER)
	{

      Image1->Canvas->MoveTo(x,y);
      Image1->Canvas->LineTo(x-size/2.0,y+size*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(x,y);
      Image1->Canvas->LineTo(x+size/2.0,y+size*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(x-size/2.0,y+size*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(x+size/2.0,y+size*pow(5,0.5)/2.0);

	}
	if(type == TRIANGLE_FILLED)
	{
      Image1->Canvas->MoveTo(x,y);
      Image1->Canvas->LineTo(x-size/2.0,y+size*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(x,y);
      Image1->Canvas->LineTo(x+size/2.0,y+size*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(x-size/2.0,y+size*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(x+size/2.0,y+size*pow(5,0.5)/2.0);

      Image1->Canvas->FloodFill(x,y+size*pow(5,0.5)/4.0, color,fsBorder);

	}

} // triangle


void TGraphForm::pixelVerLine(int x, int pixelIncr)
{
   for(int j=top;j<=bottom;j+=pixelIncr)
   {
      Image1->Canvas->MoveTo(x,j);
      Image1->Canvas->LineTo(x,j+1);
   }//j

}//pixelVerLine

void TGraphForm::pixelHorLine(int y, int pixelIncr)
{
      for(int j=left;j<=right;j+=pixelIncr)
      {
         Image1->Canvas->MoveTo(j,y);
         Image1->Canvas->LineTo(j+1,y);
      }//j

}//pixelHorLine


void TGraphForm::graduations(TColor color)
{
   int Image = 1;   //Set for mono graph plot:

   Image1->Canvas->Pen->Color = color;

   if(showXGradLines)
   {
      //Obtain vertical graduations:
      if(logPlotMode == 1 || logPlotMode == 3)
      {
         //log graduations:
         for(int i=xmin;i<=xmax-1; i+=1)
         {
             int x = positionPixel(Image,xmin,xmax,i,1);
             pixelVerLine(x,4);
             for(int n=2;n<10;n++)
             {
               //float pos = log10( n*pow10(i) );
               float pos = log10( n*pow(10,i) );
               x = positionPixel(Image,xmin,xmax,pos,1);
               pixelVerLine(x,4);
             }//n
         }//i

      }
      else
      {  //linear graduations
         if(xgrad)
         {
            float interval = (xmax-xmin)/xnoDivs;
            float pos = xmin;
            for(int i=0;i<=xnoDivs;i++)
            {
               int x = positionPixel(Image,xmin,xmax,pos,1);
               pixelVerLine(x,4);
               pos += interval;
            }//i
         }//if

      }
   }//if(showXGradLines)

   if(showYGradLines)
   {
       //Obtain horizontal graduations:
      if(logPlotMode == 2 || logPlotMode == 3)
      {
         for(int i=ymin1;i<=ymax1-1;i+=1)
         {
            int y = positionPixel(Image,ymin1,ymax1,i,0);
            pixelHorLine(y,3);
            for(int n=2;n<10;n++)
            {
               //float pos = log10( n*pow10(i) );
               float pos = log10( n*pow(10,i) );
               y = positionPixel(Image,ymin1,ymax1,pos,0);
               pixelHorLine(y,3);
            }//n
         }//i
      }
      else
      {
         if(ygrad)
         {
            float interval = (ymax1-ymin1)/ynoDivs;
            float pos = ymin1;
            for(int i=0;i<=ynoDivs;i++)
            {
               int y = positionPixel(Image,ymin1,ymax1,pos,0);
               pixelHorLine(y,3);
               pos += interval;
            }//i
         }//if
      }
   }//if(showYGradLines

}//graduations



void TGraphForm::verLines(int xInterval)
{
   for(int i=left1;i<=right1;i+=xInterval)
   {
      Image1->Canvas->MoveTo(i,top1);
      Image1->Canvas->LineTo(i,bottom1);
   }//i
   if(monoMode == false)
   {
     for(int i=left2;i<=right2;i+=xInterval)
     {
      Image2->Canvas->MoveTo(i,top2);
      Image2->Canvas->LineTo(i,bottom2);
     }//i

   }

}//verLines


void TGraphForm::horLines(int yInterval)
{
   //Produce horizontal lines at yInterval on each side of origin:
   for(int i=C1;i>=top1;i-=yInterval)
   {
      Image1->Canvas->MoveTo(left1,i);
      Image1->Canvas->LineTo(right1,i);
   }//i

   for(int i=C1;i<=bottom1;i+=yInterval)
   {
      Image1->Canvas->MoveTo(left1,i);
      Image1->Canvas->LineTo(right1,i);
   }//i

   if(monoMode == false)
   {
      for(int i=C2;i>=top2;i-=yInterval)
      {
         Image2->Canvas->MoveTo(left2,i);
         Image2->Canvas->LineTo(right2,i);
      }//i

      for(int i=C2;i<=bottom2;i+=yInterval)
      {
         Image2->Canvas->MoveTo(left2,i);
         Image2->Canvas->LineTo(right2,i);
      }//i
   }

}//horLines


void TGraphForm::setupGrid(int xInterval,int yInterval)
{
   verLines(xInterval);
   horLines(yInterval);

}//setupGrid


void TGraphForm::xAxisScale()
{
   if(showXAxisNums)
   {
      int Image = 1;  //Set for mono graph:

      char numtext[3], powtext[3];

      sprintf(numtext,"%d",10);
      int horPos = (bottom-top)+12;

      //Obtain x axis scale:
      if(logPlotMode == 1 || logPlotMode == 3)
      {
         for(int i=xmin;i<=xmax;i+=1)
         {
            int x = positionPixel(Image,xmin,xmax,i,1);
            sprintf(powtext,"%d",i);
            x = x-30;
            Image1->Canvas->TextOut(x+15,horPos+25,numtext);
            Image1->Canvas->TextOut(x+28,horPos+20,powtext);
         }//i

      }
      else
      {
         int x,exp;
         char floatnumtext[6], field[6];
         char *s1 = "%", *s2 = "f";
         strcpy(field,s1);

         exp =  getExponent(xmax);
         //strcpy(xfield,"5.2");   //temp
         strcat(field,xfield);
         strcat(field,s2);

         float interval = (xmax-xmin)/xnoDivs;
         float pos = xmin;

         Image1->Canvas->Font->Size = 8;

         for(int i=0;i<=xnoDivs;i++)
         {
            x = positionPixel(Image,xmin,xmax,pos,1);
            float posx = pos;
            for(int k=0;k<abs(exp);k++)
            {
               if(xmax > 1)
               {
                  posx/= 10;
               }
               else
               {
                 posx*= 10.0;
               }

            }
            sprintf(floatnumtext,field,posx);
            x = x-30;
            Image1->Canvas->TextOut(x+15,horPos+20,floatnumtext);
            pos += interval;

         }//i


         //Label with correct power of 10:
        Image1->Canvas->TextOut(x+45,horPos+20,"E");
         char tmp[10];
         itoa(exp,tmp,10);
         if(exp > 0)
         {
            char tempStr[20];
            sprintf(tempStr,"%s%s","+",tmp);
            strcpy(tmp,tempStr);
         }
         Image1->Canvas->TextOut(x+52,horPos+20,tmp);
      }
   }//if(ShowXAxisNums)

}//xAxisScale

int TGraphForm::getExponent(float num)
{
   int exp;
   exp = 0;
   bool check = true;
   if(num == 0.0)
       return 0;

   if(num < 0.0 )
   {
      //Get magnitude value for exponent calcs:
      num = -1.0*num;
   }

   if(num < 1.0)
   {
      while(check)
      {
        num = num*10.0;
        exp -= 1;
        if(num > 1.0)
        {
            check = false;
        }
      }//while
   }//if
   else
   {
      while(check)
      {
        num = num/10.0;
        if(num < 1.0)
        {
            check = false;
        }
        else
        {
            exp += 1;
        }
      }//while

   }

   return exp;

}//getExponent

void TGraphForm::yAxisScale()
{

   if(showYAxisNums)
   {
      int Image = 1;  //set for mono graph:

      char numtext[3], powtext[3], error[20];

      sprintf(numtext,"%d",10);
      int verPos = left-85;

      //Obtain y axis scale:
      if(logPlotMode == 2 || logPlotMode == 3)
      {
         for(int i = ymin1;i<=ymax1;i+=1)
         {
            int y = positionPixel(Image,ymin1,ymax1,i,0);
            sprintf(powtext,"%d",i);
            Image1->Canvas->TextOut(verPos+60,y-5,numtext);
            Image1->Canvas->TextOut(verPos+75,y-10,powtext);
         }//i
      }
      else
      {
         int y,exp;
         char floatnumtext[6], field[6];
         char *s1 = "%", *s2 = "f";
         strcpy(field,s1);

         exp =  getExponent(ymax1);
         //strcpy(yfield,"5.2");   //temp
         strcat(field,yfield);
         strcat(field,s2);

         float interval = (ymax1-ymin1)/ynoDivs;
         float pos = ymin1;

         Image1->Canvas->Font->Size = 8;

         for(int i=0;i<=ynoDivs;i++)
         {
            y = positionPixel(Image,ymin1,ymax1,pos,0);
            float posy = pos;
            for(int k=0;k<abs(exp);k++)
            {
               if(ymax1 > 1)
               {
                  posy/= 10;
               }
               else
               {
                 posy*= 10.0;
               }

            }
            sprintf(floatnumtext,field,posy);
            Image1->Canvas->TextOut(verPos+60,y-6,floatnumtext);
            pos += interval;

         }//i

         //Label with correct power of 10:
         Image1->Canvas->TextOut(verPos+63,y-21,"E");
         char tmp[10];
         itoa(exp,tmp,10);
         if(exp > 0)
         {
            char tempStr[20];
            sprintf(tempStr,"%s%s","+",tmp);
            strcpy(tmp,tempStr);
         }
         Image1->Canvas->TextOut(verPos+70,y-21,tmp);

         //Reset to default value:
         //Image1->Canvas->Font->Size = 11;

      }
   }//if(showYAxisNums)

}//yAxisScale


void TGraphForm::axisDefaults(int xdivs,int ydivs, char *xspec,
                              char *yspec)
{
   xnoDivs = xdivs;   //Redefine no. scale divs. along x-axis
   ynoDivs = ydivs;   //Redefine no. scale divs. along y-axis
   strcpy(xfield,xspec);  //Redefine field specification: x scaling
   strcpy(yfield,yspec);  //Redefine field specification: y scaling

}//axisDefaults


void TGraphForm::drawVerticalMarker(float pos, int color,int width)
{

	//int width: 0: standard 1 pixel; 1: 3 pixel width

	// Draws a vertical marker line at the requested postion
	if(logPlotMode == 0 || logPlotMode == 2)
  	{
      if ((pos >= xmin) && (pos <= xmax))
      {
			C1 = ( (pos-xmin) / (xmax-xmin) )* (right-left);
			C1 = C1 + xmargin;  //Add margin offset

			Image1->Canvas->Pen->Color = color;

			// Check we have an odd number (for symmetry)
         if ( (width % 2) == 0 )
         {
				// Reduce width by 1 pixel
				width -= 1;
		 }
         int span = (width - 1) / 2.0;

         for(int i = -span; i < span+1; i++)
		 {
		 	Image1->Canvas->MoveTo(C1+i,top);
            Image1->Canvas->LineTo(C1+i,bottom);

		 }

	  }

   }


}// drawVerticalMarker

// To be used with animated plots (in association with legendCaptions(.)
// NB: This Windows version to be checked.
void TGraphForm::legend(struct leg_data l_data, bool refresh, int numBoxes,int color, int leg_left, int leg_top,int leg_width,int bCol)

{

   char cap1[30], cap2[30], cap3[30], cap4[30];

   int cap_width = 65;
   int left = leg_left + cap_width;
   int top = leg_top;

   int data_rect_col = bCol;
   Image1->Canvas->Pen->Color = data_rect_col;

   if(refresh)  //Repaint to background previous value
   {
       Image1->Canvas->Font->Color = m_cMonoBackCol;

   }
   else
   {
       Image1->Canvas->Font->Color = color;
   }

   int shift = 0;
   int data_width = leg_width - cap_width; //150;
   int top_high = top + 28;
   int top_low = top - 5;
   int left_start = left;

   float data;
   for(int i = 0; i < numBoxes;i++)
   {

		if (i == 0)
		{

           // Repaint rectangle area before writng data
           Image1->Canvas->Rectangle(shift+left_start,top_low,shift+left_start+data_width,top_high);
           Image1->Canvas->Brush->Color = clBlack;
           Image1->Canvas->FloodFill(shift+left_start+1,top_low+1,data_rect_col,fsBorder);


   	       if (_isnan(l_data.data1))
   	       {
		      sprintf(cap1,"%s  %s","XXXX",l_data.data1_unit);
	       }
   	       else
   	       {

				if ( (l_data.data1 == 0.0))
				{
					data = l_data.data1;
					sprintf(cap1,"%5d  %s",(int) data,l_data.data1_unit);
				}
				else if ( (l_data.data1 < 1.0) && (l_data.data1 > 1.0e-3))
				{

					data = l_data.data1 * 1000;
					sprintf(cap1,"%5.2f  %c%s", data,'m',l_data.data1_unit);
				}
				else if ( (l_data.data1 >= 1.0) && (l_data.data1 < 1.0e3))
				{
					data = l_data.data1;
					sprintf(cap1,"%5.2f  %s",data,l_data.data1_unit);
				}
				else if ( (l_data.data1 >= 1.0e3) && (l_data.data1 < 1.0e6))
				{
					data = l_data.data1 / 1000;
					sprintf(cap1,"%5.2f  %c%s",data,'k',l_data.data1_unit);
				}
				else if ( l_data.data1 >= 1.0e6)
				{
					data = l_data.data1 / 1000000;
					sprintf(cap1,"%5.2f  %c%s",data,'M',l_data.data1_unit);
				}
			}

            Image1->Canvas->Font->Color = color;
            Image1->Canvas->TextOut(left+shift+10,top,cap1);
		}
		else if (i == 1)
		{

		   // Repaint rectangle area before writng data
           Image1->Canvas->Rectangle(shift+left_start,top_low,shift+left_start+data_width,top_high);
           Image1->Canvas->Brush->Color = clBlack;
           Image1->Canvas->FloodFill(shift+left_start+1,top_low+1,data_rect_col,fsBorder);


   	       if (_isnan(l_data.data2))
   	       {
				sprintf(cap2,"%s  %s","XXXX",l_data.data2_unit);
		   }
		   else
		   {
				if ( (l_data.data2 == 0.0))
				{
					data = l_data.data2;
					sprintf(cap2,"%5d  %s",(int) data,l_data.data2_unit);
				}
				else if ( (l_data.data2 < 1.0) && (l_data.data2 > 1.0e-3))
				{

					data = l_data.data2 * 1000;
					sprintf(cap2,"%5.2f  %c%s", data,'m',l_data.data2_unit);
				}
				else if ( (l_data.data2 >= 1.0) && (l_data.data2 < 1.0e3))
				{
					data = l_data.data2;
					sprintf(cap2,"%5.2f  %s",data,l_data.data2_unit);
				}
				else if ( (l_data.data2 >= 1.0e3) && (l_data.data2 < 1.0e6))
				{
					data = l_data.data2 / 1000;
					sprintf(cap2,"%5.2f  %c%s",data,'k',l_data.data2_unit);
				}
				else if ( l_data.data2 >= 1.0e6)
				{
					data = l_data.data2 / 1000000;
					sprintf(cap2,"%5.2f  %c%s",data,'M',l_data.data2_unit);
				}
		   }
		   //Output data2
		   Image1->Canvas->Font->Color = color;
           Image1->Canvas->TextOut(left+shift+10,top,cap2);


		}
		else if (i == 2)
		{
            // Repaint rectangle area before writng data
           Image1->Canvas->Rectangle(shift+left_start,top_low,shift+left_start+data_width,top_high);
           Image1->Canvas->Brush->Color = clBlack;
           Image1->Canvas->FloodFill(shift+left_start+1,top_low+1,data_rect_col,fsBorder);

			if (_isnan(l_data.data3))
   	        {
				sprintf(cap3,"%s  %s","XXXX",l_data.data3_unit);
			}
			else
			{

				if ( (l_data.data3 == 0.0))
				{
					data = l_data.data3;
					sprintf(cap3,"%5d  %s",(int) data,l_data.data3_unit);
				}
				else if ( (l_data.data3 < 1.0) && (l_data.data3 > 1.0e-3))
				{

					data = l_data.data3 * 1000;
					sprintf(cap3,"%5.2f  %c%s", data,'m',l_data.data3_unit);
				}
				else if ( (l_data.data3 >= 1.0) && (l_data.data3 < 1.0e3))
				{
					data = l_data.data3;
					sprintf(cap3,"%5.2f  %s",data,l_data.data3_unit);
				}
				else if ( (l_data.data3 >= 1.0e3) && (l_data.data3 < 1.0e6))
				{
					data = l_data.data3 / 1000;
					sprintf(cap3,"%5.2f  %c%s",data,'k',l_data.data3_unit);
				}
				else if ( l_data.data3 >= 1.0e6)
				{
					data = l_data.data3 / 1000000;
					sprintf(cap3,"%5.2f  %c%s",data,'M',l_data.data3_unit);
				}
		   }

			//Ouput data3:
			Image1->Canvas->Font->Color = color;
            Image1->Canvas->TextOut(left+shift+10,top,cap3);



		}
		else if (i == 3)
		{
           // Repaint rectangle area before writng data
           Image1->Canvas->Rectangle(shift+left_start,top_low,shift+left_start+data_width,top_high);
           Image1->Canvas->Brush->Color = clBlack;
           Image1->Canvas->FloodFill(shift+left_start+1,top_low+1,data_rect_col,fsBorder);

   	       if (_isnan(l_data.data4))
   	       {
				sprintf(cap4,"%s  %s","XXXX",l_data.data4_unit);
		   }
		   else
		   {

				if ( (l_data.data4 == 0.0))
				{
					data = l_data.data4;
					sprintf(cap4,"%5d  %s",(int) data,l_data.data4_unit);
				}
				else if ( (l_data.data4 < 1.0) && (l_data.data4 > 1.0e-3))
				{
					data = l_data.data4 * 1000;
					sprintf(cap4,"%5.2f  %c%s", data,'m',l_data.data4_unit);
				}
				else if ( (l_data.data4 >= 1.0) && (l_data.data4 < 1.0e3))
				{
					data = l_data.data4;
					sprintf(cap4,"%5.2f  %s",data,l_data.data4_unit);
				}
				else if ( (l_data.data4 >= 1.0e3) && (l_data.data4 < 1.0e6))
				{
					data = l_data.data4 / 1000;
					sprintf(cap4,"%5.2f  %c%s",data,'k',l_data.data4_unit);
				}
				else if ( l_data.data4 >= 1.0e6)
				{
					data = l_data.data4 / 1000000;
					sprintf(cap4,"%5.2f  %c%s",data,'M',l_data.data4_unit);
				}
		   }

		   //Output data4:
		   Image1->Canvas->Font->Color = color;
           Image1->Canvas->TextOut(left+shift+10,top,cap4);

		}

        shift += leg_width;
   }




}//legend


// This method can be used for animated plots (e.g. an oscilloscope trace)
// where information can be updated in real time.
// Each box contains a single item with caption and data.
// This legend provides the legend captions.
// Provides up to four legend boxes side by side.
// To be used with legend(.) function which provides the data.
// NB: This Windows version to be checked.
void TGraphForm::legendCaptions(struct leg_captions l_caps,int col, int numBoxes, int leg_left, int leg_top,int leg_width,int bCol)

{

   int left = leg_left;
   int top = leg_top;
   int box_width = leg_width;
   int border_color = bCol;

   for(int i = 0; i < numBoxes; i++)
   {

        // Write main box:
		Image1->Canvas->Pen->Color = border_color;
		Image1->Canvas->Rectangle(left,top-5,left+box_width,top+28);
   	    Image1->Canvas->Brush->Color = clBlack;
        Image1->Canvas->FloodFill(left+1,top-5+1,border_color,fsBorder);


		//Write captions:
        Image1->Canvas->Brush->Color = clBlack;
        Image1->Canvas->Font->Color = col;
        Image1->Canvas->Font->Style = TFontStyles() << fsBold;


		if ( i == 0)
		{
			Image1->Canvas->TextOut(left+5,top,l_caps.data1_caption);
		}
		else if (i == 1)
		{
			Image1->Canvas->TextOut(left+5,top,l_caps.data2_caption);
		}
		else if ( i == 2)
		{
			Image1->Canvas->TextOut(left+5,top,l_caps.data3_caption);
		}
		else if (i == 3)
		{
			Image1->Canvas->TextOut(left+5,top,l_caps.data4_caption);
		}

		left += box_width;
   }


}//legendCaptions


void TGraphForm::legendCaptions1(TColor col,TColor bck_color,int numPlots,struct legend_info info[], int left, int top,
                                 int width, int height)
{

   //Set up captions with associated plot symbols:
   //Last legend to a theoretical (line) -if required
   //MAX of four  caption lines!!


   //Write legend border:
   TColor color = bck_color;

   //Get Legend dimensions:


   Image1->Canvas->Brush->Color = color;

   //Put border around the legend with another color:
   Image1->Canvas->Pen->Color = col;
   Image1->Canvas->Rectangle(left-5,top-5,left+width,top+height);


   int ypos = 5;
   Image1->Canvas->Font->Size = 12;
   for(int i=0;i<numPlots;i++)
   {
		//First Legend:
		Image1->Canvas->TextOut(left,top+ypos,info[i].text);

        Image1->Canvas->Pen->Color = info[i].color;
        if(info[i].type == CIRCLE_BORDER)
		{
			circleSize(9);
            int shift = cirSize/2.0;

			Image1->Canvas->Ellipse(left+(width-20)-shift,top+ypos+5,
                                    left+(width-20)+cirSize-shift,top+ypos+5+cirSize);

		}
		if(info[i].type == CIRCLE_FILLED)
		{
			circleSize(9);
            int shift = cirSize/2.0;
			Image1->Canvas->Ellipse(left+(width-20)-shift,top+ypos+5,
                                    left+(width-20)+cirSize-shift,top+ypos+5+cirSize);
            Image1->Canvas->Brush->Color = info[i].color;
			Image1->Canvas->FloodFill(left+(width-20)+1,top+ypos+5+1,info[i].color,fsBorder);
            //Revert back to background color of legend
            Image1->Canvas->Brush->Color =  color;
		}

		if(info[i].type == SQUARE_BORDER)
		{
			circleSize(9);
            Image1->Canvas->Rectangle(left+(width-20),top+ypos+5,left+(width-20)+cirSize,top+ypos+5+cirSize);

		}
		if(info[i].type == SQUARE_FILLED)
		{
			circleSize(9);
            Image1->Canvas->Brush->Color = info[i].color;
			Image1->Canvas->Rectangle(left+(width-20),top+ypos,left+(width-20)+cirSize,top+ypos+cirSize);
            Image1->Canvas->FloodFill(left+(width-20)+1,top+ypos+1,info[i].color,fsBorder);
            //Revert back to background color of legend
            Image1->Canvas->Brush->Color =  color;

		}
		if(info[i].type == TRIANGLE_BORDER)
		{
			circleSize(9);
            triangle(left+(width-20),top+ypos+5,cirSize,info[i].color,TRIANGLE_BORDER);

		}
		if(info[i].type == TRIANGLE_FILLED)
		{
			circleSize(9);
            Image1->Canvas->Brush->Color = info[i].color;
			triangle(left+(width-20),top+ypos+5,cirSize,info[i].color,TRIANGLE_FILLED);
            //Revert back to background color of legend
            Image1->Canvas->Brush->Color =  color;
		}


		if(info[i].type == LINE)
		{
             TColor font_col = Image1->Canvas->Font->Color;
             Image1->Canvas->Font->Color = info[i].color;
             Image1->Canvas->TextOut(left+(width-20)-10,top+ypos,"----");
             //Revert back to previous font color:
             Image1->Canvas->Font->Color = font_col;
		}

		ypos += 20;

   }//i


}//legendCaptions1




void TGraphForm::setLinePlotFlag(int flg)
{
  //Only use with single trace
  lineFlag = flg;
}


void TGraphForm::lineWidth(int width)
{
	line_width = width;
}

// COMMON LINUX METHODS END HERE ------------------------------------------

//---------------------------------------------------------------------------

void TGraphForm::plotSig(float ySig, float xSig)
{

   int Image = 1;  //Set for single graph plot

   if(logPlotMode == 1) xSig = log10(xSig);
   if(logPlotMode == 2) ySig = log10(ySig);
   if(logPlotMode == 3)
   {
      xSig = log10(xSig);
      ySig = log10(ySig);
   }
   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);
   if(plotMode == 2)
   {
      int shift = cirSize/2.0;
      //Image1->Canvas->Ellipse(xpixel,ypixel,xpixel+cirSize,
      //                        ypixel+cirSize);
      Image1->Canvas->Ellipse(xpixel-shift,ypixel,xpixel+cirSize-shift,
                              ypixel+cirSize);

   }
   if(plotMode == 3)
   {
      int shift = cirSize/2.0;
      //Image1->Canvas->Rectangle(xpixel,ypixel,xpixel+cirSize,
      //                        ypixel+cirSize);
      Image1->Canvas->Rectangle(xpixel-shift,ypixel,xpixel+cirSize-shift,
                                ypixel+cirSize);
   }
   if(plotMode == 4)
   {
       //Plot a bar graph
       drawBar(ymin1,ySig,xSig,m_iBarWidth,m_cDataSet[0]);


   }
   if(plotMode == 1)
   {
      int pixelSize = 1;
      Image1->Canvas->MoveTo(xpixel,ypixel);
      Image1->Canvas->LineTo(xpixel,ypixel+1);
   }
   if(plotMode == 0)
   {
      if(lineFlag>0)
      {
         Image1->Canvas->MoveTo(sigStore1,sigStore2);
         Image1->Canvas->LineTo(xpixel,ypixel);
      }
      sigStore1 = xpixel;
      sigStore2 = ypixel;
      lineFlag = 1;
   }
   if(ySig>ymaxVal1) ymaxVal1 = ySig;


}//plotSig  (for single screen plotting)




void TGraphForm::legend(int l,int t,int nsets,char *t1,char *t2, char *t3,
                        TColor c1,TColor c2, TColor c3, TColor bck,
                        int pm1,int pm2,int pm3)
{
   char mes1[100], mes2[100],mes3[100];

   sprintf(mes1,t1);
   sprintf(mes2,t2);
   sprintf(mes3,t3);
   String Mes1(mes1);
   String Mes2(mes2);
   String Mes3(mes3);

   //int left = 50;
   //int top = 50;
   int left = l;
   int top = t;

   int shift = 90;   //Define shift to right for putting data plot symbol

   if(monoMode == false)
   {
      left = 30;
      top = 30;
   }

   //Set font size for labels:
   //Image1->Canvas->Font->Color = clNavy;
   Image1->Canvas->Font->Height = 10;
   //Write legend border:
   Image1->Canvas->Brush->Color = bck;
   Image1->Canvas->Rectangle(left-5,top-5,left+100,top+40);

   //Write captions:
   //Image1->Canvas->TextOut(left,top,Mes1);
   //Image1->Canvas->TextOutA(left,top+20,Mes2);

   if(nsets == 1)
   {
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

   }
   if(nsets == 2)
   {
     //First data set:
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

      //Second data set:
     Image1->Canvas->Font->Color = c2;
     Image1->Canvas->TextOut(left,top+12,Mes2);
     Image1->Canvas->Pen->Color = c2;
     setDataSymbol(left,top+12,pm2,shift);
   }
   if(nsets == 3)
   {
     //First data set:
     Image1->Canvas->Font->Color = c1;
     Image1->Canvas->TextOut(left,top,Mes1);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c1;
     setDataSymbol(left,top,pm1,shift);

     //Second data set:
     Image1->Canvas->Font->Color = c2;
     Image1->Canvas->TextOut(left,top+12,Mes2);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c2;
     setDataSymbol(left,top+12,pm2,shift);

     //Third data set:
     Image1->Canvas->Font->Color = c3;
     Image1->Canvas->TextOut(left,top+24,Mes3);
     //Output the plot symbol:
     Image1->Canvas->Pen->Color = c3;
     setDataSymbol(left,top+24,pm3,shift);
   }

   //Legend for second trace (if dual trace):
   if(monoMode == false)
   {
      sprintf(mes1,"yaxisMax:  %5.1f",ymax2);
      sprintf(mes2,"ymaxValue: %5.1f",ymaxVal2);
      String Mes1(mes1);
      String Mes2(mes2);

      //Set font size for labels:
      Image2->Canvas->Font->Color = clNavy;
      Image2->Canvas->Font->Height = 10;
      //Write legend border:
      Image2->Canvas->Rectangle(left-5,top-5,left+100,top+40);
      //Write captions:
      Image2->Canvas->TextOut(left,top,Mes1);
      Image2->Canvas->TextOutA(left,top+20,Mes2);

   }

}//legend



void TGraphForm::setDataSetText(char *t1, char*t2, char *t3)
{
   strcpy(m_sLegendText1,t1);
   strcpy(m_sLegendText2,t2);
   strcpy(m_sLegendText3,t3);

}//setDataSetText

void TGraphForm::setSkipSetFlags(bool f1,bool f2,bool f3)
{
   m_bSkipSet[0] = f1;
   m_bSkipSet[1] = f2;
   m_bSkipSet[2] = f3;

}//setSkipSetFlags


void TGraphForm::setDataPlotModes(int index1,int index2,int index3)
{
   m_iPlotMode[0] = index1;
   m_iPlotMode[1] = index2;
   m_iPlotMode[2] = index3;

}//setDataPlotmodes



void TGraphForm::setDataSetColors(TColor c1,TColor c2,TColor c3)
{
   m_cDataSet[0] = c1;
   m_cDataSet[1] = c2;
   m_cDataSet[2] = c3;

}//setDataSetColors


void TGraphForm::setLegendPosition(int l,int t)
{
   m_iLegendLeftPos = l;
   m_iLegendTopPos = t;

}//setLegendPosition


void TGraphForm::setupImageSize()
{

   if(BorderStyle != bsNone)
   {
      //Adjust image margins accordingly:
      int imageLeftMargin = 10;
      int imageTopMargin = 15;
      int hMenuBar = 5;//30;    //Estimate on menu bar height


      Image1->Width = Width - 3*imageLeftMargin;
      Image1->Left =  (float)( ( (float)Width - (float)Image1->Width)/2.0);
      Image1->Top =  10 + hMenuBar;
      Image1->Height = Height - Image1->Top - 75;
   }
   else
   {
      //Form embedded in another visual app - with no border

      //Adjust image margins accordingly:
      int imageLeftMargin = 2;
      int imageTopMargin = 1;
      int hMenuBar = 5;//30;    //Estimate on menu bar height


      Image1->Width = Width - 2*imageLeftMargin;
      Image1->Left = imageLeftMargin;
      Image1->Top =  imageTopMargin;
      Image1->Height = Height - 25*imageTopMargin;

   }


}//setupImageSize


void TGraphForm::drawBar(float y1,float y2,float x, int bwidth, TColor color)
{
      int X,X1,Y1,X2,Y2;

      int width = (bottom-top) + 2*ymargin;

      Y1 = ( (y1-ymin1)/(ymax1-ymin1) )* (bottom-top);
      Y2 = ( (y2-ymin1)/(ymax1-ymin1) )* (bottom-top);
      //Add margins
      Y1 = Y1 + ymargin;
      Y2 = Y2 + ymargin;
      Y1 = width-Y1;
      Y2 = width-Y2;

      //Get x coordinates for rectangle
      //X1 = ( ((x-bwidth/2.0) - xmin)/(xmax-xmin) )* (right-left);
      //X1 = X1 + xmargin;  //Add margin offset
      //X2 = ( ((x+bwidth/2.0) - xmin)/(xmax-xmin) )* (right-left);
      //X2 = X2 + xmargin;  //Add margin offset
      X = ( (x-xmin)/(xmax-xmin) )* (right-left);
      X1 = X - bwidth/2.0;
      X1 = X1 + xmargin;
      X2 = X + bwidth/2.0;
      X2 = X2 + xmargin;

      TRect rect(X1,Y2,X2,Y1);
      //Draw bar:
      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);

      //Reset brush to backgorund color:
      Image1->Canvas->Brush->Color = m_cMonoBackCol;


}//drawBar


void TGraphForm::drawBitmap(int image,float x,float y, std::string bitmapfile)
{

   int ypixel = positionPixel(image,ymin1,ymax1,y);
   int xpixel = positionPixel(image,xmin,xmax,x,1);

   //Convert to UniCode String for Canvas->Draw method:
   int len = bitmapfile.length();
   String bmpfile(bitmapfile.c_str(),len);
   Graphics::TBitmap *bmp = new Graphics::TBitmap;
	try
	{
		bmp->LoadFromFile(bmpfile);
		getImage()->Canvas->Draw(xpixel, ypixel, bmp);
	}
	__finally
	{
		delete bmp;
	}



}//drawBitmap




void TGraphForm::circle(int image,float x1,float y1,float x2,float y2)
{

   int x1pixel = positionPixel(image,xmin,xmax,x1,1);
   int y1pixel = positionPixel(image,ymin1,ymax1,y1,0);
   int x2pixel = positionPixel(image,xmin,xmax,x2,1);
   int y2pixel = positionPixel(image,ymin1,ymax1,y2,0);

	if(image == 1)Image1->Canvas->Ellipse(x1pixel,y1pixel,x2pixel,y2pixel);
	else if(image == 2)Image2->Canvas->Ellipse(x1pixel,y1pixel,x2pixel,y2pixel);


}//circle



void TGraphForm::drawArrow(float x, float y, int h, int w, TColor color, char *type)
{

   int X,Y;

   int width = (bottom-top) + 2*ymargin;

   Y = ( (y-ymin1)/(ymax1-ymin1) )* (bottom-top);
   //Add margins
   Y = Y  + ymargin;
   Y = width-Y;

   X = ( (x-xmin)/(xmax-xmin) )* (right-left);
   X = X + xmargin;

   //Define relative proportions of arrow head and arrow body:
   int arrowhead_h = h;
   int arrowhead_w = h*pow(5,0.5)/2.0;
   int arrowbody_w = 2* arrowhead_w;
   int arrowbody_h = 0.6*arrowhead_h;

   if( strcmp(type,"rARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y-h/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->MoveTo(X-h*pow(5,0.5)/2.0,Y-h/2.0);
      Image1->Canvas->LineTo(X-h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X - h*pow(5,0.5)/4.0,Y, color,fsBorder);

      //Draw the arrow body:
       TRect rect( X - arrowhead_w - arrowbody_w, Y-arrowbody_h/2.0,
                  X - arrowhead_w, Y+arrowbody_h/2.0 );

      //Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);


   }//rARROW
   else if( strcmp(type,"lARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y-h/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->MoveTo(X+h*pow(5,0.5)/2.0,Y-h/2.0);
      Image1->Canvas->LineTo(X+h*pow(5,0.5)/2.0,Y+h/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X + h*pow(5,0.5)/4.0,Y, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X + arrowhead_w + arrowbody_w, Y-arrowbody_h/2.0,
                  X + arrowhead_w, Y+arrowbody_h/2.0 );

      //Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FillRect(rect);


   }//lARROW
   else if( strcmp(type,"uARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X-h/2.0,Y+h*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(X+h/2.0,Y+h*pow(5,0.5)/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X,Y+h*pow(5,0.5)/4.0, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X - arrowbody_h/2.0, Y + arrowhead_w,
                  X + arrowbody_h/2.0, Y + arrowhead_w + arrowbody_w );


      Image1->Canvas->FillRect(rect);


   }//uARROW
   else if( strcmp(type,"dARROW") == 0 )
   {
      //Draw arrow pointing to right:
      //Create arrow head:
      Image1->Canvas->Pen->Color = color;
      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X-h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X,Y);
      Image1->Canvas->LineTo(X+h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->MoveTo(X-h/2.0,Y-h*pow(5,0.5)/2.0);
      Image1->Canvas->LineTo(X+h/2.0,Y-h*pow(5,0.5)/2.0);

      Image1->Canvas->Brush->Color = color;
      Image1->Canvas->FloodFill(X,Y-h*pow(5,0.5)/4.0, color,fsBorder);

      //Draw the arrow body:
      TRect rect( X - arrowbody_h/2.0, Y - arrowhead_w - arrowbody_w,
                  X + arrowbody_h/2.0, Y - arrowhead_w );

      Image1->Canvas->FillRect(rect);


   }//dARROW



}//drawArrow



void TGraphForm::plotAudioMarkerPos(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if((Image1->Canvas->Pixels[xpixel][ypixel]==m_cMonoBackCol)
             || (Image1->Canvas->Pixels[xpixel][ypixel]==m_cAudioClipMarkerColor) )
         {
            //Only write pixel if not in wave signal:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
         (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {
         //Erasing the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cAudioMarkerColor )
         {
            //Only erase pixel if audio marker color:
            Image1->Canvas->LineTo(xpixel,ypixel+1);

         }
      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotAudioMarkerPos (plot current marker for audio play position)


void TGraphForm::plotAudioMarkerPos1(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(!m_bSigRangeDefined)
         {
            //check for white signal:
            int tpixel = positionPixel(Image,ymin1,ymax1,ymin1);
            if( !(ypixel==tpixel)) //Only do after first pixel -for white signal may get confused with canvas
            {
               if(!m_bSigTrace) //Not got to lower bound of signal yet:
               {
                  if(Image1->Canvas->Pixels[xpixel][ypixel]==getDataPlotColor())
                  //if(Image1->Canvas->Pixels[xpixel][ypixel]==clNavy)
                  {
                     m_iSigLower = ypixel;
                     m_bSigTrace = true;
                  }

               }
               else    //Got to top of signal - now find upper y coordinate
               {
                  if(Image1->Canvas->Pixels[xpixel][ypixel]==m_cMonoBackCol)
                  {
                     m_iSigUpper = ypixel;
                     m_bSigRangeDefined = true;
                  }

               }
            }
         }
         //Write pixel line:
         Image1->Canvas->LineTo(xpixel,ypixel+1);

      }
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
         (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {

         //Erasing the audio position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);

         if( (ypixel < m_iSigLower) && (ypixel > m_iSigUpper) )
         {
            //Repaint signal color:
            Image1->Canvas->Pen->Color = getDataPlotColor();
            Image1->Canvas->LineTo(xpixel,ypixel+1);
            //Reset to background color
            if(!m_bSelected)
               Image1->Canvas->Pen->Color = m_cMonoBackCol;
             else
               Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;


         }
         else
         {
            //Repaint background color
            if(!m_bSelected)
               Image1->Canvas->Pen->Color = m_cMonoBackCol;
             else
               Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }

      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotAudioMarkerPos1 (plot current marker for audio play position)




void TGraphForm::plotClipMarkerPos(float ySig, float xSig)
{
   int Image = 1;  //Set for single graph plot

   int ypixel = positionPixel(Image,ymin1,ymax1,ySig);
   int xpixel = positionPixel(Image,xmin,xmax,xSig,1);

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio clip position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cMonoBackCol )
         {
            //Only write pixel if not in wave signal:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
         //Erasing the audio clip position marker:
         int pixelSize = 1;
         Image1->Canvas->MoveTo(xpixel,ypixel);
         if(Image1->Canvas->Pixels[xpixel][ypixel] == m_cAudioClipMarkerColor )
         {
            //Only erase pixel if audio clip marker color:
            Image1->Canvas->LineTo(xpixel,ypixel+1);
         }
      }
   }

   if(ySig>ymaxVal1) ymaxVal1 = ySig;

}//plotClipMarkerPos (plot current marker for clip position)





bool TGraphForm::plotSig1(float signal)
{
   int Image = 1;  //Set for top trace:

   if(counter1 >= left1+WINDOW_PIXEL_LENGTH) return false;
   int ypixel = positionPixel(Image,ymin1,ymax1,signal);
   if(ypixel >= bottom1) return false;
   if(plotMode == 2)
   {
     Image1->Canvas->Ellipse(counter1,ypixel,counter1+cirSize,
                             ypixel+cirSize);
   }
   else if(plotMode == 1)
   {
      int pixelSize = 1;
      Image1->Canvas->MoveTo(counter1,ypixel);
      Image1->Canvas->LineTo(counter1+pixelSize,ypixel+pixelSize);
   }
   else if(plotMode == 0)
   {
     if(counter1>left1)
      {
         Image1->Canvas->MoveTo(counter1-1,sigStore1);
         Image1->Canvas->LineTo(counter1,ypixel);
      }
      sigStore1 =ypixel;


  }
  counter1 = counter1+xincrement;
  if(signal >ymaxVal1) ymaxVal1 = signal;

  return true;

}//plotSig1 (trace 1 of dual CRO)


void TGraphForm::animatedPlot1(float signal,bool stop)
{

   buffer1[WINDOW_PIXEL_LENGTH-1] = signal;
   dispBufferElems(signal,1);

   buffCount1++;
   if(buffCount1<WINDOW_PIXEL_LENGTH) return;
   Image1->Canvas->Pen->Color = clNavy;
   for(int j=0;j<WINDOW_PIXEL_LENGTH;j++)
   {
      plotSig1(buffer1[j]);
   }//j
   initialisePizPos();

   //Update view:
   Sleep(0);
   Update();

   if(!stop)
   {
      //Clear graph view:
      TRect newRect = Rect(left1+1,top1+1,right1-1,bottom1-1);
      Image1->Canvas->Brush->Color = clWhite;
      Image1->Canvas->FillRect(newRect);
      Image1->Canvas->Pen->Color = clBlack;
      Image1->Canvas->MoveTo(left1,C1);
      Image1->Canvas->LineTo(right1,C1);
      if(showGrid) grid(gridInt,gridInt);
   }

}//animatedPlot1


bool TGraphForm::plotSig2(float signal)
{
   int Image = 2;  //Set for top trace:

   if(counter2 >= left2+WINDOW_PIXEL_LENGTH) return false;
   int ypixel = positionPixel(Image,ymin2,ymax2,signal);
   if(ypixel >= bottom2) return false;
   if(plotMode == 2)
   {
     Image2->Canvas->Ellipse(counter2,ypixel,counter2+cirSize,
                             ypixel+cirSize);
   }
   else if(plotMode == 1)
   {
      int pixelSize = 1;
      Image2->Canvas->MoveTo(counter2,ypixel);
      Image2->Canvas->LineTo(counter2+pixelSize,ypixel+pixelSize);
   }
   else if(plotMode == 0)
   {
     if(counter2>left2)
      {
         Image2->Canvas->MoveTo(counter2-1,sigStore2);
         Image2->Canvas->LineTo(counter2,ypixel);
      }
      sigStore2 =ypixel;


  }
  counter2 = counter2+xincrement;
  if(signal >ymaxVal2) ymaxVal2 = signal;
  return true;

}//plotSig2   (trace 2 of CRO)


void TGraphForm::animatedPlot2(float signal,bool stop)
{

   buffer2[WINDOW_PIXEL_LENGTH-1] = signal;
   dispBufferElems(signal,2);

   buffCount2++;
   if(buffCount2<WINDOW_PIXEL_LENGTH) return;
   Image2->Canvas->Pen->Color = clNavy;
   for(int j=0;j<WINDOW_PIXEL_LENGTH;j++)
   {
      plotSig2(buffer2[j]);
   }//j
   initialisePizPos();

   //Update view:
   Sleep(0);
   Update();

   if(!stop)
   {
      //Clear graph view:
      TRect newRect = Rect(left2+1,top2+1,right2-1,bottom2-1);
      Image2->Canvas->Brush->Color = clWhite;
      Image2->Canvas->FillRect(newRect);
      Image2->Canvas->Pen->Color = clBlack;
      Image2->Canvas->MoveTo(left2,C1);
      Image2->Canvas->LineTo(right2,C1);
      if(showGrid) grid(gridInt,gridInt);
   }


}//animatedPlot2


void TGraphForm::dispBufferElems(float signal,int buffMode)
{
   for(int i=0;i<WINDOW_PIXEL_LENGTH-1;i++)
   {
     if(buffMode == 1) buffer1[i] = buffer1[i+1];
     else
      buffer2[i] = buffer2[i+1];
   }
   if(buffMode == 1) buffer1[WINDOW_PIXEL_LENGTH-1] = signal;
   else
      buffer2[WINDOW_PIXEL_LENGTH-1] = signal;


}//displayBufferElems




void TGraphForm::setDataSymbol(int left, int top, int pmode,int shift)
{

        if(pmode == 2)  //Circle symbol data plot
        {
            Image1->Canvas->Ellipse(left+shift,top+4,left+shift+cirSize,
                                 top+4+cirSize);
        }
        if(pmode == 3)  //Rectangle symbol data plot
        {
           Image1->Canvas->Rectangle(left+shift,top+4,left+shift+cirSize,
                                   top+4+cirSize);
        }
        if(pmode == 0)  //Line plot
        {
             Image1->Canvas->Rectangle(left+(shift-8),top+5,left+(shift-8)+14,
                                   top+5+1);

        }

}//setDataSymbol



void TGraphForm::pixelAudioMarkerLine(int x, int pixelIncr)
{

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=top;j<=bottom;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(x,j);
            if((Image1->Canvas->Pixels[x][j]==m_cMonoBackCol)
                || (Image1->Canvas->Pixels[x][j]==m_cAudioClipMarkerColor) )
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
         }//j
      }//if
      if((Image1->Canvas->Pen->Color == m_cMonoBackCol) ||
            (Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor) )
      {
          //Erasing the audio position marker:
          for(int j=top;j<=bottom;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]== m_cAudioMarkerColor)
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
          }//j

      }
   }//if(plotMode==1)

}//pixelAudioMarkerLine




void TGraphForm::pixelClipMarkerLine(int x, int pixelIncr)
{
   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=top;j<=bottom;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]==m_cMonoBackCol )
            {
               //Only write pixel if not in wave signal:
                Image1->Canvas->LineTo(x,j+1);
            }
         }//j
      }//if
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
          //Erasing the audio position marker:
          for(int j=top;j<=bottom;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(x,j);
            if(Image1->Canvas->Pixels[x][j]== m_cAudioClipMarkerColor)
            {
               //Only erase pixel if audio clip marker color:
                Image1->Canvas->LineTo(x,j+1);
            }
          }//j

      }


   }//if(plotMode==1)


}//pixelClipMarkerLine



void TGraphForm::pixelHorMarkerLine(int y, int pixelIncr)
{

   if(plotMode == 1)
   {
      if(Image1->Canvas->Pen->Color == m_cAudioClipMarkerColor)
      {
         //Plotting the audio position marker:
         for(int j=left;j<=right;j+=pixelIncr)
         {
            Image1->Canvas->MoveTo(j,y);
            if(Image1->Canvas->Pixels[j][y]==m_cMonoBackCol )
            {
               //Only write pixel if not in signal:
               Image1->Canvas->LineTo(j+1,y);
            }
         }//j
      }//if
      if(Image1->Canvas->Pen->Color == m_cMonoBackCol)
      {
          //Erasing the audio position marker:
          for(int j=left;j<=right;j+=pixelIncr)
          {
            Image1->Canvas->MoveTo(j,y);
            if(Image1->Canvas->Pixels[j][y]== m_cAudioClipMarkerColor)
            {
               //Only erase pixel if audio clip marker color:
               Image1->Canvas->LineTo(j+1,y);
            }
          }//j

      }

   }//if(plotMode==1)


}//pixelHorMarkerLine





void __fastcall TGraphForm::Print1Click(TObject *Sender)
{


  TPrintDialog *printdlg = new TPrintDialog(this);
  printdlg->Options << poPageNums << poSelection;

  char tmp[10];



  TPrinter *Prntr = Printer();
  printer = Prntr;

  if( printdlg->Execute() )
  {

     PrintTimer->Enabled = true;
     Prntr->BeginDoc();


      Prntr->Canvas->Draw(0,0,Image1->Picture->Graphic);

      //Printer()->Canvas->TextOut(0, 0, "Hello, world");

      Prntr->EndDoc();


  }


}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Save1Click(TObject *Sender)
{

   //Save graph to file:
   char old_dir[MAXPATH],new_dir[MAXPATH];

   char fileName[100];
   char *ps;

   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);


   if( (!strcmp(m_sFileExt,"spe")) || (!strcmp(m_sFileExt,"txt")) ||
       (!strcmp(m_sFileExt,"dat")) || (!strcmp(m_sFileExt,"csv"))  )
   {
       strcpy(fileName,"");// Let user define filename in save dialog box
   }
   else
   {
      bool search = true;
      ps = strchr(Title,'.');
      while(search)
      {
        ps--;
        if( strchr(ps,'\\') )
        {
            search = false;
            ps++;
            strcpy(fileName,ps);
            bool scan = true;
            int i=0;
            while( scan )
            {
              if(fileName[i] == '.')
              {
                 scan = false;
                 fileName[i+1] = 'b';
                 fileName[i+2] = 'm';
                 fileName[i+3] = 'p';
                 fileName[i+4] = '\0';
              }
              else
              {
               i++;
              }
            }//while
        }
      }//while
   }

   SaveDialog1->DefaultExt = "bmp";
   SaveDialog1->Filter = "bmp";
   SaveDialog1->Title = "Save as bitmap file ...";
   SaveDialog1->FileName = fileName;
   if( SaveDialog1->Execute())
   {

      //Save bit map image to executable directory:
      Image1->Picture->SaveToFile(SaveDialog1->FileName);
      char mes[100];
      sprintf(mes,"%s%s","A bitmap image for this graphic has been saved as\n",
      SaveDialog1->FileName);
      MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
   }

   //Change back to original directory:
   if(chdir(old_dir) == -1)
   {
      MessageBox(NULL,"Problem changing to old dir!","",0);
   }

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::PrintTimerTimer(TObject *Sender)
{
   /*
   if(prntDlg == NULL)
   {
      prntDlg = new TPrintProgDlg(this);
      prntDlg->Show();
   }
   */
   if(printer->Printing)
   {
       char tmp[10];
       //prntDlg->Label5->Caption = "printing...";
       itoa(printer->PageNumber,tmp,10);
       //prntDlg->Label6->Caption = tmp;
       //prntDlg->Label7->Caption = printer->Printers->Strings[printer->PrinterIndex];
       strcpy(m_sPrinterName, (printer->Printers->Strings[printer->PrinterIndex]).c_str() );

   }
   else
   {
     //prntDlg->Label5->Caption = "completed.";
     //prntDlg->Label7->Caption = m_sPrinterName;
     if(printer != NULL)
     {
       printer = NULL;

     }
     //if(prntDlg != NULL)
     //{
       //prntDlg = NULL;
     //}

     PrintTimer->Enabled = false;

   }

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Exit1Click(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormResize(TObject *Sender)
{

  if(m_bAllowResize)
  {

      /* ------------------OLD VERSION -----------------------------------
      if(!Visible)
      {
          //Do nothing - form being created
      }
      else
      {
         if( (Height == m_iFormHeight) && (Width == m_iFormWidth) )
         {



         }
         else
         {

            if( !strcmp(m_sFile,"filcoeffs.txt") )
            {
               //Prevent resizing for this file:
               Height = m_iFormHeight;
               Width = m_iFormWidth;
               return;

            }

            //Redraw image:
            m_bResize = true;
            redrawCanvas(Width,Height);
            if(m_bIsWaveFile)
            {
               //Replot data:
               redrawWaveData();
            }
            else
            {
               redrawData();
            }
            m_bResize = false;
         }
       }
       ----------------------------------------------------------------- */

       if(!Visible)
      {
          //Do nothing - form being created
      }
      else
      {
         if( (Height == m_iFormHeight) && (Width == m_iFormWidth) )
         {



         }
         else
         {
            //Redraw image:
            m_bResize = true;

            if(m_bIsWaveFile)
            {
               //Replot data:
                redrawCanvas(Width,Height);
               redrawWaveData();
            }
            else
            {
               redrawCanvas1(Width,Height);
               redrawData1();
            }
            m_bResize = false;
         }
       }




  }
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Play1Click(TObject *Sender)
{

   int minSampleSize = 2;

   if(m_cAudioMarkerColor == clLime)   //Redefine to avoid problmes with audio clipper color
       m_cAudioMarkerColor = clRed;

   if(!m_bSelected)    //No area selected - play complete wavefile
   {


      //Check sample size:
      if(xmax < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

      //Enable graphic playback timer:
      Playback->Enabled = true;
      //Define interval for updating:
      int totSamples =  xmax;
      float timeper = (float)totSamples/(float)m_iSampleRate;
      if(timeper <= 3)
      {
         Playback->Interval = (timeper/10.0)*1000.0;
      }
      else
      {
         //Playback->Interval = ((timeper/10.0)*1000.0)/2.0;
         Playback->Interval = 500; //0.1sec
      }



      m_iCurrSampleNum = 0;
      changePlotMode(1); //pixel plot
      //m_cAudioMarkerColor = clRed;
      //m_cWaveSigColor = clBlack;


      /*
      //Commence play of audio wavefile
      TAudPlay *audioPlayer = new TAudPlay(this, Handle,
                                            m_iNumChannels,
                                            m_iSampleRate,
                                            m_iBitsPerSample,
                                            m_szAudioIniFile,Top,Left,
                                            true  //miniplayer toggle flag
                                            );
      AudioPlayer = audioPlayer;
      audioPlayer->SetPlayFromFileFlag(true);
      audioPlayer->SetWaveFileName(m_sFile);
      audioPlayer->Visible = false;
      audioPlayer->Playing();
      */
   }//if(!m_bSelected)
   else                       //Play selected region of wavefile
   {


      //Re-assign clip marker postions - start marker may have been dropped last
      if(m_fClipEndPos < m_fClipStartPos)
      {
         float tmp1;
         int tmp2;
         tmp1 = m_fClipStartPos;
         m_fClipStartPos = m_fClipEndPos;
         m_fClipEndPos = tmp1;
         tmp2 = m_iClipStartPixPos;
         m_iClipStartPixPos = m_iClipEndPixPos;
         m_iClipEndPixPos = tmp2;


      }

      //Check sample size:
      if(m_fClipEndPos - m_fClipStartPos < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

      //Get current directory:
      strcpy(m_sCurrDir,"X:\\");
      m_sCurrDir[0] = 'A' + getdisk();
      getcurdir(0,m_sCurrDir+3);

      sprintf(m_sClipWaveFile,"%s%s",m_sCurrDir,"\\clipped.wav");

     //Obtain wavefile for clipped region:
      floatvec clippedBuffer;
      clippedBuffer.reDim( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
      WAVETOOLS wavTools(m_iNumChannels,m_iSampleRate,
                         m_iBitsPerSample,0);

      wavTools.readWaveFile(m_sFile);
      //Get required region:
      clippedBuffer = wavTools.clipWaveFile(wavTools.getWaveBuffer(),
                                            (int) m_fClipStartPos,
                                            (int) m_fClipEndPos    );
     wavTools.upDateWaveBuffer(clippedBuffer);
     wavTools.setWaveFileName(m_sClipWaveFile);
     wavTools.makeWaveFile();



     //Enable graphic playback timer:
      Playback->Enabled = true;
      //Define interval for updating:

      int totSamples = ( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
      float timeper = (float)totSamples/(float)m_iSampleRate;
      if(timeper <= 3)
      {
         Playback->Interval = (timeper/10.0)*1000.0;
      }
      else
      {
         Playback->Interval = ((timeper/10.0)*1000.0)/2.0;
      }

      m_iCurrSampleNum = (int) m_fClipStartPos;
      changePlotMode(1); //pixel plot
      m_cAudioMarkerColor = clRed;
      //m_cWaveSigColor = clBlack;


      /*
      //Commence play of audio wavefile
      TAudPlay *audioPlayer = new TAudPlay(this, Handle,
                                            m_iNumChannels,
                                            m_iSampleRate,
                                            m_iBitsPerSample,
                                            m_szAudioIniFile,Top,Left,
                                            true  //miniplayer toggle flag
                                            );
      AudioPlayer = audioPlayer;
      audioPlayer->SetPlayFromFileFlag(true);
      audioPlayer->SetWaveFileName(m_sClipWaveFile);
      audioPlayer->Visible = false;
      audioPlayer->Playing();
      */

   }

}


unsigned long int TGraphForm::readWaveHeaderFile(FILE *fp)
{

   char a;
   unsigned long int size;
   unsigned int numberOfSamples;
   unsigned short wFormatTag,nBlockAlign,wBitsPerSample,nChannels,cbSize;
   unsigned long int nSamplesPerSec,nAvgBytesPerSec;
   short dataSample;


   //Read general file tag type (e.g. RIFF) and file size:
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index
   fread(&size,sizeof(unsigned long int),1,fp);

   //Read specific RIFF file type (e.g. WAVE):
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index

   //Read file format tag:
   for(int index=0;index<4;index++)
   {
      fread(&a,sizeof(char),1,fp);
   }//index

   //Read 'fmt' info size:
   fread(&size,sizeof(unsigned long int),1,fp);

   //Read struct wave_format_tag (WAVEFORMATEX)
   fread(&wFormatTag,sizeof(unsigned short),1,fp);
   fread(&nChannels,sizeof(unsigned short),1,fp);
   fread(&nSamplesPerSec,sizeof(unsigned long int),1,fp);
   fread(&nAvgBytesPerSec,sizeof(unsigned long int),1,fp);
   fread(&nBlockAlign,sizeof(unsigned short),1,fp);
   fread(&wBitsPerSample,sizeof(unsigned short),1,fp);
   fread(&cbSize,sizeof(unsigned short),1,fp);

   m_iSampleRate =  nSamplesPerSec;


   if( cbSize == 0 )
   {
      //Process for normal 'data' or  'fact'/'data' headers
      fread(&a,sizeof(char),1,fp);
      if(a == 'd')
      {
         //Process for normal 'data' header
         for(int index=0;index<3;index++)
         {
            fread(&a,sizeof(char),1,fp);
         }//index
         //Read data size:
         fread(&size,sizeof(unsigned long int),1,fp);


         return size;

      }//if
      else if(a == 'f')
      {
         //Process for  'fact' /'data' combination header
         for(int index=0;index<15;index++)
         {
            fread(&a,sizeof(char),1,fp);
         }//index
         //Read data size:
         fread(&size,sizeof(unsigned long int),1,fp);

         return size;

      }//if
      else
      {
        //Format structure not supported
        MessageBox(NULL,"Audio file format not supported.",BEST_MESSAGE,
                   MB_ICONINFORMATION);

      }
   }
   else
   {
      //Process for UtterSense Application format (earlier versions):
      //Next two bytes should be 't' and 'a'  (first two chars read with cbSize)
      //Read in remainder of 'data' header:
      for(int index=0;index<2;index++)
      {
         fread(&a,sizeof(char),1,fp);
      }//index

      //Read data:
      //Read data size:
      fread(&size,sizeof(unsigned long int),1,fp);
      return size;


   }

}//readWaveHeaderFile


void TGraphForm::redrawCanvas(int width,int height)
{
   //Redraw as a result of form resizing:

   //Clear existing picture:
   if(Image1 == NULL)
   {
      return;
   }
   else
   {
      if(monoMode == true)
      {
         Image2->Visible = false;
         image = 1;
         //Instantiate new image object:

         //Only redraw if we need to resize? - then we can get rid of flashing
         //when changing in animated mode.
         if(m_bResize)
         {
            delete Image1;
            Image1 = new TImage(this);
            Image1->Parent = this;
         }

         //Set up dimensions of image
         setupImageSize();

         Image1->Canvas->MoveTo(0,0);

         Image1->Canvas->Brush->Color = clWhite;
         TRect rect(0,0,Image1->Width,Image1->Height);
         Image1->Canvas->FillRect(rect);


         xincrement = 1; //Default value for pixel plot advances
         cirSize = 5;    //Initialise circle size for circle plots
         setLinePlotFlag(0);

         //Stet up graphic display area:
         setupGraphSize();

         //Color background for mon graph:
         TRect rect1(xmargin,ymargin,Image1->Width-xmargin,
                    Image1->Height-ymargin);
         MonoGraphRectangle = rect1;
         setMonoGraphBackground(m_cMonoBackCol);


         setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                     Image1->Height-ymargin,xLabel,yLabel);


        //Draw line through origin:
        if(logPlotMode == 0 || logPlotMode == 1)
        {
            C1 = ( ymax1/(ymax1-ymin1) )* (bottom-top);
            C1 = C1 + ymargin;  //Add margin offset
            Image1->Canvas->MoveTo(left,C1);
            Image1->Canvas->LineTo(right,C1);
            tagOrigin(image,left,C1);
        }
        counter1 = 0.0;
        strcpy(xfield,"5.1");
        strcpy(yfield,"5.1");
        //Draw graduations:
        if(getGridFlg())
         graduations(clWhite);
        //Draw axis scales:
        if(!Bitmap) axisScales();

      }

   }
}//redrawCanvas



void TGraphForm::redrawCanvas1(int width,int height)
{
   //Redraw as a result of form resizing:

   //Clear existing picture:
   if(Image1 == NULL)
   {
      return;
   }
   else
   {


      if(monoMode == true)
      {
         Image2->Visible = false;
         image = 1;
         //Instantiate new image object:

         //Only redraw if we need to resize? - then we can get rid of flashing
         //when changing in animated mode.
         if(m_bResize)
         {
            delete Image1;
            Image1 = new TImage(this);
            Image1->Parent = this;
         }

         //Set up dimensions of image
         setupImageSize();
         //Set up display graphic area:
         setupGraphSize();

         //Color background for mon graph:
         TRect rect(xmargin,ymargin,Image1->Width-xmargin,
         Image1->Height-ymargin);
         MonoGraphRectangle = rect;
         setMonoGraphBackground(m_cMonoBackCol);


         setupWindow(image,xmargin,ymargin,Image1->Width-xmargin,
                     Image1->Height-ymargin,xLabel,yLabel);



         //Draw graduations:
         if(showGrid)
         {
            if(m_cMonoBackCol == clBlack)
            {
               graduations(clWhite);
            }
            if(m_cMonoBackCol == clWhite)
            {
               graduations(clBlack);
            }
         }

         //Draw axis scales:
         if(!Bitmap) axisScales();

         if(show_ori_line)
         {
            Image1->Canvas->Brush->Color = origin_color;
            drawOriginLine();
         }

         //Redraw legend, if required:
         //if(m_bLegendOn)
         if (legend_flg)
         {

#if NUM_PLOTS == 1
	          info[0].type = LEGEND_PLOT_SYM1;
	          info[0].color = LEGEND_SYM1_COLOR;
	          info[0].text = LEGEND_CAPTION1;
#endif
#if NUM_PLOTS == 2
	          info[0].type = LEGEND_PLOT_SYM1;
	          info[0].color = LEGEND_SYM1_COLOR;
	          info[0].text = LEGEND_CAPTION1;
	          info[1].type = LEGEND_PLOT_SYM2;
	          info[1].color = LEGEND_SYM2_COLOR;
	          info[1].text = LEGEND_CAPTION2;
#endif
#if NUM_PLOTS == 3
	          info[0].type = LEGEND_PLOT_SYM1;
	          info[0].color = LEGEND_SYM1_COLOR;
	          info[0].text = LEGEND_CAPTION1;
	          info[1].type = LEGEND_PLOT_SYM2;
	          info[1].color = LEGEND_SYM2_COLOR;
	          info[1].text = LEGEND_CAPTION2;
	          info[2].type = LEGEND_PLOT_SYM3;
	          info[2].color = LEGEND_SYM3_COLOR;
	          info[2].text = LEGEND_CAPTION3;
#endif
#if NUM_PLOTS == 4
	          info[0].type = LEGEND_PLOT_SYM1;
	          info[0].color = LEGEND_SYM1_COLOR;
	          info[0].text = LEGEND_CAPTION1;
	          info[1].type = LEGEND_PLOT_SYM2;
	          info[1].color = LEGEND_SYM2_COLOR;
	          info[1].text = LEGEND_CAPTION2;
	          info[2].type = LEGEND_PLOT_SYM3;
	          info[2].color = LEGEND_SYM3_COLOR;
	          info[2].text = LEGEND_CAPTION3;
	          info[3].type = LEGEND_PLOT_SYM4;
	          info[3].color = LEGEND_SYM4_COLOR;
	          info[3].text = LEGEND_CAPTION4;
#endif

             legendCaptions1(0,LEGEND_BCK_COLOR,NUM_PLOTS,info,LEGEND_LEFT,LEGEND_TOP,
                                LEGEND_WIDTH,LEGEND_HEIGHT);
         }

         if (DRAW_VER_MARKER)
         {
	 	    drawVerticalMarker (V_MARKER_POS,V_MARKER_COLOR,MARKER_WIDTH);
         }



         Image1->Show();

      }


   }
}//redrawCanvas1



void TGraphForm::redrawData()
{
   floatmat data;
   int featureIndex, numFeatures;
   int endWordPos;
   char tmp[10], tmp1[10];

   //Plot recording data:
   if( (!strcmp(m_sFileExt,"txt")) )
   {
      data.textLoad(m_sFile);
   }
   else if( (!strcmp(m_sFileExt,"spe")) )
   {
      data.textLoad(m_sFile);
   }
   else if( (!strcmp(m_sFileExt,"fft")) )
   {
      data.textLoad(m_sFile);
      //Get square magnitude from complex data

      float r1,r2,max;
      data.SetPtrBegin();
      max = 0.0;
      for(int i=0;i<data.rows();i++)
      {
        r1 = *(data.curPtr());
        data++;
        r2 = *(data.curPtr());
        *(data.curPtr())= sqrt( pow( r1,2) + pow(r2,2) );
        if(*(data.curPtr()) > max)
           max = *(data.curPtr());
        data++;

      }//i
      //Normalise for plotting:
      if(max!=0.0)
      {
         data.normalize(0.75*(1.0/max));
      }

   }
   else
   {
      data.load(m_sFile);
   }

   if( !strcmp(m_sFileExt,"txt") )     //Plot r(t) file
   {
     //Plot data:
         //Define data plot color:
         setDataPlotColor(clWhite);
         setPenColor();

         data.SetPtrBegin();
         float x = 0.0;
         float max = 0.0;
         float yRange = max;
         float xRange = (float)data.rows();

         for(int i=0;i<data.rows();i++)
         {
            plotSig( *data.curPtr(),x);
            x += 1.0;
            if(i<data.rows()-1) data++;
         }//i

   }
   else if( !strcmp(m_sFileExt,"spe") || !strcmp(m_sFileExt,"fft") ) //Plot frequency spectrum
   {
      int sampleSize,xPlotStartVal;

      //Show graph form:
      float x = 0.0;
      float delta_f;
      float harmonic = 0.0;

      sampleSize = data.rows();
      //Define the spectral resolution:
      delta_f = (float) m_iSampleRate/sampleSize;

      //Set color for plotting data:
      setDataPlotColor(getSpectrum());
      setPenColor();

      setSFMarker(m_cSFmarker);


      float samfac = xmaxLinear/(float)m_iSampleRate;

      if(logPlotMode==1)
      {
         //Log plot
         xPlotStartVal = 1;
      }
      else if(logPlotMode==0)
      {
         xPlotStartVal = 0;
      }

      bool plot = true;
      while(plot)
      {
         data.SetPtrBegin();
         for(int j=xPlotStartVal;j<samfac*sampleSize;j++)
         {
            x = j*((float)m_iSampleRate/(float)sampleSize);
            x += harmonic*m_iSampleRate;
            if(j>sampleSize-1) break;
            if(x>1)
            {
               if(harmonic>0)
               {
                  setDataPlotColor(getHarmonics());
                  setPenColor();
               }
               if(x > xmaxLinear)
               {
                 plot = false;
                 break;
               }
               if(!strcmp(m_sFileExt,"spe"))
                  plotVerLine( *data(j,0),x);
               if(!strcmp(m_sFileExt,"fft"))
                  plotVerLine( *data(j,1),x);

            }

         }//j

         if(harmonic == 0.0)
         {
            if( (x >= m_iSampleRate - (1.5)*delta_f) && (x <= m_iSampleRate + (1.5)*delta_f) )
            {
               //Show sampling frequency:
               //Set color for plotting data:
               setDataPlotColor(getSFMarker());
               setPenColor();
               plotVerLine( 0.5*ymin1,x);
               plotVerLine( -0.5*ymin1,x);
               //Reset color for plotting data:
               setDataPlotColor(getSpectrum());
               setPenColor();
            }
         }
         if(m_bHarmonic == false) plot = false;
         harmonic+= 1.0;
      }//while

      m_bIsWaveFile = false;
      Play1->Enabled = false;

   }
   else
   {   //Plot cepstral coefficients:

      //Get maximum value for scaling:
      numFeatures = data.cols();

      //Hard code on feature index:
      featureIndex = 0;

      //Show graph form:
      float x = 0.0;
      data.SetPtrBegin();
      for(int j=0;j<featureIndex;j++) data++;
      for(int i=0;i<data.rows();i++)
      {
         plotSig( *data.curPtr(),x);
         x += 1.0;
         if(i<data.rows()-1)
         {
            for(int j=0;j<numFeatures;j++) data++;

         }
         //if(i<data.rows()-1) data++;
      }//i
   }

}//redrawData


void TGraphForm::redrawData1()
{


#if NUM_PLOTS == 1
		if (!PLOT1_SUPPRESS)
		{
			changePlotMode(PLOT1_MODE);
			lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data2[i] >= Y_MIN) && (EasyPlotForm->data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data2[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(EasyPlotForm->data2[i],EasyPlotForm->data1[i],PLOT1_COLOR);
					}

				}
			}
		}
#endif
#if NUM_PLOTS == 2
      if (!PLOT1_SUPPRESS)
		{
			changePlotMode(PLOT1_MODE);
			lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data2[i] >= Y_MIN) && (EasyPlotForm->data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data2[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data2[i],EasyPlotForm->data1[i],PLOT1_COLOR);
					}
				}
			}
		}
		if (!PLOT2_SUPPRESS)
		{
			changePlotMode(PLOT2_MODE);
			lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data3[i] >= Y_MIN) && (EasyPlotForm->data3[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data3[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data3[i],EasyPlotForm->data1[i],PLOT2_COLOR);
					}
				}
			}
		}
#endif
#if NUM_PLOTS == 3
		if (!PLOT1_SUPPRESS)
		{
			changePlotMode(PLOT1_MODE);
			lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data2[i] >= Y_MIN) && (EasyPlotForm->data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data2[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data2[i],EasyPlotForm->data1[i],PLOT1_COLOR);
					}
				}
			}
		}

		if (!PLOT2_SUPPRESS)
		{
			changePlotMode(PLOT2_MODE);
			lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data3[i] >= Y_MIN) && (EasyPlotForm->data3[i] <= Y_MAX) )   )
				{

					if (!_isnan(EasyPlotForm->data3[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data3[i],EasyPlotForm->data1[i],PLOT2_COLOR);
					}
				}
			}
		}

		if (!PLOT3_SUPPRESS)
		{
			changePlotMode(PLOT3_MODE);
			lineWidth ( PLOT3_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data4[i] >= Y_MIN) && (EasyPlotForm->data4[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data4[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data4[i],EasyPlotForm->data1[i],PLOT3_COLOR);
					}
				}
			}
		}

#endif
#if NUM_PLOTS == 4
		if (!PLOT1_SUPPRESS)
		{
			changePlotMode(PLOT1_MODE);
			lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data2[i] >= Y_MIN) && (EasyPlotForm->data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data2[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data2[i],EasyPlotForm->data1[i],PLOT1_COLOR);
					}
				}
			}
		}

		if (!PLOT2_SUPPRESS)
		{
			changePlotMode(PLOT2_MODE);
			lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data3[i] >= Y_MIN) && (EasyPlotForm->data3[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data3[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data3[i],EasyPlotForm->data1[i],PLOT2_COLOR);
					}
				}
			}
		}

		if (!PLOT3_SUPPRESS)
		{
			changePlotMode(PLOT3_MODE);
			lineWidth ( PLOT3_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data4[i] >= Y_MIN) && (EasyPlotForm->data4[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data4[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data4[i],EasyPlotForm->data1[i],PLOT3_COLOR);
					}
				}
			}
		}

		if (!PLOT4_SUPPRESS)
		{
			changePlotMode(PLOT4_MODE);
			lineWidth ( PLOT4_LINE_WIDTH ); //for line plots
			for(int i= 0;i <EasyPlotForm->NumDataSets;i++)
			{
				if ( ((EasyPlotForm->data1[i] >= X_MIN) && (EasyPlotForm->data1[i] <= X_MAX) ) &&
				    ((EasyPlotForm->data5[i] >= Y_MIN) && (EasyPlotForm->data5[i] <= Y_MAX) )   )
				{
					if (!_isnan(EasyPlotForm->data5[i])) // Do not plot NAN numbers
					{
						plotSig(EasyPlotForm->data5[i],EasyPlotForm->data1[i],PLOT4_COLOR);
					}
				}
			}
		}
#endif


}//redrawData1


void TGraphForm::redrawData(floatvec& data)  //fft only
{

   int sampleSize,xPlotStartVal;

   //Show graph form:
   float x = 0.0;
   float delta_f;
   float harmonic = 0.0;

   sampleSize = data.rows();
   //Define the spectral resolution:
   delta_f = (float) m_iSampleRate/sampleSize;

   //Define grid plot color:
   setDataPlotColor(getGridLines());
   setPenColor();
   graduations(clWhite);
   //Redraw origin in required pen color:
   setDataPlotColor(getOrigin());
   setPenColor();
   drawOriginLine();

   //Set color for plotting data:
   setDataPlotColor(getSpectrum());
   setPenColor();

   float samfac = xmaxLinear/(float)m_iSampleRate;

   if(logPlotMode==1)
   {
      //Log plot
      xPlotStartVal = 1;
   }
   else if(logPlotMode==0)
   {
      xPlotStartVal = 0;
   }


   bool plot = true;
   while(plot)
   {
      data.SetPtrBegin();
      for(int j=xPlotStartVal;j<samfac*sampleSize;j++)
      {
         x = j*((float)m_iSampleRate/(float)sampleSize);
         x += harmonic*m_iSampleRate;
         if(j>sampleSize-1) break;
         if(x>1)
         {
            if(harmonic>0)
            {
               setDataPlotColor(getHarmonics());
               setPenColor();
            }
            if(x > xmaxLinear)
            {
              plot = false;
              break;
            }
            plotVerLine( *data(j,0),x);

         }

      }//j


      if(harmonic == 0.0)
      {
         if( (x >= m_iSampleRate - (1.5)*delta_f) && (x <= m_iSampleRate + (1.5)*delta_f) )
         {
            //Show sampling frequency:
            //Set color for plotting data:
            setDataPlotColor(getSFMarker());
            setPenColor();
            plotVerLine( 0.5*ymin1,x);
            plotVerLine( -0.5*ymin1,x);
            //Reset color for plotting data:
            setDataPlotColor(getSpectrum());
            setPenColor();
         }
      }
      if(m_bHarmonic == false) plot = false;
      harmonic+= 1.0;
   }//while

   m_bIsWaveFile = false;
   Play1->Enabled = false;

}//redrawData



void TGraphForm::redrawWaveData()
{

   FILE *fp;
   unsigned int numberOfSamples;
   short dataSample;

   if(checkForWaveFile())
   {
      //Read wave file header:
      if( (fp=fopen(m_sFile,"rb")) == NULL)
      {
        MessageBox(NULL,"Cannot open required wave file!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
        return;
      }//if

      //Instantiate graph.cpp and plot recording data:
      numberOfSamples = readWaveHeaderFile(fp);
      numberOfSamples = numberOfSamples/2;
      //N.B.: We are assuming 16-bit resolution here
      //      (2 bytes per sample);
      //numberOfSamples += 1;

      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;
      while( !feof(fp) )
      {
         fread(&dataSample,sizeof(short),1,fp);
         plotSig( (float)dataSample,x);
         x += 1;

      }//while

       fclose(fp);

      m_bIsWaveFile = true;
      Play1->Enabled = true;

   }//if
   if(checkForIFTFile())
   {
      floatvec iftspec;
      iftspec.load(m_sFile);
      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;
      iftspec.SetPtrBegin();
      for(int i=0;i<iftspec.rows();i++)
      {
         plotSig(*(iftspec.curPtr()),x);
         x += 1;
         iftspec++;

      }//i

   }//if

   m_bIsWaveFile = true;
   Play1->Enabled = true;

}//redrawWaveData
//---------------------------------------------------------------------------

void TGraphForm::redrawWaveData(floatvec& data) //Live data on stream
{
      //Plot data:
      float x = 0.0;
      Image1->Canvas->Pen->Color = m_cDataPlotColor;

      data.SetPtrBegin();
      for(int i=0;i<data.rows();i++)
      {
         plotSig( *data.curPtr(),x);
         x += 1;
         data++;
      }

      m_bIsWaveFile = true;
      Play1->Enabled = true;

}//redrawWaveData


//---------------------------------------------------------------------------




bool TGraphForm::checkForWaveFile()
{
   char *pstr;
   int len;

   //Check the file extension:
   len = strlen(m_sFile);
   if(len==0) return false;     //No data file defined
   pstr = strchr(m_sFile,'.');
   pstr++;
   if( !strcmp(pstr,"wav") )
   {
      //Wave file flagged:
      return true;
   }
   else
   {
      strcpy(m_sFileExt,pstr);
      return false;
   }


}//checkForWaveFile

//---------------------------------------------------------------------------
bool TGraphForm::checkForIFTFile()
{
   char *pstr;
   int len;

   //Check the file extension:
   len = strlen(m_sFile);
   if(len==0) return false;     //No data file defined
   pstr = strchr(m_sFile,'.');
   pstr++;
   if( !strcmp(pstr,"ift") )
   {
      //Wave file flagged: - treat .ift like a wave file (time domain signal plot)
      strcpy(m_sFileExt,pstr);
      return true;
   }
   else
   {
      strcpy(m_sFileExt,pstr);
      return false;
   }


}//checkForIFTFile



bool TGraphForm::checkForCsvFile()
{
   char *pstr;
   int len;

   //Check the file extension:
   len = strlen(m_sFile);
   if(len==0) return false;     //No data file defined
   pstr = strchr(m_sFile,'.');
   pstr++;
   if( !strcmp(pstr,"csv") )
   {
      //Csv file flagged:
      strcpy(m_sFileExt,pstr);
      return true;
   }
   else
   {
      strcpy(m_sFileExt,pstr);
      return false;
   }


}//checkForCsvFile



//---------------------------------------------------------------------------



void TGraphForm::deSelectWaveClip()
{
   //Deselect wave file clip:

   //N.B.: This function is called  either by  a left double-click,
   //      OR  a right-click (this is better - more control and less likely
   //      to move mouse    if(m_bSelected)

   if(m_bSelected)
   {
      //Erase start clip marker
      pixelClipMarkerLine(m_iClipStartPixPos,1);

      //Erase end clip marker
      pixelClipMarkerLine(m_iClipEndPixPos,1);

     Image1->Canvas->Brush->Color = m_cMonoBackCol;
     //Fill upper region:
     Image1->Canvas->FloodFill(m_iClipStartPixPos,ymargin+2,m_cAudioClipMarkerColor,fsSurface);
     //Fill lower region:
     Image1->Canvas->FloodFill(m_iClipStartPixPos,Image1->Height-ymargin-2,m_cAudioClipMarkerColor,fsSurface);

      //Remove clip file from hard disc
      remove(m_sClipWaveFile);

      //Reset flags:
      m_bSelected = false;
      m_bLastClickWasDouble =true;

   }//if(m_bSelected)


}//deSelectWaveClip

//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(m_bLegendMove && !m_bIsWaveFile)
  {
	  //NEW CODE HERE------------------------------------------------------
	  //Draw hozizontal marker line if in left margin:
	  //Mouse is down:
	  //Get x value relating to this position:
	  if(X < Image1->Left)
	  {
		 int pixres = 1;//200;

		 m_iCurrYPixPos = Y - Image1->Top;
		 m_iLastYPixPos = m_iCurrYPixPos;

		 changePlotMode(1); //pixel plot
		 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
		 //Draw marker at this point:
		 pixelHorMarkerLine(m_iCurrYPixPos,1);
	  }
	  if( (Y > Image1->Top + Image1->Height) && (X > Image1->Left))
	  {


		if( Shift.Contains(ssLeft) )
		{

			 int pixres = 1;//200;

			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 m_iCurrXPixPos = X;
			 m_iLastXPixPos = m_iCurrXPixPos;
			 m_iClipStartPixPos = X;
			 m_fClipStartPos = getXValFromPixPos(X);
			 pixelClipMarkerLine(X,1);
		}
	  }

	 //END NEW CODE--------------------------------------------------------

	  return;
  }

  if(m_bIsWaveFile)
  {
	   //Initialise abort flag:
	   m_bAbortSelection = false;

	   //Redefine mouse position in terms of Image position:
	   X = X - Image1->Left;

	   //Check to see whether mouse is in graph area:
	   if( X>xmargin && X<Image1->Width-xmargin)
	   {
		  m_bAbortSelection = false;
	   }
	   else
	   {
		  m_bAbortSelection = true;
		  return;
	   }


	   if( Shift.Contains(ssDouble) )
	   {
			//Deselect current selected wave clip
			//deSelectWaveClip();

	   }
	   else  //Single mouse-down click
	   {

			 if( !Shift.Contains(ssLeft) )   //Right-click to perform de-selection
			 {
				//Deselect current selected wave clip
				deSelectWaveClip();

				return;
			 }

			 m_iCurrPixPos = X;
			 m_iLastPixPos = m_iCurrPixPos;

			 if(X <  xmargin ) return;
			  //Drop line to indicate start of wave clip:
			 if( !m_bSelected)
			 {
				changePlotMode(1); //pixel plot
				Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
				//Draw marker at this point:
				//Get x value relating to this position:
				m_iClipStartPixPos = X;
				m_fClipStartPos = getXValFromPixPos(X);

				pixelClipMarkerLine(X,1);

			 }//if( !m_bSelected)

	   }
  }//if(m_bIsWaveFile


}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{


      if(m_bLegendMove && !m_bIsWaveFile)
	  {
		 //Redraw image:
		 //Get new position for legend:
		 //Check to see if we are using bottom of form for positioning left horizontal position:

		 if(Y >= (Height-80) )
		 {
			//m_iLegendLeftPos = X - Image1->Left + 5;
			m_iLegendLeftPos = X - Image1->Left + 19;
			//m_iLegendTopPos = Y;
			redrawCanvas1(Width,Height);
			redrawData1();
			//m_bLegendMove = false;
		 }
		 //Check to see if we are using left of form for positioning vertical position:
		 if(X <= 80 )
		 {
			//m_iLegendLeftPos = X;
			m_iLegendTopPos = Y - Image1->Top + 5;
			redrawCanvas1(Width,Height);
			redrawData1();
			//m_bLegendMove = false;
		 }



		 return;

	  }
	  if(m_bIsWaveFile)
	  {
		  if(m_bAbortSelection) return;
		  //Redefine mouse position in terms of Image position:
		  X = X - Image1->Left;

			 if(!m_bSelected && m_bLastClickWasDouble)
			 {
				//Performing deselection:
				m_bLastClickWasDouble = false;

			 }
			 else if((!m_bSelected && !m_bLastClickWasDouble))
			 {

				Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
				//Get x value relating to this position:
				m_iClipEndPixPos = X;
				m_fClipEndPos = getXValFromPixPos(X);

				//Re-assign clip marker positions - start marker may have been dropped last
				if(m_fClipEndPos < m_fClipStartPos)
				{
				   float tmp1;
				   int tmp2;
				   tmp1 = m_fClipStartPos;
				   m_fClipStartPos = m_fClipEndPos;
				   m_fClipEndPos = tmp1;
				   tmp2 = m_iClipStartPixPos;
				   m_iClipStartPixPos = m_iClipEndPixPos;
				   m_iClipEndPixPos = tmp2;

				}

				//Check sample interval - if too small abort selection process:
			   if( m_iClipEndPixPos - m_iClipStartPixPos < 3 )
			   {

				   //Erase start clip marker
				   Image1->Canvas->Pen->Color = m_cMonoBackCol;
				   pixelClipMarkerLine(m_iClipStartPixPos,1);

				   //Erase end clip marker
				   pixelClipMarkerLine(m_iClipEndPixPos,1);
				   return;
			   }


				//Flag that clip region has been selected:
				m_bSelected = true;


				//Color selected area between start and end of clip positions:
			   Image1->Canvas->Brush->Color = m_cAudioClipMarkerColor;
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cMonoBackCol,fsSurface);

			   //Fill lower region:
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cMonoBackCol,fsSurface);

			 }//if(!m_bSelected)
			 else if((m_bSelected && !m_bLastClickWasDouble))
			 {
				//Color in new selected area:
				 //Color selected area between start and end of clip positions:
			   Image1->Canvas->Brush->Color = m_cAudioClipMarkerColor;
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cMonoBackCol,fsSurface);

			   //Fill lower region:
			   Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cMonoBackCol,fsSurface);

			 }

	  }//if(m_bIsWaveFile

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TGraphForm::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{

    //NEW CODE----------------------------------------------------------------
   if(m_bLegendMove && !m_bIsWaveFile)
   {
	   if(X < Image1->Left) //In left margin, create horizontal line at Y position
	   {                    //For moving legend only

		  if(Shift.Contains(ssLeft) )
		  {

			 //Erase marker for previous marker position:
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelHorMarkerLine(m_iLastYPixPos,1);

			 //Plot marker for current marker position:
			 m_iCurrYPixPos = Y - Image1->Top;
			 m_iLastYPixPos = m_iCurrYPixPos;
			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 pixelHorMarkerLine(m_iCurrYPixPos,1);

		  }

	   }//if
	   if( (Y > Image1->Top + Image1->Height) && (X > Image1->Left))
	  {

		 if(Shift.Contains(ssLeft) )
		  {

			 int pixres = 1;//200;

			 //Erase marker for previous marker position:
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelHorMarkerLine(m_iLastYPixPos,1);
			 pixelClipMarkerLine(m_iLastXPixPos,1);

			 changePlotMode(1); //pixel plot
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
			 //Draw marker at this point:
			 m_iCurrXPixPos = X;
			 m_iLastXPixPos = m_iCurrXPixPos;
			 m_iClipStartPixPos = X;
			 m_fClipStartPos = getXValFromPixPos(X);
			 pixelClipMarkerLine(X,1);
		 }
	  }

	  return;


   }
   //END OF NEW CODE-------------------------------------------------------

   if(m_bIsWaveFile)
   {
	   if(m_bAbortSelection) return;


	   //Process for a vertical line at given X position:

	   //Redefine mouse position in terms of Image position:
	   X = X - Image1->Left;

	   if(!m_bSelected && Shift.Contains(ssLeft) )
	   {

		 //Mouse is down and we are still defining selected area:
		 //Get x value relating to this position:
		  int pixres = 1;//200;
		  m_iCurrPixPos = X;

		  //Erase marker for previous marker position:
		  if( (m_iLastPixPos != m_iClipStartPixPos) )
		  {
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;

			 pixelClipMarkerLine(m_iLastPixPos,1);
			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

		  }



		  //Plot marker for current marker position:
		  pixelClipMarkerLine(X,1);
		  if(!m_bLegendMove) pixelClipMarkerLine(m_iClipStartPixPos,1);

		  m_iLastPixPos = m_iCurrPixPos;

	   }
	   else if(m_bSelected && Shift.Contains(ssLeft) )
	   {

		  //if( (X > m_iClipStartPixPos) && (X < m_iClipEndPixPos) )
		  //{

			//Area already selected - move area to another location:
			//Get x value relating to this position:

			 m_iCurrPixPos = X;


			 //Deselect current selected wave clip
			 //Erase start clip marker
			 Image1->Canvas->Pen->Color = m_cMonoBackCol;
			 pixelClipMarkerLine(m_iClipStartPixPos,1);
			 //Erase end clip marker
			 pixelClipMarkerLine(m_iClipEndPixPos,1);

			 Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

			Image1->Canvas->Brush->Color = m_cMonoBackCol;
			//Fill upper region:
			Image1->Canvas->FloodFill(m_iClipStartPixPos+1,ymargin+2,m_cAudioClipMarkerColor,fsSurface);
			//Fill lower region:
			Image1->Canvas->FloodFill(m_iClipStartPixPos+1,Image1->Height-ymargin-2,m_cAudioClipMarkerColor,fsSurface);



			//Obtain current marker positions:
			float m_fNewClipStartPos,m_fNewClipEndPos,m_iNewClipStartPixPos,m_iNewClipEndPixPos;

			m_iNewClipStartPixPos = m_iCurrPixPos;
			m_iNewClipEndPixPos =   m_iNewClipStartPixPos + (m_iClipEndPixPos-m_iClipStartPixPos);
			m_fNewClipStartPos = getXValFromPixPos(m_iNewClipStartPixPos);
			m_fNewClipEndPos = getXValFromPixPos(m_iNewClipEndPixPos);


			//Put markers in for current area:
			Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;

			pixelClipMarkerLine(m_iNewClipStartPixPos,1);

			pixelClipMarkerLine(m_iNewClipEndPixPos,1);


			//Update start and end positions:
			m_iClipStartPixPos = m_iNewClipStartPixPos;
			m_iClipEndPixPos =   m_iNewClipEndPixPos;
			m_fClipStartPos =  m_fNewClipStartPos;
			m_fClipEndPos =  m_fNewClipEndPos;


			 m_iLastPixPos = m_iCurrPixPos;
		  //}
	   }
   }//if(m_bISWaveFile

   
}
//---------------------------------------------------------------------------






/*---------------------------------------------------------------------------
//Timer and Message handlers
----------------------------------------------------------------------------*/

void __fastcall TGraphForm::MMmciNotify(TMessage Message)
{

   if(Message.WParam == MCI_NOTIFY_SUCCESSFUL)
   {
      /*
      if(AudioPlayer->GetPauseStatus() == false)
      {
         //Playback has come to an end:
         AudioPlayer->SetDevicePlayStatus(false);
         AudioPlayer->StopPlay();
         if(AudioPlayer->Visible == false)
         {
            //If audioplayer in invisible mode (used for
            //playback with graphic animation)
            delete AudioPlayer;
            AudioPlayer = NULL;
            //playback->Enabled = false;

         }

      }
      else
      {
         //Allow playback to continue after pause
      }
      */
   }
   else if(Message.WParam == MCI_NOTIFY_ABORTED)
   {
      /*
      if(AudioPlayer->ResumePlayStatus() == false)
      {
         //Pause/resume play has not been enacted; abort as normal:
         AudioPlayer->SetDevicePlayStatus(false);
         AudioPlayer->StopModeView();
      }
      else
      {
         //Resume play has just been enacted; this sends an
         //abort message from the previous play command which
         //needs to be ignored. However, reset ResumePlay flag to
         //false:
         AudioPlayer->SetResumePlayStatus(false);
      }
      */
   }
   else if(Message.WParam == MCI_NOTIFY_SUPERSEDED)
   {

   }
   else if(Message.WParam == MCI_NOTIFY_FAILURE)
   {
      MessageBox(NULL,"Device Failure!",BEST_MESSAGE,MB_ICONINFORMATION);
   }

}//MMmciNotify


void __fastcall TGraphForm::PlaybackTimer(TObject *Sender)
{

   //Update current position of speech during audio playback:

   /*
   FILE *fp;
   fp = fopen("test.txt","at");
   fprintf(fp,"%d %d\n",m_iSigLower,m_iSigUpper);
   fclose(fp);
   */

   //Define line pixel resolution:
   int pixres = 100;
   //Offset from bottom of canvas to avoid problems with confuding signal with canvas
   int offset = 1000; //N.B.: This is specific to +-33000 x axis - make general for any axis
   if(!m_bSelected)             //Play back complete recording:
   {
      //First, remove previous marker position:
      if(m_iCurrSampleNum > 0)
      {

         Image1->Canvas->Pen->Color = m_cMonoBackCol;
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            //plotAudioMarkerPos(i,m_iCurrSampleNum);
            plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
       }//if

      //Draw marker at this point:
      Image1->Canvas->Pen->Color = m_cAudioMarkerColor;
      m_iCurrSampleNum += (((float)Playback->Interval)*((float)m_iSampleRate))/1000.0;
      if(m_iCurrSampleNum < xmax)
      {

         m_iSigUpper = 0;
         m_iSigLower = 0;
         m_bSigTrace = false;
         m_bSigRangeDefined = false;
         //Draw marker at this point:
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            //plotAudioMarkerPos(i,m_iCurrSampleNum);
            plotAudioMarkerPos1(i,m_iCurrSampleNum);
         }
      }
      else
      {
         Playback->Enabled = false;
      }
   }//if(!m_bSelected)
   else                              //Selection made: play back required clip
   {

      //First, remove previous marker position:
      if(m_iCurrSampleNum > 0)
      {
         Image1->Canvas->Pen->Color = m_cAudioClipMarkerColor;
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            plotAudioMarkerPos(i,m_iCurrSampleNum);
            //plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
       }//if

      //Draw marker at this point:
      Image1->Canvas->Pen->Color = m_cAudioMarkerColor;
      m_iCurrSampleNum += (((float)Playback->Interval)*((float)m_iSampleRate))/1000.0;
      if(m_iCurrSampleNum < (int) m_fClipEndPos)
      {
         m_iSigUpper = 0;
         m_iSigLower = 0;
         m_bSigTrace = false;
         m_bSigRangeDefined = false;
         //Draw marker at this point:
         for(int i=ymin1+offset;i<ymax1-offset;i+=pixres)
         {
            plotAudioMarkerPos1(i,m_iCurrSampleNum);

         }
      }
      else
      {
         Playback->Enabled = false;
      }


   }



}
//---------------------------------------------------------------------------



void __fastcall TGraphForm::SelectedRegion1Click(TObject *Sender)
{
  //Create wav file out of current selected region:

  char old_dir[MAXPATH],new_dir[MAXPATH];

  //Get current directory:
  strcpy(old_dir,"X:\\");
  old_dir[0] = 'A' + getdisk();
  getcurdir(0,old_dir+3);

  int minSampleSize = 2;

   if(m_bSelected)     //Range is currently selected
   {

      //Check sample size:
      if(m_fClipEndPos - m_fClipStartPos < minSampleSize)
      {
         //Abort playback - sample too small
         char mes[200];
         sprintf(mes,"%s%s","The sample size is too small for playback.\n",
                 "Please select a larger sample.");
         MessageBox(NULL,mes,BEST_MESSAGE,MB_ICONINFORMATION);
         return;

      }

     //Open save dialog box to define file:
     SaveDialog1->DefaultExt = "wav";
     SaveDialog1->Filter = "wav";
     SaveDialog1->Title = "Save selected signal range to ...";
     SaveDialog1->FileName = "*.wav";
     if( SaveDialog1->Execute())
     {

         //Obtain wavefile for clipped region:
         floatvec clippedBuffer;
         clippedBuffer.reDim( (int) m_fClipEndPos - (int) m_fClipStartPos + 1);
         WAVETOOLS wavTools(m_iNumChannels,m_iSampleRate,
                            m_iBitsPerSample,0);

         wavTools.readWaveFile(m_sFile);
         //Get required region:
         clippedBuffer = wavTools.clipWaveFile(wavTools.getWaveBuffer(),
                                               (int) m_fClipStartPos,
                                               (int) m_fClipEndPos    );
        wavTools.upDateWaveBuffer(clippedBuffer);
        wavTools.setWaveFileName( (SaveDialog1->FileName).c_str() );
        wavTools.makeWaveFile();
        MessageBox(NULL,"Selected region has been saved as audio file.",
                   BEST_MESSAGE,MB_ICONINFORMATION);
     }
     else
     {

     }

     //Change back to original directory:
     if(chdir(old_dir) == -1)
     {
        MessageBox(NULL,"Problem changing to old dir!","",0);
     }

   }//if(m_bSelected)

}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormDblClick(TObject *Sender)
{
   //Double click to move the legend
   if(!m_bIsWaveFile)     //Only operate double clicks for non-wave files:
   {
      m_bLegendMove = true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	//Toggle to delete (close) the forn
	//(for use when requiring screen capture, and the Menu and top border controls
	// are disabled)
    switch( Key )
	{

		case VK_DELETE:
	   	Close();
	   	break;

	}


}

#endif









