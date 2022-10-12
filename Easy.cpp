//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
#include "Easy.h"
#include "applaunch.hpp"




//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEasyPlotForm *EasyPlotForm;


//-------------------------------------------------------------------------------


//---------------------------------------------------------------------------
__fastcall TEasyPlotForm::TEasyPlotForm(TComponent* Owner)
   : TForm(Owner)
{
   char currDir[200];

   //Form Positioning:
   Left = 300;


   //Initialise base pointer to shape animation classes:
   //m_pShape = NULL;

   //Initialise with default settings for graphics
   m_cBackground = clBlack;
   m_cGridlines  = clWhite;
   m_cOrigin     = clWhite;
   m_cSignal     = clWhite;
   m_cSpectrum   = clLime;

   m_cAudioMarker = clRed;

   //Legend default settings:
   m_iLeftPos = 50;
   m_iTopPos = 50;

   //Default data setting:
   m_iNumDataSets = 1;
   m_bXDataFileSet = true; //x-axis data defined in data source file
   CheckBox3->Checked = true;
   for(int i=0;i<MAX_NUM_DATASETS;i++)
   {
	  m_bSkipSet[i] = false;
   }

   //Set default data plot colors:
   m_cDataSet[0] = clWhite;
   m_cDataSet[1] = clRed;
   m_cDataSet[2] = clGreen;

   //Color for legend background:
   m_cLegend = clBlack;

   //Set default for legend
   m_bLegendOn = true;

   //Set default legend text:
   String str1 = "Data Set 1";
   String str2 = "Data Set 2";
   String str3 = "Data Set 3";
#ifdef WIDE_CHAR_APP
   StringToWideChar(str1,m_swLegendText1,99);
   StringToWideChar(str2,m_swLegendText2,99);
   StringToWideChar(str3,m_swLegendText3,99);
#else
   strcpy(m_sLegendText1,"Data Set 1");
   strcpy(m_sLegendText2,"Data Set 2");
   strcpy(m_sLegendText3,"Data Set 3");
#endif

   //Bar graph attributes:
   m_iBarWidth = 10;

   //Set default plot modes for data sets:
   for(int i=0;i<MAX_NUM_DATASETS;i++)
   {
	  m_iPlotMode[i] = 3; //Default to rectangle data plot type
   }

   //Initial setting for  file type:
   m_bIsWaveFile = false;

   m_bTitleOverride = false;

   //Define default values for graph size:
   //m_iHeight = Height; //292 pixels
   //m_iWidth = 1.35*(414); //559 pixels

   //m_iHeight = 768;//700;
   //m_iWidth = 1368;//1350; //559 pixels
   m_iHeight = 720;//700;
   m_iWidth  = 1280;

  //Define size of data plot points (rectangle or circle):
  m_iDataPlotSize =8; //pixels

  //Define flag for graph border:
  m_bDrawBorder =true;



   CheckBox2->Checked = true;


   //Define Autoscale text:
   strcpy(m_sAutoNumStr,"autoscale");
   //Set Autoscale flag:
   if(CheckBox2->Checked)
   {
   	   Edit6->Enabled = false;
	   Edit7->Enabled = false;
	   Edit5->Enabled = false;
	   Edit8->Enabled = false;

	   m_bAutoScale = true;
 #ifdef WIDE_CHAR_APP
      String uniStr = m_sAutoNumStr;
	  wchar_t buf[20];
	  StringToWideChar(uniStr,buf,19);
	  Edit5->SetTextBuf(buf);
	  Edit6->SetTextBuf(buf);
	  Edit7->SetTextBuf(buf);
	  Edit8->SetTextBuf(buf);
#else
      char tmp[20];
      strcpy(tmp,"autoscale");
      Edit5->SetTextBuf(tmp);
      Edit6->SetTextBuf(tmp);
      Edit7->SetTextBuf(tmp);
      Edit8->SetTextBuf(tmp);
#endif
	  m_bAutoXMin = true;
	  m_bAutoXMax = true;
	  m_bAutoYMin = true;
	  m_bAutoYMax = true;

   }
   else
   {
	   m_bAutoScale = false;
	   m_bAutoXMin = false;
	   m_bAutoXMax = false;
	   m_bAutoYMin = false;
	   m_bAutoYMax = false;
   }


   //Craph features:
   m_bXGrad = true;
   m_bYGrad = true;
   m_bXAxisShow = true;
   m_bYAxisShow = true;

   //Animation effects:
   m_bAnimation = false;
   m_bMovieControl = false;
   m_bProgrammedControl = false;

   //Movie flag sub-direcoty ovveride:
   m_bOverride = false; //Flag to ovveride creation of separate sub-dirs for each program tag file
   //Plot delay time (msec):
   m_iViewDelay = 100;
   m_GraphFrm = NULL;
   //Filename tag for movie stills:
   strcpy(m_sMovieTag,"tagname");
   //Target directory for movie stills:
   char curr_dir[MAXPATH];
   //Get current directory:
   strcpy(curr_dir,"X:\\");
   curr_dir[0] = 'A' + getdisk();
   getcurdir(0,curr_dir+3);
	//strcpy(m_sMovieTarget,curr_dir);
	//Flag for viewing movie stills on creation:
   m_bViewStill = false;  //true
   //Initialise file pointer for tag and def file:
   m_fpTagFile = NULL;
   m_fpDefFile = NULL;
   m_bFileIgnore = false;

   //Initialsie flag for multi tag file usage:
   m_bMultiTagFiles = false;
   m_SLTagFiles = NULL;
   m_SLContIndex = NULL;

   //Initialise pointer for movie preview:
   m_ImMovie = NULL;
   m_MovieForm = NULL;
   m_iPreviewInt = 10; //ms
   m_iPreviewIndexStart = 1; //Start frame index

   //Get drive on which app. sits:
   int disk = getdisk() + 'A';
   //Get location of .ini file
   getcurdir(0,currDir);

   sprintf(m_sProgLocation,"%c%s%s",disk,":\\",currDir);
   int len = strlen(m_sProgLocation);

   //m_sProgLocation[len-5] = '\0';


   //Initialise Menu flag for Graph form:
   m_bDisableGraphMenu = false;

   //Get settings from .ini file:
   GetIniFileSettings();


   Timer1->Enabled  = false;
   Timer1->Interval = m_iViewDelay;
   Timer2->Enabled  = false;
   Timer2->Interval = 10;


   //Indicate whether using legacy or Linux integrated code:
#ifdef ORI_WIN_CODE
   StatusBar1->Panels->Items[1]->Text = "LEGACY WINDOWS CODE RUNNING";
#else
   StatusBar1->Panels->Items[1]->Text = "LINUX INTEGRATED CODE RUNNING";
#endif


}
//---------------------------------------------------------------------------
void TEasyPlotForm::GetIniFileSettings()
{

   char name[10],tmp[10];
   char iniFile[200];

   sprintf(iniFile,"%s%s", m_sProgLocation,"\\Ini\\easyplotter.ini");

   m_iNumPlotTypes = 0;
   for(int i=0;i<MAX_NUM_PLOTTYPES;i++)
   {
      itoa(i+1,tmp,10);
      sprintf(name,"%s%s","type",tmp);

      GetPrivateProfileString("Plot Types",name,"",
                               m_sPLOTTYPES[i],sizeof(m_sPLOTTYPES[i]),
                              iniFile);

      if( strcmp(m_sPLOTTYPES[i],"") != 0 )
      {
         m_iNumPlotTypes++;
      }
      else
      {
         //No more types listed in .ini file - stop count
         break;
      }

   }//i
   

}//GetIniFileSettings


void TEasyPlotForm::parseDefFile()
{


   char contents[9000];
   char ch[2];


   if( (m_fpDefFile=fopen(m_sDefFile,"rt")) == NULL)
   {

#ifdef WIDE_CHAR_APP
	  String str = BEST_MESSAGE;
	  wchar_t mes[] = L"Cannot open required Animation Program File.\nPlease try again.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  return;
#else
      MessageBox(NULL,"Cannot open required Animation Program File.\nPlease try again.",
                 BEST_MESSAGE,MB_ICONEXCLAMATION);
	  return;
#endif

   }
   //Read contents of file:
   int byteCnt = 0;
   while( !feof(m_fpDefFile))
   {
	  fread(ch,sizeof(char),1,m_fpDefFile);
	  contents[byteCnt] = ch[0];
	  //contents++;
	 byteCnt++;
   }
   contents[byteCnt] = '\0';
   fclose(m_fpDefFile);


   //Search for <TAGFILES> section
   char *fixed_ptr1 = NULL;
   char *fixed_ptr2 = NULL;
   char *control_ptr1 = NULL;
   char *control_ptr2 = NULL;
   char fixed_str[1000];
   char control_str[1000];
   if( strstr(contents,"<TAGFILES>") != NULL)
   {
	  //Process <FIXED> animation instructions:
	  fixed_ptr1 = strstr(contents,"<TAGFILES>");
	  fixed_ptr2 = strstr(contents,"</TAGFILES>");
	  int len = strlen(fixed_ptr1) - strlen(fixed_ptr2);
	  strcpy(fixed_str,fixed_ptr1);
	  fixed_str[len-1] = '\0';


	  //Initialise taglist:

	   if(m_SLTagFiles == NULL)
	   {
			m_SLTagFiles = new TStringList();

	   }
	   m_iTagFileCount = 0;

	  //Retrieving the tag file list:
	  bool parsing = true;
	  char *ptr = fixed_ptr1;
	  int numFiles = 0;
	  char tagname[50];
	  int len1,intvar;
	  while(parsing)
	  {
		  char *p = strstr(ptr,"<NAME>");
		  if( p != NULL)
		  {
			 for(int i=0;i<6;i++)
			 {
				 p++;
			 }
			 m_iTagFileCount++;
			 //Grab the tag filename:
			 char *p1 = strstr(p,"</NAME>");
			 len1 = strlen(p) - strlen(p1);

			 for(int i=0;i<len1;i++)
			 {
				tagname[i] = p[0];
				p++;
			 }
			 tagname[len1] = '\0';

			 //Add the file path:
			 char file_path[100];
			 sprintf(file_path,"%s%s%s",m_sMovieTargetRoot,"\\",tagname);
			 String str1 = file_path;
			 wchar_t wtagname[100];
			 StringToWideChar(str1,wtagname,99);
			 m_SLTagFiles->Add(wtagname);

			 ptr = p;
		  }
		  else
		  {
			 parsing = false;
		  }
	  }

	  //Assign first tag file name:
	  char str[100];
#ifdef WIDE_CHAR_APP
	  wcstombs(str, m_SLTagFiles->Strings[0].c_str(), 99);
#else
      strcpy(str,m_SLTagFiles->Strings[0].c_str());
#endif
	  strcpy(m_sTagFile,str);

	   if(m_iTagFileCount > 1)
	   {
			m_bMultiTagFiles = true;
	   }

	  //Set up sub-dir:
	  setUpSubDir();

   }//if( strstr(contents,"<TAGFILES>") != NULL)
   else
   {
#ifdef WIDE_CHAR_APP
	  String str = BEST_MESSAGE;
 	  wchar_t mes[] = L"Cannot find a <TAGFILE> section.\nPlease try again.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  exit(1);
#else
     MessageBox(NULL,"Cannot find a <TAGFILE> section.\nPlease try again.",
                BEST_MESSAGE,MB_ICONEXCLAMATION);
	  exit(1);
#endif

   }

   //Get X,Y DATA RANGES
   //Uncheck auto axis box:
   CheckBox2->Checked = false;
   if( strstr(contents,"<DATA>") != NULL)
   {
	   char xmin[10],xmax[10],ymin[10],ymax[10];
	   int len1;

	   //Process <FIXED> animation instructions:
	  fixed_ptr1 = strstr(contents,"<DATA>");
	  fixed_ptr2 = strstr(contents,"</DATA>");
	  int len = strlen(fixed_ptr1) - strlen(fixed_ptr2);
	  strcpy(fixed_str,fixed_ptr1);
	  fixed_str[len-1] = '\0';



	  //Get XMin:
	  char *ptr = fixed_ptr1;

	  char *p = strstr(ptr,"<XMIN>");
	  for(int i=0;i<6;i++)
	  {
		 p++;
	  }
	  //Grab the value:
	  char *p1 = strstr(p,"</XMIN>");
	  len1 = strlen(p) - strlen(p1);

	  for(int i=0;i<len1;i++)
	  {
		 xmin[i] = p[0];
		 p++;
	  }
	  xmin[len1] = '\0';
	  //Set Edit box:
	  String str = xmin;
#ifdef WIDE_CHAR_APP
	  wchar_t buf[50];
	  StringToWideChar(str,buf,49);
	  Edit6->Enabled = true;
	  Edit6->SetTextBuf(buf);
#else
      char buf[50];
	  strcpy(buf,str.c_str());
	  Edit6->Enabled = true;
	  Edit6->SetTextBuf(buf);
#endif

	  ptr = p;

	  //Get XMax:
	  p = strstr(ptr,"<XMAX>");
	  for(int i=0;i<6;i++)
	  {
		 p++;
	  }
	  //Grab the value:
	  p1 = strstr(p,"</XMAX>");
	  len1 = strlen(p) - strlen(p1);

	  for(int i=0;i<len1;i++)
	  {
		 xmax[i] = p[0];
		 p++;
	  }
	  xmax[len1] = '\0';
	  //Set Edit box:
	  str = xmax;
#ifdef WIDE_CHAR_APP
	  StringToWideChar(str,buf,49);
	  Edit5->Enabled = true;
	  Edit5->SetTextBuf(buf);
#else
      strcpy(buf,str.c_str());
	  Edit5->Enabled = true;
	  Edit5->SetTextBuf(buf);
#endif


	  ptr = p;

	  //Get YMin:
	  p = strstr(ptr,"<YMIN>");
	  for(int i=0;i<6;i++)
	  {
		 p++;
	  }
	  //Grab the value:
	  p1 = strstr(p,"</YMIN>");
	  len1 = strlen(p) - strlen(p1);

	  for(int i=0;i<len1;i++)
	  {
		 ymin[i] = p[0];
		 p++;
	  }
	  ymin[len1] = '\0';
	  //Set Edit box:
	  str = ymin;
#ifdef WIDE_CHAR_APP
	  StringToWideChar(str,buf,49);
	  Edit7->Enabled = true;
	  Edit7->SetTextBuf(buf);
#else
      strcpy(buf,str.c_str());
	  Edit7->Enabled = true;
	  Edit7->SetTextBuf(buf);
#endif

	  ptr = p;

	  //Get YMax:
	  p = strstr(ptr,"<YMAX>");
	  for(int i=0;i<6;i++)
	  {
		 p++;
	  }
	  //Grab the value:
	  p1 = strstr(p,"</YMAX>");
	  len1 = strlen(p) - strlen(p1);

	  for(int i=0;i<len1;i++)
	  {
		 ymax[i] = p[0];
		 p++;
	  }
	  ymax[len1] = '\0';
	  //Set Edit box:
	  str = ymax;
#ifdef WIDE_CHAR_APP
	  StringToWideChar(str,buf,49);
	  Edit8->Enabled = true;
	  Edit8->SetTextBuf(buf);
#else
      strcpy(buf,str.c_str());
	  Edit8->Enabled = true;
	  Edit8->SetTextBuf(buf);
#endif

	  ptr = p;


   }
   else
   {
#ifdef WIDE_CHAR_APP
	   String str = BEST_MESSAGE;
	  wchar_t mes[] = L"Cannot find a <DATA> section.\nPlease try again.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  exit(1);
#else
      MessageBox(NULL,"Cannot find a <DATA> section.\nPlease try again.",
                 BEST_MESSAGE,MB_ICONEXCLAMATION);
	  exit(1);
#endif
   }

   //Test for Program Control requirements:
   if( strstr(contents,"<CONTROL>") != NULL)
   {
	  m_bProgrammedControl = true;

	   //Process <FIXED> animation instructions:
	  control_ptr1 = strstr(contents,"<CONTROL>");
	  control_ptr2 = strstr(contents,"</CONTROL>");
	  int len = strlen(control_ptr1) - strlen(control_ptr2);
	  strcpy(control_str,control_ptr1);
	  control_str[len-1] = '\0';


	  //Initialise Control Index list:

	   if(m_SLContIndex != NULL)
	   {
		   //Erase previous listing
		   delete m_SLContIndex;
		   m_SLContIndex = NULL;
	   }
	   if(m_SLContIndex == NULL)
	   {
			m_SLContIndex = new TStringList();

	   }
	   m_iContCnt = 0;

	  //Retrieving the control index list:
	  bool parsing = true;
	  char *ptr = control_ptr1;
	  char index[10];
	  int len1,intvar;
	  while(parsing)
	  {
		  char *p = strstr(ptr,"<INDEX>");
		  if( p != NULL)
		  {
			 for(int i=0;i<7;i++)
			 {
				 p++;
			 }
			 m_iContCnt++;
			 //Grab the control frame index:
			 char *p1 = strstr(p,"</INDEX>");
			 len1 = strlen(p) - strlen(p1);

			 for(int i=0;i<len1;i++)
			 {
				index[i] = p[0];
				p++;
			 }
			 index[len1] = '\0';

			 //Add the index to string list:
			 String str1 = index;
			 wchar_t windex[10];
			 StringToWideChar(str1,windex,9);
			 m_SLContIndex->Add(windex);

			 ptr = p;
		  }
		  else
		  {
			 parsing = false;
		  }
	  }

   }
   else
   {
	  //Set Program Pause COntrol flag:
	  m_bProgrammedControl = false;

   }
   

}//parseDefFile



void __fastcall TEasyPlotForm::PlotBtnClick(TObject *Sender)
{


      /*
      //Set status bar:
      if(!m_bAnimation)
      {
         StatusBar1->Panels->Items[0]->Text = "Plot mode";
         StatusBar1->Panels->Items[1]->Text = "";
      }
      else
      {
        StatusBar1->Panels->Items[0]->Text = "Animation mode";
      }



     //Draw required data plot:
     int intvar;
      char title[100], tmp[100];
      //int xPlotStartVal;
      float xPlotStartVal;
      float xRange,yRange,yMin,xMin;
      //Define data plot file name:
      intvar = Edit1->GetTextLen();
      Edit1->GetTextBuf(tmp,intvar+1);
      strcpy(m_sDataFile,tmp);


      //Check to see if a wave file:
      int len = strlen(m_sDataFile);
      if( ( (m_sDataFile[len-1] == 'v') && (m_sDataFile[len-2] == 'a') &&
          (m_sDataFile[len-3] == 'w') )  )
      {
           m_bIsWaveFile = true;
      }
      else
      {
          m_bIsWaveFile = false;
      }


      //If wave file, use special plot and graphic routine:
      if(m_bIsWaveFile)
      {
         //Plot wave file here
         plotWaveFile();
         return;

      }
      */

      //-----------------------------------------------------
      //Set status bar:
	  if(!m_bAnimation)
	  {
		 StatusBar1->Panels->Items[0]->Text = "Plot mode";
		 StatusBar1->Panels->Items[1]->Text = "";
	  }
	  else
	  {
		StatusBar1->Panels->Items[0]->Text = "Animation mode";
	  }



	 //Draw required data plot:
	 int intvar;
	 char title[100], tmp[100];
	 wchar_t wtmp[100], wtitle[100];


	  //int xPlotStartVal;
	  float xPlotStartVal;
	  float xRange,yRange,yMin,xMin;
	  //Define data plot file name:
	  intvar = Edit1->GetTextLen();
 #ifdef WIDE_CHAR_APP
	  Edit1->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 99);
	  strcpy(m_sDataFile,tmp);
 #else
      Edit1->GetTextBuf(tmp,intvar+1);
	  strcpy(m_sDataFile,tmp);
 #endif


	  //Check to see if a wave file:
	  int len = strlen(m_sDataFile);
	  if( ( (m_sDataFile[len-1] == 'v') && (m_sDataFile[len-2] == 'a') &&
		  (m_sDataFile[len-3] == 'w') )  )
	  {
		   m_bIsWaveFile = true;
	  }
	  else
	  {
		  m_bIsWaveFile = false;
	  }


	  //If wave file, use special plot and graphic routine:
	  if(m_bIsWaveFile)
	  {
		 //Plot wave file here
		 plotWaveFile();
		 return;

	  }



#ifdef ORI_WIN_CODE

      //If not a .wav file, check to see that file extension is .dat:
	  len = strlen(m_sDataFile);
	  if( ( (m_sDataFile[len-1] == 't') && (m_sDataFile[len-2] == 'a') &&
		  (m_sDataFile[len-3] == 'd') )  )
	  {

	  }
	  else
	  {
		  if(!m_bAnimation)
		  {
			 //char mes[100];
			 //sprintf(mes,"%s%s","The file selected is not a .WAV or .DAT type.\n",
			 //                 "Please select again.");


			 String str = "BEST_MESSAGE";
			 wchar_t mes[] = L"The file selected is not a .WAV or .DAT type.\nPlease select again.";
			 wchar_t cap[100];
			 StringToWideChar(str,cap,99);



			 MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
			 return;
		  }
		  else
		  {

			if(!m_bFileIgnore)
			{
				m_bFileIgnore = true;
				m_iTotAniCnt = 1;


			   //Timer1->Enabled = false;
			   //m_iTotAniCnt = 1;
			   //char mes[200];
				//sprintf(mes,"%s%s%s%s%s%s","You have chosen to run an animation program file\n",
				//		"without selecting a source data file.\n",
				//		"Choose YES if you are sure your program file does not require\n",
				//		"a source data file.\n",
				//		"Choose NO if you need the source data file,\n",
				//		" and to terminate the procedure.");


			   String str = "You have chosen to run an animation program file\n";
			   String str1 = "without selecting a source data file.\n";
			   String str2 = "Choose YES if you are sure your program file does not require\n";
			   String str3 = "a source data file.\n";
			   String str4 = "Choose NO if you need the source data file,\n";
			   String str5 = " and to terminate the procedure.";
			   String mes_str = str + str1 + str2 + str3 + str4 + str5;

			   String capstr = "BEST_MESSAGE";
			   wchar_t mes[300];
			   wchar_t cap[100];
			   StringToWideChar(capstr,cap,99);
			   StringToWideChar(mes_str,mes,299);

				/*
				int rtnCode;
				rtnCode = MessageBox(NULL,mes,cap,MB_YESNO|MB_ICONEXCLAMATION);
				if(rtnCode == IDNO)
				{
				  return;
				}
				else if(rtnCode == IDYES)
				{
				  m_bFileIgnore = true;
				  Timer1->Enabled = true;
				}
				*/
			}//if(!m_bFileIgnore)

		  }
	  }


	  //Get XData Set flag:
	  if(CheckBox3->Checked == true)
	  {
		 m_bXDataFileSet = true;
	  }
	  else
	  {
		 m_bXDataFileSet = false;
	  }


	  //Get x-y ranges from file if Auto Scale selected:
	  //If Auto Scale option on - obtain x-y ranges for plotting:
	  if(CheckBox2->Checked)
	  {
			//Get ranges for plotting:
			if(!m_bFileIgnore)
			{
				  float ymax = 0.0;
				  float ymin = 0.0;
				  //Get x-axis range: (assume x range in ascending order)
				  m_fmData.load(m_sDataFile);
				  m_fmData.SetPtrBegin();
				  if(m_bXDataFileSet)
				  {
					 xMin = *m_fmData.curPtr();
					 xRange = *m_fmData(m_fmData.rows()-1,0);
				  }
				  else
				  {
					 xMin = 0.0;    //Start incremental value of x:
					 xRange = m_fmData.rows()-1;
				  }

				  //Get max value for y value sets:
				  m_fmData.SetPtrBegin();
				  bool firstVal = false;
				  for(int i=0;i<m_fmData.rows();i++)
				  {

					  //Move to first element of current y data set:
					  if(m_bXDataFileSet) m_fmData++;
					  if(firstVal == false)
					  {
						ymin = *m_fmData.curPtr();
						ymax = *m_fmData.curPtr();
						firstVal = true;
					  }
					  if(m_bXDataFileSet)
					  {
						 for(int j=0;j<m_fmData.cols()-1;j++)
						 {
						   if( *m_fmData.curPtr() > ymax)
						   {
							  ymax = *m_fmData.curPtr();
						   }
						   if( *m_fmData.curPtr() < ymin)
						   {
							  ymin = *m_fmData.curPtr();
						   }
						   m_fmData++;
						 }
					  }
					  else
					  {
						 for(int j=0;j<m_fmData.cols();j++)
						 {
						   if( *m_fmData.curPtr() > ymax)
						   {
							  ymax = *m_fmData.curPtr();
						   }
						   if( *m_fmData.curPtr() < ymin)
						   {
							  ymin = *m_fmData.curPtr();
						   }
						   m_fmData++;
						 }
					  }

				  }//i

				  //Handle for case when y value remains constant:
				  if(ymin == ymax)
				  {
					  ymin = 0.0;
				  }

				  yRange = (int)ceil(ymax); //Round up to nearest integer value
				  yMin = (int) floor(ymin);
				  if(xRange > 1 )
				  {
					 xRange = (int) ceil(xRange);
				  }
				  else
				  {


				  }


				  //Passing to Auto-scale memmory:
				  m_fAutoXMin =  xMin;
				  m_fAutoXMax =  xRange;
				  m_fAutoYMin =  yMin;
				  m_fAutoYMax =  yRange;


			}//if(!m_bFileIgnore)


			//Populate Edit boxes with results:
			String str = m_sAutoNumStr;
			wchar_t buf[100];
			StringToWideChar(str,buf,99);

			float2Str(m_fAutoXMax);
			//Edit5->SetTextBuf(m_sAutoNumStr);
			Edit5->SetTextBuf(buf);
			float2Str(m_fAutoXMin);
			//Edit6->SetTextBuf(m_sAutoNumStr);
			Edit6->SetTextBuf(buf);
			float2Str(m_fAutoYMin);
			//Edit7->SetTextBuf(m_sAutoNumStr);
			Edit7->SetTextBuf(buf);
			float2Str(m_fAutoYMax);
			//Edit8->SetTextBuf(m_sAutoNumStr);
			Edit8->SetTextBuf(buf);


	  }//if(CheckBox2->Checked)



	  //Define graph title:
	  intvar = Edit2->GetTextLen();
	  Edit2->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 99);
	  strcpy(title,tmp);


	  //x-axis label:
	  intvar = Edit3->GetTextLen();
	  Edit3->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 99);
	  strcpy(m_sXLabel,tmp);

	  //y-axis label:
	  intvar = Edit4->GetTextLen();
	  Edit4->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 99);
	  strcpy(m_sYLabel,tmp);


	  if(CheckBox2->Checked == false)  //Suppress for auto-scale
	  {
		 //Define range for plotting:
		 intvar = Edit8->GetTextLen();
		 Edit8->GetTextBuf(wtmp,intvar+1);
		 wcstombs(tmp, wtmp, 99);
		 yRange = atof(tmp);

		 intvar = Edit7->GetTextLen();
		 Edit7->GetTextBuf(wtmp,intvar+1);
		 wcstombs(tmp, wtmp, 99);
		 yMin = atof(tmp);

		 intvar = Edit6->GetTextLen();
		 Edit6->GetTextBuf(wtmp,intvar+1);
		 wcstombs(tmp, wtmp, 99);
		 xMin = atof(tmp);


		 intvar = Edit5->GetTextLen();
		 Edit5->GetTextBuf(wtmp,intvar+1);
		 wcstombs(tmp, wtmp, 99);
		 xRange = atof(tmp);
	  }


	  //Define plot mode:
	  if(RadioGroup1->ItemIndex==0)
	  {
		 m_iLogPlotMode =0;
	  }
	  else if(RadioGroup1->ItemIndex==1)
	  {
		 m_iLogPlotMode =1;
	  }
	  else if(RadioGroup1->ItemIndex==2)
	  {
		 m_iLogPlotMode =2;
	  }
	  else if(RadioGroup1->ItemIndex==3)
	  {
		 m_iLogPlotMode =3;
	  }

	  if(m_iLogPlotMode==1)  //Log plot on x axis:
	  {
		 //Log plot
		 xPlotStartVal = xMin;
		 if(xMin==0)
			xPlotStartVal = 1.0;

		 if(xRange==0)
			xRange = 1.0;

	  }
	  if(m_iLogPlotMode==2) //Log plot on y axis:
	  {
		 //Log plot
		 if(yMin==0)
			yMin = 1.0;

		 if(yRange==0)
			yRange = 1.0;

		xPlotStartVal = xMin;

	  }
	  if(m_iLogPlotMode==3) //Log plot on x and y axis:
	  {
		 xPlotStartVal = xMin;
		 if(xMin==0)
			xPlotStartVal = 1.0;

		 if(xRange==0)
			xRange = 1.0;

		 if(yMin==0)
			yMin = 1.0;

		 if(yRange==0)
			yRange = 1.0;

	  }
	  else if(m_iLogPlotMode==0)
	  {
		 //xPlotStartVal = 0;
		 xPlotStartVal = xMin;

	  }

	  //Get required form dimensions for graph plots:
	  int reqFormWidth = m_iWidth;
	  int reqFormHeight = m_iHeight;

	  TBorderStyle bStyle;
	  if(m_bDisableGraphMenu)
	  {
		   //Removes top border area
		   bStyle = bsNone;

		  if(!m_bAnimation)
		  {
		   //Inform user that the DELETE button needs to be pressed to close the form;
		   String str = BEST_MESSAGE;
		   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
		   wchar_t cap[100];
		   StringToWideChar(str,cap,99);
		   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
		  }

	  }
	  else
	  {
		 bStyle = bsSizeable;
	  }

	  TGraphForm *graphFrm = new TGraphForm(this,true,m_iPlotMode[0],
											m_iLogPlotMode, m_sXLabel, m_sYLabel, title,
											yMin,yRange, xPlotStartVal, xRange,
											10, 10, m_bXGrad,m_bYGrad,
											"","","","",
											0.0,0.0,0.0,0.0,false,0,
											m_bXAxisShow,m_bYAxisShow,false,m_cBackground,
											m_sDataFile, "",
											1,0,
											16,
											//reqFormWidth,reqFormHeight,bsSizeable,m_bDrawBorder);
											reqFormWidth,reqFormHeight,bStyle,m_bDrawBorder,
											m_bDisableGraphMenu);

	  graphFrm->circleSize(m_iDataPlotSize);
	  m_GraphFrm = graphFrm;

	  //Disallow resize of form:
	  graphFrm->AllowResize(true);

	  //Set width if bar graph required:
	  if(m_iPlotMode[0] == 4) graphFrm->setBarWidth(m_iBarWidth);

	  //Define grid color:
	  graphFrm->setGridLines(m_cGridlines);
	  graphFrm->setDataPlotColor(m_cGridlines);
	  graphFrm->setPenColor();
	  graphFrm->graduations();
	  //Redraw origin in required pen color:
	  graphFrm->setOrigin(m_cOrigin);
	  graphFrm->setDataPlotColor(m_cOrigin);
	  graphFrm->setPenColor();
	  graphFrm->drawOriginLine();
	  graphFrm->setXDataFileSetFlg(m_bXDataFileSet);
	  graphFrm->setSkipSetFlags(m_bSkipSet[0],m_bSkipSet[1],m_bSkipSet[2]);
	  graphFrm->setDataPlotModes(m_iPlotMode[0],m_iPlotMode[1],m_iPlotMode[2]);


	  if(!m_bFileIgnore)
	  {
		  m_fmData.load(m_sDataFile);

		 //Get number of data sets:
		 if(m_bXDataFileSet)
		 {
			m_iNumDataSets = m_fmData.cols() - 1;
		 }
		 else
		 {
			m_iNumDataSets = m_fmData.cols();
		 }
	  }//if(!m_bFileIgnore)

	  //Convert to char
	  wcstombs(m_sLegendText1, m_swLegendText1, 99);
	  wcstombs(m_sLegendText2, m_swLegendText2, 99);
	  wcstombs(m_sLegendText3, m_swLegendText3, 99);

	  graphFrm->setLegend(m_bLegendOn);
	  graphFrm->setNumDataSets(m_iNumDataSets);
	  graphFrm->setLegendPosition(m_iLeftPos,m_iTopPos);
	  graphFrm->setDataSetText(m_sLegendText1,m_sLegendText2,m_sLegendText3);
	  graphFrm->setDataSetColors(m_cDataSet[0],m_cDataSet[1],m_cDataSet[2]);
	  graphFrm->setLegendBackColor(m_cLegend);


	  if(m_bLegendOn)
	  {
		 graphFrm->legend(m_iLeftPos,m_iTopPos,m_iNumDataSets,
						  m_sLegendText1, m_sLegendText2,m_sLegendText3,
						  m_cDataSet[0],m_cDataSet[1],m_cDataSet[2],m_cLegend,
						  m_iPlotMode[0],m_iPlotMode[1],m_iPlotMode[2]);
	  }
	  if(!m_bAnimation)
	  {
		 graphFrm->Show();
	  }
	  else
	  {
		 if(m_bViewStill)
		 {
		   graphFrm->Show();
		 }

	  }


	  if(!m_bAnimation)
	  {
		 plotData(graphFrm);

	  }//if(!m_bAnimation)
	  else      //Perform animation requirements:
	  {

		 //Timer1->Enabled = false;
		 m_bProcFIXED = true;
		 m_bProcVAR = false;
		 //Perform <FIXED> operations
		 for(int m_iFIXEDIndex=0;m_iFIXEDIndex<m_iNumFIXEDPlots;m_iFIXEDIndex++)
		 {
			doPlotType(graphFrm,m_sFIXED[m_iFIXEDIndex],m_iFIXEDIndex);
		 }
		 m_bProcFIXED = false;
		 m_bProcVAR = true;

		 /* OLD CODE
		 //Perform <VAR> operations:
		 if(m_bVARPresent)
		 {
			doPlotType(graphFrm,m_sVARSingle,0);
		 }
		 */
		 // REPLACED BY
		 //Perform <VAR> operations:
		 if(m_bVARPresent)
		 {
			for(int m_iVARIndex=0;m_iVARIndex<m_iNumVARPlots;m_iVARIndex++)
			{
				doPlotType(graphFrm,m_sVAR[m_iVARIndex],m_iVARIndex);
			}
		 }
		 //Timer1->Enabled = true;

	  }//else
#else

// --------------------------------------------------------------------
// LINUX INTEGRATION CODE
// --------------------------------------------------------------------



    // Read in the data file (s):
	readData ();

	// Plot the graph
	doGraph();


#endif

}

#if !defined(ORI_WIN_CODE)
// ------------------------------------------------------------------------
// Define Linux integrated functions here
// ------------------------------------------------------------------------
int TEasyPlotForm::readData()
{

    int retVal = 0;


	printf("Filename: %s\n",filename);
	if( (fp=fopen(filename,"rt")) != NULL )
	{

			int i = 0;
			int ret;
			while(1)
			{
#if NUM_PLOTS == 1
				ret = fscanf(fp,"%f %f\n",&data1[i],&data2[i]);
#endif
#if NUM_PLOTS == 2
				ret = fscanf(fp,"%f %f %f\n",&data1[i],&data2[i],&data3[i]);
#endif
#if NUM_PLOTS == 3
				ret = fscanf(fp,"%f %f %f %f\n",&data1[i],&data2[i],&data3[i],
				              &data4[i]);
#endif
#if NUM_PLOTS == 4
				ret = fscanf(fp,"%f %f %f %f\n",&data1[i],&data2[i],&data3[i],
				              &data4[i],&data5[i]);
#endif

				//printf("%f\n",data1[i]);
                if(ret < 0 )
				{
					break;
				}
				i++;

			}
			fclose(fp);
			NumDataSets = i;


	}
	else
	{
			//printf("Error opening file for writing data!\n");
            MessageBox(NULL,"Error opening file for writing data!","",0);
			retVal = -1;

	}


       /*
       char mes [200];
       sprintf (mes, "Number of datasets: %d\n",NumDataSets);
       MessageBox(NULL,mes,"",0);
       */


	  return retVal;

} // readData


void TEasyPlotForm::doGraph()
{
     // Instantiate Graphics form:
     m_bDisableGraphMenu = false;
     TGraphForm *graphFrm = new TGraphForm(this, X_WIN_SIZE, Y_WIN_SIZE,
                                      PLOT_MODE, LOG_MODE,
                                      X_LABEL, Y_LABEL, GRAPH_TITLE,
                                      Y_MIN,Y_MAX, X_MIN, X_MAX,
                                      X_INT, Y_INT, X_GRAD, Y_GRAD, X_LABEL_PREC, Y_LABEL_PREC,
                                      SHOW_GRID, G_INT,
                                      X_SHOW, Y_SHOW, ORI_LINE, ORI_LINE_COLOR, BITMAP,
                                      BCK_COLOR, AUDIOFILE, INI_AUDIOFILE,
                                      NUM_CHANS, SAMPLE_RATE, BITS_PER_SAMPLE, bsSizeable,
                                      m_bDrawBorder, m_bDisableGraphMenu);


     //Show graph form:
   //graphFrm->Caption= "UtterSense";
   graphFrm->Show();

   if (DRAW_VER_MARKER)
   {
	 	graphFrm->drawVerticalMarker (V_MARKER_POS,V_MARKER_COLOR,MARKER_WIDTH);
   }

   //Setup Legend:
   if(legend_flg)
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

      graphFrm->legendCaptions1(0,LEGEND_BCK_COLOR,NUM_PLOTS,info,LEGEND_LEFT,LEGEND_TOP,
                                LEGEND_WIDTH,LEGEND_HEIGHT);

   }

   // Define size of data plots:
   graphFrm->circleSize(SYMBOL_SIZE);

   		//Plot the test data:
#if NUM_PLOTS == 1
		if (!PLOT1_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT1_MODE);
			graphFrm->lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data2[i] >= Y_MIN) && (data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(data2[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data2[i],data1[i],PLOT1_COLOR);
					}

				}
			}
		}
#endif
#if NUM_PLOTS == 2
      if (!PLOT1_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT1_MODE);
			graphFrm->lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data2[i] >= Y_MIN) && (data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(data2[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data2[i],data1[i],PLOT1_COLOR);
					}
				}
			}
		}
		if (!PLOT2_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT2_MODE);
			graphFrm->lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data3[i] >= Y_MIN) && (data3[i] <= Y_MAX) )   )
				{
					if (!_isnan(data3[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data3[i],data1[i],PLOT2_COLOR);
					}
				}
			}
		}
#endif
#if NUM_PLOTS == 3
		if (!PLOT1_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT1_MODE);
			graphFrm->lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data2[i] >= Y_MIN) && (data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(data2[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data2[i],data1[i],PLOT1_COLOR);
					}
				}
			}
		}

		if (!PLOT2_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT2_MODE);
			graphFrm->lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data3[i] >= Y_MIN) && (data3[i] <= Y_MAX) )   )
				{

					if (!_isnan(data3[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data3[i],data1[i],PLOT2_COLOR);
					}
				}
			}
		}

		if (!PLOT3_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT3_MODE);
			graphFrm->lineWidth ( PLOT3_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data4[i] >= Y_MIN) && (data4[i] <= Y_MAX) )   )
				{
					if (!_isnan(data4[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data4[i],data1[i],PLOT3_COLOR);
					}
				}
			}
		}

#endif
#if NUM_PLOTS == 4
		if (!PLOT1_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT1_MODE);
			graphFrm->lineWidth ( PLOT1_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data2[i] >= Y_MIN) && (data2[i] <= Y_MAX) )   )
				{
					if (!_isnan(data2[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data2[i],data1[i],PLOT1_COLOR);
					}
				}
			}
		}

		if (!PLOT2_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT2_MODE);
			graphFrm->lineWidth ( PLOT2_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data3[i] >= Y_MIN) && (data3[i] <= Y_MAX) )   )
				{
					if (!_isnan(data3[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data3[i],data1[i],PLOT2_COLOR);
					}
				}
			}
		}

		if (!PLOT3_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT3_MODE);
			graphFrm->lineWidth ( PLOT3_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data4[i] >= Y_MIN) && (data4[i] <= Y_MAX) )   )
				{
					if (!_isnan(data4[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data4[i],data1[i],PLOT3_COLOR);
					}
				}
			}
		}

		if (!PLOT4_SUPPRESS)
		{
			graphFrm->changePlotMode(PLOT4_MODE);
			graphFrm->lineWidth ( PLOT4_LINE_WIDTH ); //for line plots
			for(int i= 0;i <NumDataSets;i++)
			{
				if ( ((data1[i] >= X_MIN) && (data1[i] <= X_MAX) ) &&
				    ((data5[i] >= Y_MIN) && (data5[i] <= Y_MAX) )   )
				{
					if (!_isnan(data5[i])) // Do not plot NAN numbers
					{
						graphFrm->plotSig(data5[i],data1[i],PLOT4_COLOR);
					}
				}
			}
		}
#endif


} // doGraph

#endif



//---------------------------------------------------------------------------
void TEasyPlotForm::doPlotType(TGraphForm *graphFrm,char *plotType,int index)
{
   
  //--------------------------------------------------------------
	if( strcmp(plotType,"DPLOT") == NULL)
   {
	  if(m_iLogPlotMode==0)        //Linear plot
	  {
		 plotData(graphFrm);

	  }
	  else if(m_iLogPlotMode==1)
	  {
		 plotData(graphFrm);

	  }

	  return;
   }//if (DPLOT)
   //----------------------------------------------------------------
   //PROCESSING STARTS HERE FOR SPECIALISED SHAPE CLASSES:
	int sign;

	//Set up test string array:
	std::vector<std::string> arglist; //NB: STL members vector template and string are in std namespace scope
	//arglist.reserve(m_iNumArgs);
	arglist.reserve(MAX_ARGS);
	int PlotProc; //(0: FIXED; 1: VAR )
	if(m_bProcVAR)
	{
	   /* OLD CODE --------------------------------------------------------
	   //Add the required vector strings:
	   for(int i=0;i<m_iNumArgs;i++) //Picks up last defined m_iNumArgs for VAR
	   //for(int i=0;i<MAX_ARGS;i++)
	   {
		  arglist.push_back(m_sVARSingleParams[i]);
	   }
	   PlotProc = 1;
	   --------------------------------------------------------------------*/
	   //REPLACED BY
	   for(int i=0;i<m_iNumVARPlotArgs[index];i++)
	   {
		  arglist.push_back(m_sVARParams[index][i]);
	   }
	   PlotProc = 1;

	}
	if(m_bProcFIXED)
	{
	   //Add the required vector strings:
	   //for(int i=0;i<m_iNumArgs;i++)
	   for(int i=0;i<m_iNumFIXEDPlotArgs[index];i++)
	   {
		  arglist.push_back(m_sFIXEDParams[index][i]);
	   }
	   PlotProc = 0;
	}


   if( (strcmp(plotType,"HLINE") == NULL) ||
			(strcmp(plotType,"VLINE") == NULL)    )
   {

	   A_Line *line = new A_Line(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	   m_pShape = line;
	   m_iTotAniCnt = m_pShape->movieStillCnt();
	   if( strcmp(plotType,"HLINE") == NULL )
	   {
			m_pShape->left2right();

	   }
	   if( strcmp(plotType,"VLINE") == NULL )
	   {
			m_pShape->upDown();
	   }


  }//(VLINE/HLINE)
  else if( (strcmp(plotType,"TEXTud") == NULL) ||
		   (strcmp(plotType,"TEXTlr") == NULL)  ||
		   (strcmp(plotType,"TEXTxy") == NULL)  )
  {


	   A_Text *text = new A_Text(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	   m_pShape = text;
	   m_iTotAniCnt = m_pShape->movieStillCnt();
	   if( strcmp(plotType,"TEXTlr") == NULL )
	   {
			m_pShape->left2right();
	   }
	   if( strcmp(plotType,"TEXTud") == NULL )
	   {
			m_pShape->upDown();
	   }
	   if( strcmp(plotType,"TEXTxy") == NULL )
	   {
			m_pShape->unconstrained();
	   }

  }//TEXT
  else if( (strcmp(plotType,"TEXTud_csize") == NULL) ||
		   (strcmp(plotType,"TEXTlr_csize") == NULL)    )
  {


	   A_Text *text = new A_Text(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	   m_pShape = text;
	   m_iTotAniCnt = m_pShape->movieStillCnt();
	   if( strcmp(plotType,"TEXTlr_csize") == NULL )
	   {
			m_pShape->leftRightChangeSize();
	   }
	   if( strcmp(plotType,"TEXTud_csize") == NULL )
	   {
			m_pShape->upDownChangeSize();
	   }

  }//TEXT
  else if( (strcmp(plotType,"BITMAPud") == NULL) ||
		   (strcmp(plotType,"BITMAPlr") == NULL) ||
		   (strcmp(plotType,"BITMAPxy") == NULL)   )
  {


	   A_BitMap *bmp = new A_BitMap(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	   m_pShape = bmp;
	   m_iTotAniCnt = m_pShape->movieStillCnt();
	   if( strcmp(plotType,"BITMAPlr") == NULL )
	   {
			m_pShape->left2right();
	   }
	   if( strcmp(plotType,"BITMAPud") == NULL )
	   {
			m_pShape->upDown();
	   }
	   if( strcmp(plotType,"BITMAPxy") == NULL )
	   {
			m_pShape->unconstrained();
	   }

  }//BITMAP
  else if( (strcmp(plotType,"CIRCLEud") == NULL) ||
		   (strcmp(plotType,"CIRCLElr") == NULL) ||
			(strcmp(plotType,"CIRCLExy") == NULL) )
  {

	   A_Circle *circle = new A_Circle(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	   m_pShape = circle;
	   m_iTotAniCnt = m_pShape->movieStillCnt();
	   if( strcmp(plotType,"CIRCLElr") == NULL )
	   {
			m_pShape->left2right();

	   }
	   if( strcmp(plotType,"CIRCLEud") == NULL )
	   {
			m_pShape->upDown();

	   }
	   if( strcmp(plotType,"CIRCLExy") == NULL )
	   {
			m_pShape->unconstrained();

	   }

  }//CIRCLE
  else if( ( strcmp(plotType,"dARROW") == NULL ) || ( strcmp(plotType,"uARROW")  == NULL )
		   || ( strcmp(plotType,"lARROW") == NULL ) || ( strcmp(plotType,"rARROW") == NULL ) )
  {


	  A_Arrow *arrow = new A_Arrow(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	  arrow->setArrowType(plotType);
	  m_pShape = arrow;
	  m_iTotAniCnt = m_pShape->movieStillCnt();

	  if( strcmp(plotType,"rARROW") == 0 )
	  {
		  m_pShape->left2right();
	  }
	  else if( strcmp(plotType,"lARROW") == 0 )
	  {
		  m_pShape->left2right();
	  }
	  else if( strcmp(plotType,"uARROW") == 0 )
	  {
		  m_pShape->upDown();
	  }
	  else if( strcmp(plotType,"dARROW") == 0 )
	  {
		  m_pShape->upDown();
	  }

  }//ARROW
  else if( strcmp(plotType,"BARPLOT") == NULL)
  {


	A_BarPlot *bar = new A_BarPlot(PlotProc,plotType,graphFrm, arglist,m_iAniCnt);
	m_pShape = bar;
	m_iTotAniCnt = m_pShape->movieStillCnt();
	m_pShape->upDown();


  }//if (BARPLOT)

  if(m_iTotAniCnt > m_iMaxTotAniCnt)     //Ensure we allow for range of plot type with largest range
  {
	  m_iMaxTotAniCnt = m_iTotAniCnt;
  }
  int mike = 4;


}



void TEasyPlotForm::plotData(TGraphForm *graphFrm)
{
    
	//Plot first data set:
	m_fmData.SetPtrBegin();

	int nData, sData;
	float fac = 10.0;

	for(int i=1;i<=m_iNumDataSets;i++)
	{
	     //Set  data plot mode type:
	     graphFrm->changePlotMode(m_iPlotMode[i-1]);

		 if(m_bSkipSet[i-1] == false)
		 {
		    //Set plot color:
		    graphFrm->setDataPlotColor(m_cDataSet[i-1]);
		    graphFrm->setPenColor();

			m_fmData.SetPtrBegin();
			float y_mag;

			if(m_bAnimation)
			{
			    if(m_bProcVAR)
				{
				   if( strstr(m_sVARSingleParams[0],"*") != NULL)
				   {  //Make movie stills over whole data range
				   	   m_iTotAniCnt = m_fmData.rows();
				   	   nData = m_iAniCnt;
				   	   sData = 0;
				   }
				   else
				   {
					    m_iTotAniCnt = atoi(m_sVARSingleParams[1]) - atoi(m_sVARSingleParams[0]) + 1;
					    sData = 0;
					    nData = m_iAniCnt;
						//Move data pointer to correct position
						for(int m1=0;m1<(atoi(m_sVARSingleParams[0])-1);m1++)
						{
							 for(int m2=0;m2<m_iNumDataSets+1;m2++) m_fmData++;
						}//m1

						}

					}//if(m_bProcVAR)
					else if(m_bProcFIXED)
					{
						if( strstr(m_sFIXEDParams[m_iFIXEDIndex][0],"*") != NULL)
						{  //Make plot over whole data range
						   nData = m_fmData.rows();
						   sData = 0;
						}
						else
						{
						   //Make plot over defined range:
						   sData = 0;
						   nData = atoi(m_sFIXEDParams[m_iFIXEDIndex][1]) - atoi(m_sFIXEDParams[m_iFIXEDIndex][0]) + 1;
						   //Move data pointer to correct position
						   for(int m1=0;m1<(atoi(m_sFIXEDParams[m_iFIXEDIndex][0])-1);m1++)
						   {
							 for(int m2=0;m2<m_iNumDataSets+1;m2++) m_fmData++;
						   }//m1

						}

					}///if(m_bProcFIXED

				 }
				 else
				 {
					 nData = m_fmData.rows();
					 sData = 0;
				 }

				 float x,y;
				 x = 0;
				 for(int j=sData;j<nData;j++)
				 //for(int j=0;j<m_fmData.rows();j++)
				 {

				   if(m_bXDataFileSet)
				   {
					   //float x,y;
					   x = *m_fmData.curPtr();
					   for(int jj=1;jj<=i;jj++) m_fmData++;
					   y = *m_fmData.curPtr();
					   if(y<0 )
					   {
						   y_mag = -1.0*y;
					   }
					   else
					   {
							  y_mag = y;
					   }
				   }//if(m_bXDataFileSet)
				   else
				   {
					   //for(int jj=1;jj<=i;jj++) m_fmData++;
					   for(int jj=1;jj<i;jj++) m_fmData++;
					   y = *m_fmData.curPtr();
					   if(y<0 )
					   {
					   	  y_mag = -1.0*y;
					   }
					   else
					   {
						 y_mag = y;
					   }

					}

					float xMax,xMin,yMax,yMin;
					if(m_iLogPlotMode == 0 )
					{
					    xMax = graphFrm->getxMax();
					    xMin = graphFrm->getxMin();
					    yMax = graphFrm->getyMax();
					    yMin = graphFrm->getyMin();
					}
					if(m_iLogPlotMode == 1 )
					{

					    xMax = pow(fac, graphFrm->getxMax());
					    xMin = pow(fac, graphFrm->getxMin());
					    yMax = graphFrm->getyMax();
					    yMin = graphFrm->getyMin();
					}
					if(m_iLogPlotMode == 2 )
					{

					    yMax = pow(fac, graphFrm->getyMax());
						yMin = pow(fac, graphFrm->getyMin());
						xMax = graphFrm->getxMax();
						xMin = graphFrm->getxMin();
					}
					if(m_iLogPlotMode == 3 )
					{

						yMax = pow(fac, graphFrm->getyMax());
						yMin = pow(fac, graphFrm->getyMin());
						xMax = pow(fac, graphFrm->getxMax());
						xMin = pow(fac, graphFrm->getxMin());

					}

					if( x <= xMax && x >= xMin      //Ensure plot within defined region
						&& y <= yMax && y >= yMin )
					{
					     graphFrm->plotSig(y,x);
					}

					if(!m_bXDataFileSet) x+=1;
					int numDataSetsLeft = m_iNumDataSets-i;
					for(int jj=1;jj<=numDataSetsLeft+1;jj++) m_fmData++;

				 }//j
		 }//m_bSkipSet[i-1]

		 //Re-initialise to accommodate new line plots:
		 graphFrm->setLinePlotFlag(0);
	}//i


}//plotData
//---------------------------------------------------------------------------
void TEasyPlotForm::plotWaveFile()
{

   FILE *fp;
   unsigned int numberOfSamples;
   short dataSample;

   //Instantiate graph for this wave file:
   if( (fp=fopen(m_sDataFile,"rb")) == NULL)
   {
#ifdef WIDE_CHAR_APP
	 String str = "BEST_MESSAGE";
	 wchar_t mes[] = L"Cannot open wave file.";
	 wchar_t cap[100];
	 StringToWideChar(str,cap,99);
	 MessageBox,(NULL,mes,cap,MB_ICONEXCLAMATION);
	 return;
#else
     MessageBox,(NULL,"Cannot open wave file.",BEST_MESSAGE,MB_ICONEXCLAMATION);
	 return;
#endif
   }//if


   //Instantiate graph.cpp and plot recording data:
   numberOfSamples = readWaveHeaderFile(fp);
   numberOfSamples = numberOfSamples/2;
   //N.B.: We are assuming 16-bit resolution here
   //      (2 bytes per sample);
   //numberOfSamples += 1;

   //Instantiate graph form:
   float yRange = 33000.0;  //(assuming 16-bit resolution)
   float xRange = (float)numberOfSamples;
   char title[100];
   wchar_t wtitle[100];
   sprintf(title,"%s%s","VOICE PRINT FOR WAVE FILE ",m_sDataFile);
   if(m_bTitleOverride)
   {
	  int intvar;
#ifdef WIDE_CHAR_APP
	  intvar = Edit2->GetTextLen();
	  Edit2->GetTextBuf(wtitle,intvar+1);
	  wcstombs(title, wtitle, 99);
#else
     char buf[100];
     intvar = Edit2->GetTextLen();
	 Edit2->GetTextBuf(buf,intvar+1);
	 strcpy(title, buf);
#endif

   }

   int reqFormWidth = m_iWidth;
   int reqFormHeight;
   reqFormHeight = m_iHeight;

   char filename[100];
#ifdef WIDE_CHAR_APP
   wcstombs(filename, (OpenDialog1->FileName).c_str(), 99);
#else
   strcpy(filename, (OpenDialog1->FileName).c_str());
#endif


   TBorderStyle bStyle;
   if(m_bDisableGraphMenu)
   {
	   //Removes top border area
	   bStyle = bsNone;
#ifdef WIDE_CHAR_APP
       //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
       MessageBox(NULL,"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button."
                  ,BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif

   }
   else
   {
	 bStyle = bsSizeable;
   }


#ifdef WIDE_CHAR_APP
       //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
      MessageBox(NULL,"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button."
                 ,BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif



#ifdef ORI_WIN_CODE
   TGraphForm *graphFrm = new TGraphForm(this,true,0,
                                         0, "", "", title,
                                         -yRange,yRange, 0.0, xRange,
                                         10, 10, false, false,
                                         "","","","",
                                         0.0,0.0,0.0,0.0,false,0,
                                         true,false,false,m_cBackground,
                                         (OpenDialog1->FileName).c_str(), "",
                                         m_iNumChannels,m_iPlaybackWaveFileSamplesPerSec,
                                         m_iBitsPerSample,
                                         reqFormWidth,reqFormHeight,bsSizeable,m_bDrawBorder,
										 m_bDisableGraphMenu);
#else
   m_bDisableGraphMenu = false;
   TGraphForm *graphFrm = new TGraphForm(this,reqFormWidth,reqFormHeight,0,
                                         0, "", "", title,
                                         -yRange,yRange, 0.0, xRange,
                                         10, 10, false, false,1,1,
                                         true,10,false,false,false,0,
                                         true, m_cBackground,
                                         (OpenDialog1->FileName).c_str(), "",
                                         m_iNumChannels,m_iPlaybackWaveFileSamplesPerSec,
                                         m_iBitsPerSample, bsSizeable,m_bDrawBorder,
										 m_bDisableGraphMenu);

#endif

   
   //Show graph form:
   graphFrm->circleSize(m_iDataPlotSize);
   graphFrm->Caption= "UtterSense";

   graphFrm->Show();

   //Set Audio marker color:
   graphFrm->setAudioMarker(m_cAudioMarker);

   //Plot data:
   //Define data plot color:
   graphFrm->setDataPlotColor(m_cSignal);
   graphFrm->setPenColor();

   float x = 0.0;

   while( !feof(fp) )
   {
	  fread(&dataSample,sizeof(short),1,fp);
	  graphFrm->plotSig( (float)dataSample,x);
	  x += 1;

   }//while

   fclose(fp);


   //Position graph form relative to recording studio control

   graphFrm->Left = Left;
   graphFrm->Top = Top;


   //Plot word end marker (if required):
   //if( strlen(tmp1) != 0 )
   //{
   //   for(float f=0;f<=yRange;f=f+0.001*yRange)
   //   {
   //      graphFrm->plotSig( f,endWordPos);

   //   }
   //   for(float f= -yRange;f<0;f=f+0.001*yRange)
   //   {
   //     graphFrm->plotSig( f,endWordPos);

   // }
   //}


  }//plotWaveFile


  void __fastcall TEasyPlotForm::OKBtnClick(TObject *Sender)
{
   LPCTSTR path = "Help\\easyplotter.hlp";
   WinHelp(Handle,path,HELP_QUIT,0);
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::settingsBtnClick(TObject *Sender)
{

     /*

   //Dialog box for settings:
   TSettingsDlg *settings = new TSettingsDlg(this);


   //Get details for current directory:
   char old_dir[MAXPATH],new_dir[MAXPATH];
   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);


   //Default position settings:
   char tmp[10];
   itoa(m_iLeftPos,tmp,10);
   settings->Edit1->Text = tmp;
   itoa(m_iTopPos,tmp,10);
   settings->Edit2->Text = tmp;

   //Set default data set colors:
   settings->ComboBox1->Text = setComboBoxDefaultColor(0);
   settings->ComboBox2->Text = setComboBoxDefaultColor(1);
   settings->ComboBox3->Text = setComboBoxDefaultColor(2);

   //Set default  main graph colors:
   settings->ComboBox5->Text = getColorText(m_cBackground);
   settings->ComboBox6->Text = getColorText(m_cGridlines);
   settings->ComboBox7->Text = getColorText(m_cOrigin);

   //Set legend background color:
   settings->ComboBox4->Text = setLegendComboBackgroundColor();
   //Set data exclusion checkbox:
   if(m_bSkipSet[0])
   {
      settings->CheckBox1->Checked = false;
   }
   else
   {
      settings->CheckBox1->Checked = true;
   }
   if(m_bSkipSet[1])
   {
      settings->CheckBox2->Checked = false;
   }
   else
   {
      settings->CheckBox2->Checked = true;
   }
   if(m_bSkipSet[2])
   {
      settings->CheckBox3->Checked = false;
   }
   else
   {
      settings->CheckBox3->Checked = true;
   }

   //Set legend flag:
   if(m_bLegendOn)
   {
      settings->CheckBox4->Checked = true;
   }
   else
   {
      settings->CheckBox4->Checked = false;
   }

   //Set legend default text:
   settings->Edit3->Text = m_sLegendText1;
   settings->Edit4->Text = m_sLegendText2;
   settings->Edit5->Text = m_sLegendText3;

   //Graph features:
   if(m_bXGrad)
   {
      settings->CheckBox6->Checked = true;
   }
   else
   {
      settings->CheckBox6->Checked = false;
   }
   if(m_bYGrad)
   {
      settings->CheckBox9->Checked = true;
   }
   else
   {
      settings->CheckBox9->Checked = false;
   }
   if(m_bXAxisShow)
   {
      settings->CheckBox7->Checked = true;
   }
   else
   {
      settings->CheckBox7->Checked = false;
   }
   if(m_bYAxisShow)
   {
      settings->CheckBox8->Checked = true;
   }
   else
   {
      settings->CheckBox8->Checked = false;
   }
   //Set plot mode type for data sets:
   settings->Label17->Enabled = false;
   settings->Edit9->Enabled = false;
   setRadioDataPlotModes(settings);
   if(settings->RadioGroup1->ItemIndex == 3)
   {
     settings->Label17->Enabled = true;
     settings->Edit9->Enabled = true;

   }
   itoa(m_iBarWidth,tmp,10);
   settings->Edit9->Text = tmp;


   //ANIMATION SETTINGS----------------------------------
   //Set data plot  delay:
   itoa(m_iViewDelay,tmp,10);
   settings->Edit6->Text = tmp;
   //Set movie tag file name:
    settings->Edit7->Text = m_sMovieTag;
    //Set target directory:
    settings->Edit8->Text = m_sMovieTargetRoot;
    //Set view movie still creation process flag
    if(m_bViewStill)
    {
       settings->CheckBox5->Checked = true;
    }
    else if(!m_bViewStill)
    {
       settings->CheckBox5->Checked = false;
    }

    itoa(m_iPreviewInt,tmp,10);
    settings->Edit10->Text = tmp;

   //--------------------------------------------------

   if(settings->ShowModal() == mrOk)
   {
      //Get position settings:
      char tmp[10];
      int intvar;
      intvar = settings->Edit1->GetTextLen();
      settings->Edit1->GetTextBuf(tmp,intvar+1);
       m_iLeftPos = atoi(tmp);
       intvar = settings->Edit2->GetTextLen();
       settings->Edit2->GetTextBuf(tmp,intvar+1);
       m_iTopPos = atoi(tmp);

       //Set data set colors:
      intvar = settings->ComboBox1->GetTextLen();
      settings->ComboBox1->GetTextBuf(tmp,intvar+1);
      setDataSetColors(tmp,0);
      intvar = settings->ComboBox2->GetTextLen();
      settings->ComboBox2->GetTextBuf(tmp,intvar+1);
      setDataSetColors(tmp,1);
      intvar = settings->ComboBox3->GetTextLen();
      settings->ComboBox3->GetTextBuf(tmp,intvar+1);
      setDataSetColors(tmp,2);

      //Set main graph colors:
      intvar = settings->ComboBox5->GetTextLen();
      settings->ComboBox5->GetTextBuf(tmp,intvar+1);
      m_cBackground = setColor(tmp);
      intvar = settings->ComboBox6->GetTextLen();
      settings->ComboBox6->GetTextBuf(tmp,intvar+1);
      m_cGridlines = setColor(tmp);
      intvar = settings->ComboBox7->GetTextLen();
      settings->ComboBox7->GetTextBuf(tmp,intvar+1);
      m_cOrigin = setColor(tmp);

      //Set legend background color:
      intvar = settings->ComboBox4->GetTextLen();
      settings->ComboBox4->GetTextBuf(tmp,intvar+1);
      setLegendBackgroundColor(tmp);

      //Check for Data Exclusion:
      if(!settings->CheckBox1->Checked)
      {
         m_bSkipSet[0] = true;
      }
      else
      {
         m_bSkipSet[0] = false;
      }
      if(!settings->CheckBox2->Checked)
      {
         m_bSkipSet[1] = true;
      }
      else
      {
         m_bSkipSet[1] = false;
      }
      if(!settings->CheckBox3->Checked)
      {
         m_bSkipSet[2] = true;
      }
      else
      {
         m_bSkipSet[2] = false;
      }

      //Set legend flag:
      if(settings->CheckBox4->Checked)
      {
         m_bLegendOn = true;
      }
      else
      {
         m_bLegendOn = false;
      }

      //Set legend Text:
      intvar = settings->Edit3->GetTextLen();
      settings->Edit3->GetTextBuf(m_sLegendText1,intvar+1);
      intvar = settings->Edit4->GetTextLen();
      settings->Edit4->GetTextBuf(m_sLegendText2,intvar+1);
      intvar = settings->Edit5->GetTextLen();
      settings->Edit5->GetTextBuf(m_sLegendText3,intvar+1);

      //Set data plot modes
      setDataPlotModes(settings->RadioGroup1->ItemIndex,
                       settings->RadioGroup2->ItemIndex,
                       settings->RadioGroup3->ItemIndex);

      if(settings->CheckBox6->Checked == true)
      {
         m_bXGrad = true;

      }
      else if(settings->CheckBox6->Checked == false)
      {
         m_bXGrad = false;

      }
      if(settings->CheckBox9->Checked == true)
      {
         m_bYGrad = true;

      }
      else if(settings->CheckBox9->Checked == false)
      {
         m_bYGrad = false;

      }
      if(settings->CheckBox7->Checked == true)
      {
         m_bXAxisShow = true;

      }
      else if(settings->CheckBox7->Checked == false)
      {
         m_bXAxisShow = false;

      }
      if(settings->CheckBox8->Checked == true)
      {
         m_bYAxisShow = true;

      }
      else if(settings->CheckBox8->Checked == false)
      {
         m_bYAxisShow = false;

      }

      //Save bar plot settings:
      intvar = settings->Edit9->GetTextLen();
      settings->Edit9->GetTextBuf(tmp,intvar+1);
      m_iBarWidth = atoi(tmp);

      //Get Animation Effects settings:
      intvar = settings->Edit6->GetTextLen();
      settings->Edit6->GetTextBuf(tmp,intvar+1);
      m_iViewDelay = atoi(tmp);
      Timer1->Interval = m_iViewDelay;
      intvar = settings->Edit7->GetTextLen();
      settings->Edit7->GetTextBuf(tmp,intvar+1);
      strcpy(m_sMovieTag,tmp);
      intvar = settings->Edit8->GetTextLen();
      settings->Edit8->GetTextBuf(m_sMovieTargetRoot,intvar+1);
      intvar = settings->Edit10->GetTextLen();
      settings->Edit10->GetTextBuf(tmp,intvar+1);
      m_iPreviewInt = atoi(tmp);


      //strcpy(m_sMovieTarget,settings->DirectoryListBox1->Directory.c_str() );
      if(settings->CheckBox5->Checked) m_bViewStill = true;
      if(!settings->CheckBox5->Checked) m_bViewStill = false;

      //Change back to original directory in case changed by target deirectory
      //selections:
      if(chdir(old_dir) == -1)
      {
         MessageBox(NULL,"Problem changing to old dir!","",0);
      }

   }
   else
   {
      //Change back to original directory in case changed by target deirectory
      //selections:
      if(chdir(old_dir) == -1)
      {
         MessageBox(NULL,"Problem changing to old dir!","",0);
      }

   }
   */


   //Dialog box for settings:
   TSettingsDlg *settings = new TSettingsDlg(this);


   //Get details for current directory:
   char old_dir[MAXPATH],new_dir[MAXPATH];
   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);


   //Default position settings:
   char tmp[10];
   itoa(m_iLeftPos,tmp,10);
   settings->Edit1->Text = tmp;
   itoa(m_iTopPos,tmp,10);
   settings->Edit2->Text = tmp;

   //Set default data set colors:
   settings->ComboBox1->Text = setComboBoxDefaultColor(0);
   settings->ComboBox2->Text = setComboBoxDefaultColor(1);
   settings->ComboBox3->Text = setComboBoxDefaultColor(2);

   //Set default  main graph colors:
   settings->ComboBox5->Text = getColorText(m_cBackground);
   settings->ComboBox6->Text = getColorText(m_cGridlines);
   settings->ComboBox7->Text = getColorText(m_cOrigin);

   //Set legend background color:
   settings->ComboBox4->Text = setLegendComboBackgroundColor();
   //Set data exclusion checkbox:
   if(m_bSkipSet[0])
   {
	  settings->CheckBox1->Checked = false;
   }
   else
   {
	  settings->CheckBox1->Checked = true;
   }
   if(m_bSkipSet[1])
   {
	  settings->CheckBox2->Checked = false;
   }
   else
   {
	  settings->CheckBox2->Checked = true;
   }
   if(m_bSkipSet[2])
   {
	  settings->CheckBox3->Checked = false;
   }
   else
   {
	  settings->CheckBox3->Checked = true;
   }

   //Set legend flag:
   if(m_bLegendOn)
   {
	  settings->CheckBox4->Checked = true;
   }
   else
   {
	  settings->CheckBox4->Checked = false;
   }

   //Set legend default text:
   settings->Edit3->Text = m_sLegendText1;
   settings->Edit4->Text = m_sLegendText2;
   settings->Edit5->Text = m_sLegendText3;

   //Graph features:
   if(m_bXGrad)
   {
	  settings->CheckBox6->Checked = true;
   }
   else
   {
	  settings->CheckBox6->Checked = false;
   }
   if(m_bYGrad)
   {
	  settings->CheckBox9->Checked = true;
   }
   else
   {
	  settings->CheckBox9->Checked = false;
   }
   if(m_bXAxisShow)
   {
	  settings->CheckBox7->Checked = true;
   }
   else
   {
	  settings->CheckBox7->Checked = false;
   }
   if(m_bYAxisShow)
   {
	  settings->CheckBox8->Checked = true;
   }
   else
   {
	  settings->CheckBox8->Checked = false;
   }

   //Set plot mode type for data sets:
   settings->Label17->Enabled = false;
   settings->Edit9->Enabled = false;
   setRadioDataPlotModes(settings);
   if(settings->RadioGroup1->ItemIndex == 3)
   {
	 settings->Label17->Enabled = true;
	 settings->Edit9->Enabled = true;

   }
   itoa(m_iBarWidth,tmp,10);
   settings->Edit9->Text = tmp;

   //Set Data Plot Size:
   itoa(m_iDataPlotSize,tmp,10);
   settings->Edit12->Text = tmp;

   //Set Graph height:
   itoa(m_iHeight,tmp,10);
   settings->Edit13->Text = tmp;

   //Set Graph width:
   itoa(m_iWidth,tmp,10);
   settings->Edit14->Text = tmp;


   //Set flag for graph border:
	if(m_bDrawBorder)
   {
	 settings->CheckBox12->Checked = true;

   }

   //Set flag for disabling the Graph Form Menu:
	if(m_bDisableGraphMenu)
   {
	 settings->CheckBox13->Checked = true;

   }
   else
   {
	 settings->CheckBox13->Checked = false;
   }

   //ANIMATION SETTINGS----------------------------------
   //Set data plot  delay:
   itoa(m_iViewDelay,tmp,10);
   settings->Edit6->Text = tmp;
   //Set movie tag file name:
	settings->Edit7->Text = m_sMovieTag;
	//Set target directory:
	settings->Edit8->Text = m_swMovieTargetRoot;
	//Set view movie still creation process flag
	if(m_bViewStill)
	{
	   settings->CheckBox5->Checked = true;
	}
	else if(!m_bViewStill)
	{
	   settings->CheckBox5->Checked = false;
	}

	//Set multi subdirectory ovveride flag:
	if(m_bOverride)
	{
		settings->CheckBox10->Checked = true;
	}
	else
	{
	   settings->CheckBox10->Checked = false;
	}

	//Set flag to enable movie preview pause control:
	if(m_bMovieControl)
	{
		settings->CheckBox11->Checked = true;
	}
	else
	{
	   settings->CheckBox11->Checked = false;
	}

	itoa(m_iPreviewInt,tmp,10);
	settings->Edit10->Text = tmp;
	itoa(m_iPreviewIndexStart,tmp,10);
	settings->Edit11->Text = tmp;

   //--------------------------------------------------

   if(settings->ShowModal() == mrOk)
   {

	  //Get position settings:
	  char tmp[10];
	  wchar_t wtmp[10];
	  int intvar;
	  intvar = settings->Edit1->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit1->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit1->GetTextBuf(tmp,intvar+1);

#endif
	  m_iLeftPos = atoi(tmp);
	  intvar = settings->Edit2->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit2->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit2->GetTextBuf(tmp,intvar+1);
#endif
	  m_iTopPos = atoi(tmp);

	   //Set data set colors:
	  intvar = settings->ComboBox1->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox1->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox1->GetTextBuf(tmp,intvar+1);
#endif
	  setDataSetColors(tmp,0);
	  intvar = settings->ComboBox2->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox2->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox2->GetTextBuf(tmp,intvar+1);
#endif
	  setDataSetColors(tmp,1);
	  intvar = settings->ComboBox3->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox3->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox3->GetTextBuf(tmp,intvar+1);
#endif
	  setDataSetColors(tmp,2);

	  //Set main graph colors:
	  intvar = settings->ComboBox5->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox5->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox5->GetTextBuf(tmp,intvar+1);
#endif
	  m_cBackground = setColor(tmp);
	  intvar = settings->ComboBox6->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox6->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox6->GetTextBuf(tmp,intvar+1);
#endif
	  m_cGridlines = setColor(tmp);
	  intvar = settings->ComboBox7->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox7->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox7->GetTextBuf(tmp,intvar+1);
#endif
	  m_cOrigin = setColor(tmp);

	  //Set legend background color:
	  intvar = settings->ComboBox4->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->ComboBox4->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->ComboBox4->GetTextBuf(tmp,intvar+1);
#endif
	  setLegendBackgroundColor(tmp);

	  //Check for Data Exclusion:
	  if(!settings->CheckBox1->Checked)
	  {
		 m_bSkipSet[0] = true;
	  }
	  else
	  {
		 m_bSkipSet[0] = false;
	  }
	  if(!settings->CheckBox2->Checked)
	  {
		 m_bSkipSet[1] = true;
	  }
	  else
	  {
		 m_bSkipSet[1] = false;
	  }
	  if(!settings->CheckBox3->Checked)
	  {
		 m_bSkipSet[2] = true;
	  }
	  else
	  {
		 m_bSkipSet[2] = false;
	  }

	  //Set legend flag:
	  if(settings->CheckBox4->Checked)
	  {
		 m_bLegendOn = true;
	  }
	  else
	  {
		 m_bLegendOn = false;
	  }

	  //Get required screen resolution:
	  intvar = settings->Edit13->GetTextLen();
#ifdef WIDE_CHAF_APP
	  settings->Edit13->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit13->GetTextBuf(tmp,intvar+1);
#endif
	  m_iHeight = atoi(tmp);

	  intvar = settings->Edit14->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit14->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit14->GetTextBuf(tmp,intvar+1);
#endif
	  m_iWidth = atoi(tmp);


	  //Set legend Text:
	  intvar = settings->Edit3->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit3->GetTextBuf(m_swLegendText1,intvar+1);
	  wcstombs(m_sLegendText1, m_swLegendText1, 99);
#else
      settings->Edit3->GetTextBuf(m_sLegendText1,intvar+1);
#endif
	  intvar = settings->Edit4->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit4->GetTextBuf(m_swLegendText2,intvar+1);
	  wcstombs(m_sLegendText2, m_swLegendText2, 99);
#else
      settings->Edit4->GetTextBuf(m_sLegendText2,intvar+1);
#endif
	  intvar = settings->Edit5->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit5->GetTextBuf(m_swLegendText3,intvar+1);
	  wcstombs(m_sLegendText3, m_swLegendText3, 99);
#else
      settings->Edit5->GetTextBuf(m_sLegendText3,intvar+1);
#endif

	  //Set data plot modes
	  setDataPlotModes(settings->RadioGroup1->ItemIndex,
					   settings->RadioGroup2->ItemIndex,
					   settings->RadioGroup3->ItemIndex);

	  if(settings->CheckBox6->Checked == true)
	  {
		 m_bXGrad = true;

	  }
	  else if(settings->CheckBox6->Checked == false)
	  {
		 m_bXGrad = false;

	  }
	  if(settings->CheckBox9->Checked == true)
	  {
		 m_bYGrad = true;

	  }
	  else if(settings->CheckBox9->Checked == false)
	  {
		 m_bYGrad = false;

	  }
	  if(settings->CheckBox7->Checked == true)
	  {
		 m_bXAxisShow = true;

	  }
	  else if(settings->CheckBox7->Checked == false)
	  {
		 m_bXAxisShow = false;

	  }
	  if(settings->CheckBox8->Checked == true)
	  {
		 m_bYAxisShow = true;

	  }
	  else if(settings->CheckBox8->Checked == false)
	  {
		 m_bYAxisShow = false;

	  }

	  //Save bar plot settings:
	  intvar = settings->Edit9->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit9->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit9->GetTextBuf(tmp,intvar+1);
#endif
	  m_iBarWidth = atoi(tmp);

	  //Get Data Plot Point size:
	  intvar = settings->Edit12->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit12->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit12->GetTextBuf(tmp,intvar+1);
#endif
	  m_iDataPlotSize = atoi(tmp);


	  //Get Animation Effects settings:
	  intvar = settings->Edit6->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit6->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit6->GetTextBuf(tmp,intvar+1);
#endif
	  m_iViewDelay = atoi(tmp);
	  Timer1->Interval = m_iViewDelay;
	  intvar = settings->Edit7->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit7->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit7->GetTextBuf(tmp,intvar+1);
#endif
	  strcpy(m_sMovieTag,tmp);
	  intvar = settings->Edit8->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit8->GetTextBuf(m_swMovieTargetRoot,intvar+1);
	  wcstombs(m_sMovieTargetRoot, m_swMovieTargetRoot, 199);
#else
      settings->Edit8->GetTextBuf(m_sMovieTargetRoot,intvar+1);
#endif
	  intvar = settings->Edit10->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit10->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit10->GetTextBuf(tmp,intvar+1);
#endif
	  m_iPreviewInt = atoi(tmp);
	  intvar = settings->Edit11->GetTextLen();
#ifdef WIDE_CHAR_APP
	  settings->Edit11->GetTextBuf(wtmp,intvar+1);
	  wcstombs(tmp, wtmp, 9);
#else
      settings->Edit11->GetTextBuf(tmp,intvar+1);
#endif
	  m_iPreviewIndexStart = atoi(tmp);

	  //Set flag for border draw
	  if(settings->CheckBox12->Checked)
	  {
		 m_bDrawBorder = true;
	  }
	  else
	  {
		 m_bDrawBorder = false;
	  }

	  //Set flag for disabling Graph Form Menu:
	  if(settings->CheckBox13->Checked)
	  {
		 m_bDisableGraphMenu = true;
	  }
	  else
	  {
		 m_bDisableGraphMenu = false;
	  }


	  //Set Movie Stills override flag for multi subdirectory creation:
	  if(settings->CheckBox10->Checked)
	  {
		 m_bOverride = true;
	  }
	  else
	  {
		 m_bOverride = false;
	  }

	  //Set Movie Preview Pause Control flag :
	  if(settings->CheckBox11->Checked)
	  {
		 m_bMovieControl = true;
	  }
	  else
	  {
		 m_bMovieControl = false;
	  }
	  //strcpy(m_sMovieTarget,settings->DirectoryListBox1->Directory.c_str() );
	  if(settings->CheckBox5->Checked) m_bViewStill = true;
	  if(!settings->CheckBox5->Checked) m_bViewStill = false;

	  //Change back to original directory in case changed by target deirectory
	  //selections:
	  if(chdir(old_dir) == -1)
	  {
#ifdef WIDE_CHAR_APP
		String str = "BEST_MESSAGE";
		wchar_t mes[] = L"Problem changing to old dir!";
		wchar_t cap[100];
		StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
        MessageBox(NULL,"Problem changing to old dir!",BEST_MESSAGE,0);
#endif
	  }

   }
   else
   {
	  //Change back to original directory in case changed by target deirectory
	  //selections:
	  if(chdir(old_dir) == -1)
	  {
#ifdef WIDE_CHAR_APP
		String str = "BEST_MESSAGE";
		wchar_t mes[] = L"Problem changing to old dir!";
		wchar_t cap[100];
		StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
         MessageBox(NULL,"Problem changing to old dir!",BEST_MESSAGE,0);
#endif
	  }

   }

}

TColor TEasyPlotForm::setColor(char *color)
{
    if( !strcmp(color,"White") ) return clWhite;
	if( !strcmp(color,"Black") ) return clBlack;
	if( !strcmp(color,"Red") ) return clRed;
	if( !strcmp(color,"Blue") ) return clBlue;
	if( !strcmp(color,"Yellow") ) return clYellow;
	if( !strcmp(color,"Green") ) return clGreen;
	if( !strcmp(color,"Maroon") ) return clMaroon;
	if( !strcmp(color,"Lime") ) return clLime;
	if( !strcmp(color,"Purple") ) return clPurple;
	if( !strcmp(color,"Grey") ) return clGray;

}//setColor


TColor TEasyPlotForm::setColor1(char *color)
{

	TColor Color;

	if( !strcmp(color,"white") )
	{
		Color = clWhite;

	}
	if( !strcmp(color,"black") )
	{
		Color = clBlack;

	}
	if( !strcmp(color,"red") )
	{
	   Color = clRed;

	}
	if( !strcmp(color,"blue") )
	{
	  Color = clBlue;

	}
	if( !strcmp(color,"yellow") )
	{
	   Color = clYellow;

	}
	if( !strcmp(color,"green") )
	{
	   Color = clGreen;

	}
	if( !strcmp(color,"maroon") )
	{
		Color = clMaroon;

	}
	if( !strcmp(color,"lime") )
	{
	  Color = clLime;

	}
	if( !strcmp(color,"purple") )
	{
		Color = clPurple;

	}
	if( !strcmp(color,"grey") )
	{
		Color = clGray;

	}

	return Color;


}//setColor1




//---------------------------------------------------------------------------
void TEasyPlotForm::setDataSetColors(char *color,int index)
{

   if(!strcmp(color,"White") )
   {
	  m_cDataSet[index] = clWhite;
   }
   if(!strcmp(color,"Maroon") )
   {
	  m_cDataSet[index] = clMaroon;
   }
   if(!strcmp(color,"Black") )
   {
	  m_cDataSet[index] = clBlack;
   }
   if(!strcmp(color,"Red") )
   {
	  m_cDataSet[index] = clRed;
   }
   if(!strcmp(color,"Blue") )
   {
	  m_cDataSet[index] = clBlue;
   }
   if(!strcmp(color,"Yellow") )
   {
	  m_cDataSet[index] = clYellow;
   }
   if(!strcmp(color,"Green")   )
   {
	  m_cDataSet[index] = clGreen;
   }
   if(!strcmp(color,"Lime")   )
   {
	  m_cDataSet[index] = clLime;
   }
   if(!strcmp(color,"Purple")   )
   {
	  m_cDataSet[index] = clPurple;
   }
   if(!strcmp(color,"Grey")   )
   {
	  m_cDataSet[index] = clGray;
   }

}//setDataSetColors


//---------------------------------------------------------------------------
void TEasyPlotForm::setLegendBackgroundColor(char *color)
{
   
   if(!strcmp(color,"White") )
   {
	  m_cLegend = clWhite;
   }
   if(!strcmp(color,"Maroon") )
   {
	  m_cLegend = clMaroon;
   }
   if(!strcmp(color,"Black") )
   {
	  m_cLegend = clBlack;
   }
   if(!strcmp(color,"Red") )
   {
	  m_cLegend = clRed;
   }
   if(!strcmp(color,"Blue") )
   {
	  m_cLegend = clBlue;
   }
   if(!strcmp(color,"Yellow") )
   {
	  m_cLegend = clYellow;
   }
   if(!strcmp(color,"Green")   )
   {
	  m_cLegend = clGreen;
   }
   if(!strcmp(color,"Purple")   )
   {
	  m_cLegend = clPurple;
   }
   if(!strcmp(color,"Grey")   )
   {
	  m_cLegend = clGray;
   }


}//setLegendBackgroundColor


char* TEasyPlotForm::setLegendComboBackgroundColor()
{
   
   if(m_cLegend == clWhite)
   {
	  return "White";
   }
   if(m_cLegend == clBlack)
   {
	  return "Black";
   }
   if(m_cLegend == clRed)
   {
	  return "Red";
   }
   if(m_cLegend == clBlue)
   {
	  return "Blue";
   }
   if(m_cLegend == clYellow)
   {
	  return "Yellow";
   }
   if(m_cLegend == clGreen)
   {
	  return "Green";
   }
   if(m_cLegend == clMaroon)
   {
	  return "Maroon";
   }
   if(m_cLegend == clPurple)
   {
	  return "Purple";
   }
   if(m_cLegend == clGray)
   {
	  return "Grey";
   }


}//setLegendComboBackgroundColor()



char* TEasyPlotForm::setComboBoxDefaultColor(int index)
{

   if(m_cDataSet[index] == clWhite)
   {
	  return "White";
   }
   if(m_cDataSet[index] == clBlack)
   {
	  return "Black";
   }
   if(m_cDataSet[index] == clRed)
   {
	  return "Red";
   }
   if(m_cDataSet[index] == clBlue)
   {
	  return "Blue";
   }
   if(m_cDataSet[index] == clYellow)
   {
	  return "Yellow";
   }
   if(m_cDataSet[index] == clGreen)
   {
	  return "Green";
   }
   if(m_cDataSet[index] == clMaroon)
   {
	  return "Maroon";
   }
   if(m_cDataSet[index] == clPurple)
   {
	  return "Purple";
   }
   if(m_cDataSet[index] == clGray)
   {
	  return "Grey";
   }


}//setComboBoxDefaultColor


void TEasyPlotForm::setRadioDataPlotModes(TSettingsDlg *settings)
{

   if(m_iPlotMode[0] == 0)
   {
	  settings->RadioGroup1->ItemIndex = 0;
   }
   else if(m_iPlotMode[0] == 2)
   {
	  settings->RadioGroup1->ItemIndex = 1;
   }
   else if(m_iPlotMode[0] == 3)
   {
	  settings->RadioGroup1->ItemIndex = 2;
   }
   else if(m_iPlotMode[0] == 4)
   {
	  settings->RadioGroup1->ItemIndex = 3;
   }
   if(m_iPlotMode[1] == 0)
   {
	  settings->RadioGroup2->ItemIndex = 0;
   }
   else if(m_iPlotMode[1] == 2)
   {
	  settings->RadioGroup2->ItemIndex = 1;
   }
   else if(m_iPlotMode[1] == 3)
   {
	  settings->RadioGroup2->ItemIndex = 2;
   }
   else if(m_iPlotMode[1] == 4)
   {
	  settings->RadioGroup2->ItemIndex = 3;
   }
   if(m_iPlotMode[2] == 0)
   {
	  settings->RadioGroup3->ItemIndex = 0;
   }
   else if(m_iPlotMode[2] == 2)
   {
	  settings->RadioGroup3->ItemIndex = 1;
   }
   else if(m_iPlotMode[2] == 3)
   {
	  settings->RadioGroup3->ItemIndex = 2;
   }
   else if(m_iPlotMode[2] == 4)
   {
	  settings->RadioGroup3->ItemIndex = 3;
   }



}//setRadioDataPlotModes



void TEasyPlotForm::setDataPlotModes(int index1,int index2,int index3)
{
     if(index1 == 0)
	 {
		m_iPlotMode[0] = 0;
	 }
	 else if(index1 == 1)
	 {
		m_iPlotMode[0] = 2;
	 }
	 else if(index1 == 2)
	 {
		m_iPlotMode[0] = 3;
	 }
	 else if(index1 == 3)
	 {
		m_iPlotMode[0] = 4;
	 }
	 if(index2 == 0)
	 {
		m_iPlotMode[1] = 0;
	 }
	 else if(index2 == 1)
	 {
		m_iPlotMode[1] = 2;
	 }
	 else if(index2 == 2)
	 {
		m_iPlotMode[1] = 3;
	 }
	 else if(index2 == 3)
	 {
		m_iPlotMode[1] = 4;
	 }
	 if(index3 == 0)
	 {
		m_iPlotMode[2] = 0;
	 }
	 else if(index3 == 1)
	 {
		m_iPlotMode[2] = 2;
	 }
	 else if(index3 == 2)
	 {
		m_iPlotMode[2] = 3;
	 }
	 else if(index3 == 3)
	 {
		m_iPlotMode[2] = 4;
	 }


}//setDataPlotModes


char* TEasyPlotForm::getColorText(TColor c)
{
   if(c == clWhite) return "White";
   if(c == clBlack) return "Black";
   if(c == clRed) return "Red";
   if(c == clBlue) return "Blue";
   if(c == clYellow) return "Yellow";
   if(c == clGreen) return "Green";
   if(c == clMaroon) return "Maroon";
   if(c == clLime) return "Lime";
   if(c == clPurple) return "Purple";
   if(c == clGray) return "Grey";

}// setGraphComboColors




void __fastcall TEasyPlotForm::createBtnClick(TObject *Sender)
{

   //Create .dat file from .csv file:
   floatmat data;
   FILE *fp;
   int numDataSets, numRows, numCols;
   float value;
   char tmp1[10], newFileName[100];
   int intvar, len;

   //Open .csv file:
   //Define data plot file name:
   intvar = Edit1->GetTextLen();
   Edit1->GetTextBuf(m_sDataFile,intvar+1);
   if( (fp=fopen(m_sDataFile,"rt")) == NULL)
   {
        MessageBox(NULL,"Cannot open required .csv file!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
        return;
   }//if


   if( (fp=fopen(m_sDataFile,"rt")) == NULL)
   {

#ifdef WIDE_CHAR_APP
		 String str = "BEST_MESSAGE";
         wchar_t mes[] = L"Cannot open required .csv file!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
        MessageBox(NULL,"Cannot open required .csv file!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif

		return;
   }//if


   char ch[1], tmp[1];

   //Get number of cols for .dat file:
   numCols = 0;
   while(!feof(fp))
   {
	  fscanf(fp,"%f",&value);
	  fscanf(fp,"%c",ch);
	  numCols++;
	  if( ch[0] =='\n' )
	  {
		 break;
	  }
   }//while
   fseek(fp,0,SEEK_SET);

   //Get number of rows for .dat file:
   numRows = 0;
   while(!feof(fp))
   {
	  for(int i=0;i<numCols;i++)
	  {
		 fscanf(fp,"%f",&value);
		 fscanf(fp,"%c",ch);
	  }
	  numRows++;
   }//while
   fseek(fp,0,SEEK_SET);

   //Set up required dimensions for floatmat object:
   numRows = numRows - 1;
   data.reDim(numRows,numCols);

   //Set data:
   data.SetPtrBegin();

   for(int j=0;j<numRows;j++)
   {
	  for(int i=0;i<numCols;i++)
	  {
		 fscanf(fp,"%f",&value);
		 *data.curPtr() = value;
		 data++;
		 fscanf(fp,"%c",ch);
	  }//i
   }//j


   //Redefine data file name:
   strcpy(newFileName,m_sDataFile);
   len = strlen(newFileName);
   newFileName[len-1] = 't';
   newFileName[len-2] = 'a';
   newFileName[len-3] = 'd';

   data.save(newFileName);

 #ifdef WIDE_CHAR_APP
   String str = "BEST_MESSAGE";
   wchar_t mes[] = L"The .csv file has been successfully converted to standard .dat format.";
   wchar_t cap[100];
   StringToWideChar(str,cap,99);
   	MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
     MessageBox(NULL,"The .csv file has been successfully converted to standard .dat format.",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif

   fclose(fp);

}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::convertBtnClick(TObject *Sender)
{

   //Get name of file
   floatmat data;
   char tmp[10], newFileName[100];;
   int intvar;

   if(!m_bIsWaveFile)
   {
	  //Define data plot file name:
      intvar = Edit1->GetTextLen();
      Edit1->GetTextBuf(m_sDataFile,intvar+1);

	  data.load(m_sDataFile);
	  //Redefine data file:
	  strcpy(newFileName, m_sDataFile);
	  int len = strlen(newFileName);
	  newFileName[len-2] = 'x';
	  newFileName[len-3] = 't';
	  data.textSave(newFileName);

#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"The .dat file has been successfully converted to standard .txt format.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
      MessageBox(NULL,"The .dat file has been successfully converted to standard .txt format.",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif


   }
   else
   {
#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot convert .wav file to standard .txt format!";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
      MessageBox(NULL,"Cannot convert .wav file to standard .txt format!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif


   }

}
//---------------------------------------------------------------------------



void __fastcall TEasyPlotForm::Button4Click(TObject *Sender)
{
   //View requested wavefile:
   char old_dir[MAXPATH],new_dir[MAXPATH];
   char fileName[200];

   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);

   int endWordPos;
   char tmp1[10];

   strcpy(tmp1,"");


   //OpenDialog1->Filter = "wav;dat;txt;csv";
   OpenDialog1->Filter =  "Data files (*.dat)|*.DAT|CSV files (Comma delimited) (*.csv)|*.CSV|Text files (*.txt)|*.TXT|Wave files (*.wav)|*.WAV";
   OpenDialog1->Title = "Select required file ...";
   //OpenDialog1->FileName = ".csv";
   if( OpenDialog1->Execute())
   {

      //Define selection in main GUI
      Edit1->SetTextBuf((OpenDialog1->FileName).c_str());
      strcpy(m_sDataFile,(OpenDialog1->FileName).c_str());



      //Change back to original directory:
      if(chdir(old_dir) == -1)
      {
         MessageBox(NULL,"Problem changing to old dir!","",0);
      }

   }
   else
   {
      //Change back to original directory:
      if(chdir(old_dir) == -1)
      {
         MessageBox(NULL,"Problem changing to old dir!","",0);
      }
   }


}
//---------------------------------------------------------------------------
unsigned long int TEasyPlotForm::readWaveHeaderFile(FILE *fp)
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

   m_iPlaybackWaveFileSamplesPerSec =  nSamplesPerSec;
   m_iNumChannels = nChannels;
   m_iBitsPerSample = wBitsPerSample;

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
#ifdef WIDE_CHAR_APP
		//Format structure not supported
		String str = "BEST_MESSAGE";
		wchar_t mes[] = L"Audio file format not supported.";
		wchar_t cap[100];
		StringToWideChar(str,cap,99);
 		MessageBox(NULL,mes,cap, MB_ICONINFORMATION);
#else
        MessageBox(NULL,"Audio file format not supported.",BEST_MESSAGE, MB_ICONINFORMATION);

#endif


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


void __fastcall TEasyPlotForm::Edit6Change(TObject *Sender)
{
   /*
   int intvar, xmin;
   char val[20];

  //Check contents of edit boxes for Auto-Scale values:
  bool autoscale = false;

   intvar = Edit6->GetTextLen();
   Edit6->GetTextBuf(val,intvar+1);
   xmin = atoi(val);


   if( (xmin == (int) m_fAutoXMin)  )
   {

      m_bAutoXMin = true;
   }
   else
   {
      m_bAutoXMin = false;
   }

  if( (strcmp(val,"autoscale")== 0) )
  {
      m_bAutoXMin = true;
  }

  if( m_bAutoXMin && m_bAutoXMax && m_bAutoYMin && m_bAutoYMax)
  {
      autoscale = true;
  }
  else
  {
    autoscale = false;
  }

  if(autoscale)
  {
     CheckBox2->Checked = true;
  }
  else
  {
    CheckBox2->Checked = false;
  }
  */

}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Edit5Change(TObject *Sender)
{
 /*
  int intvar,xmax;
   char val[20];

  //Check contents of edit boxes for Auto-Scale values:

  bool autoscale = false;


   intvar = Edit5->GetTextLen();
   Edit5->GetTextBuf(val,intvar+1);
   xmax = atoi(val);

   if( xmax == (int) m_fAutoXMax )
   {

      m_bAutoXMax = true;
   }
   else
   {
      m_bAutoXMax = false;
   }


  if( (strcmp(val,"autoscale")== 0) )
  {
      m_bAutoXMax = true;
  }

  if( m_bAutoXMin && m_bAutoXMax && m_bAutoYMin && m_bAutoYMax)
  {
      autoscale = true;
  }
  else
  {
    autoscale = false;
  }

  if(autoscale)
  {
     CheckBox2->Checked = true;
  }
  else
  {
    CheckBox2->Checked = false;
  }
  */

}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Edit7Change(TObject *Sender)
{
  /*
  int intvar,ymin;
   char val[20];

  //Check contents of edit boxes for Auto-Scale values:
  bool autoscale = false;

   intvar = Edit7->GetTextLen();
   Edit7->GetTextBuf(val,intvar+1);
   ymin = atoi(val);


   if( ymin == (int) m_fAutoYMin )
   {
      m_bAutoYMin = true;
   }
   else
   {
      m_bAutoYMin = false;
   }


  if( (strcmp(val,"autoscale")== 0) )
  {
      m_bAutoYMin = true;
  }

  if( m_bAutoXMin && m_bAutoXMax && m_bAutoYMin && m_bAutoYMax)
  {
      autoscale = true;
  }
  else
  {
    autoscale = false;
  }

  if(autoscale)
  {
     CheckBox2->Checked = true;
  }
  else
  {
    CheckBox2->Checked = false;
  }
  */
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Edit8Change(TObject *Sender)
{

  /*
  int intvar,ymax;
   char val[20];

  //Check contents of edit boxes for Auto-Scale values:
  bool autoscale = false;

   intvar = Edit8->GetTextLen();
   Edit8->GetTextBuf(val,intvar+1);
   ymax = atoi(val);
   if( ymax == (int) m_fAutoYMax )
   {
         m_bAutoYMax = true;
   }
   else
   {
        m_bAutoYMax = false;
   }

   if( (strcmp(val,"autoscale")== 0) )
  {
      m_bAutoYMax = true;
  }

  if( m_bAutoXMin && m_bAutoXMax && m_bAutoYMin && m_bAutoYMax)
  {
      autoscale = true;
  }
  else
  {
    autoscale = false;
  }

  if(autoscale)
  {
     CheckBox2->Checked = true;
  }
  else
  {
    CheckBox2->Checked = false;
  }
 */
}

//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::CheckBox2Click(TObject *Sender)
{


 #ifdef WIDE_CHAR_APP
   if(CheckBox2->Checked)
   {
   	  Edit5->Enabled = false;
	  Edit6->Enabled = false;
	  Edit7->Enabled = false;
	  Edit8->Enabled = false;

	  //Repopulate edit boxes with auto-settings:
	  String str = m_sAutoNumStr;
	  wchar_t buf[100];
	  StringToWideChar(str,buf,99);


	  float2Str(m_fAutoXMax);
	  //Edit5->SetTextBuf(m_sAutoNumStr);
	  Edit5->SetTextBuf(buf);
	  float2Str(m_fAutoXMin);
	  //Edit6->SetTextBuf(m_sAutoNumStr);
	  Edit6->SetTextBuf(buf);
	  float2Str(m_fAutoYMin);
	  //Edit7->SetTextBuf(m_sAutoNumStr);
	  Edit7->SetTextBuf(buf);
	  float2Str(m_fAutoYMax);
	  //Edit8->SetTextBuf(m_sAutoNumStr);
	  Edit8->SetTextBuf(buf);
	}
	else
	{
	  wchar_t buf[] = L"";

	  Edit5->Enabled = true;
	  Edit6->Enabled = true;
	  Edit7->Enabled = true;
	  Edit8->Enabled = true;

	  Edit5->SetTextBuf(buf);
	  Edit6->SetTextBuf(buf);
	  Edit7->SetTextBuf(buf);
	  Edit8->SetTextBuf(buf);


	}
#else
    if(CheckBox2->Checked)
   {

      Edit5->Enabled = false;
      Edit6->Enabled = false;
      Edit7->Enabled = false;
      Edit8->Enabled = false;

      //Repopulate edit boxes with auto-settings:
      float2Str(m_fAutoXMax);
      Edit5->SetTextBuf(m_sAutoNumStr);
      float2Str(m_fAutoXMin);
      Edit6->SetTextBuf(m_sAutoNumStr);
      float2Str(m_fAutoYMin);
      Edit7->SetTextBuf(m_sAutoNumStr);
      float2Str(m_fAutoYMax);
      Edit8->SetTextBuf(m_sAutoNumStr);

    }
    else
    {
      Edit5->Enabled = true;
      Edit6->Enabled = true;
      Edit7->Enabled = true;
      Edit8->Enabled = true;

    }

#endif

}
//---------------------------------------------------------------------------
void TEasyPlotForm::float2Str(float num)
{

   float autoNumber;
   char fspec[20], sign[2];
   int exp = m_GraphFrm->getExponent(num);
   strcpy(sign,"");

   if(exp < 1) //-ve exponents:
   {
	  autoNumber = num;
	  for(int i=0;i<abs(exp);i++)
	  {
		 autoNumber *= 10.0;
	  }
	  //Build up the text string:
	  strcpy(fspec,"%s%6.6f%s%s");
	  char tmp[10];
	  itoa(exp,tmp,10);
	  sprintf(m_sAutoNumStr,fspec,sign,autoNumber,"E",tmp);
	  int mike = 4;
   }
   else if(exp >= 0 )
   {
	  autoNumber = num;
	  for(int i=0;i<abs(exp);i++)
	  {
		 autoNumber /= 10.0;
	  }
	  //Build up the text string:
	  strcpy(fspec,"%s%6.6f%s%s");
	  char tmp[10], esign[10];
	  itoa(exp,tmp,10);
	  strcpy(esign,"+");
	  strcat(esign,tmp);
	  strcpy(tmp,esign);

	  sprintf(m_sAutoNumStr,fspec,sign,autoNumber,"E",tmp);
	  int mike = 4;

   }

}//float2Str1

#ifdef ORI_WIN_CODE
void __fastcall TEasyPlotForm::animateBtnClick(TObject *Sender)
{
   
   //Run the plot code:

   //Dialog box to choose 'program file':
  //View requested wavefile:
   char old_dir[MAXPATH],new_dir[MAXPATH];
   char fileName[200];

   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);
   strcpy(m_sOldDir,old_dir);

   int endWordPos;
   char tmp1[10];

   strcpy(tmp1,"");


   //OpenDialog1->Filter = "wav;dat;txt;csv";
   OpenDialog1->Filter =  "Tag program files (*.tag)|*.TAG|Def files (*.def)|*.DEF";
   OpenDialog1->Title = "Select required animation program file ...";
   OpenDialog1->FileName = "";
   OpenDialog1->Options << ofHideReadOnly << ofEnableSizing << ofAllowMultiSelect;
   OpenDialog1->FilterIndex = 2; //Show  DEF files first
   if( OpenDialog1->Execute())
   {

	  if(OpenDialog1->Files->Count == 1 )
	  {
		 //Process for only one .tag or one .def file
		 if(OpenDialog1->FilterIndex == 1)
		 {
			//Process a single .tag file
			char str[200];
#ifdef WIDE_CHAR_APP
			wcstombs(str, (OpenDialog1->FileName).c_str(), 199);
#else
            strcpy(str, (OpenDialog1->FileName).c_str());
#endif
			//strcpy(m_sTagFile,(OpenDialog1->FileName).c_str());
			strcpy(m_sTagFile,str);
			m_iTagFileCount = 1;

		 }
		 if(OpenDialog1->FilterIndex == 2)
		 {

		   char str[200];
#ifdef WIDE_CHAR_APP
		   wcstombs(str, (OpenDialog1->FileName).c_str(), 199);
#else
           strcpy(str, (OpenDialog1->FileName).c_str());
#endif
		   strcpy(m_sDefFile,str);

		   //Set up the tag file list from control (DEF) file:
		   parseDefFile();
		 }

	  }
	  else
	  {
		 //Check that multi fils are of .tsg type (.def type multi choice invalid)
		 if(OpenDialog1->FilterIndex == 2)
		 {
#ifdef WIDE_CHAR_APP
		   String str = "BEST_MESSAGE";
		   wchar_t mes[] = L"Multi Selection for a .DEF file is invalid\nPlease try again.";
		   wchar_t cap[100];
		   StringToWideChar(str,cap,99);

		   MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
		   return;
#else
           MessageBox(NULL,"Multi Selection for a .DEF file is invalid\nPlease try again.",
           BEST_MESSAGE,MB_ICONINFORMATION);
		   return;
#endif

		 }

		 //Multi tagfile select:
		 m_bMultiTagFiles = true;
		 if(m_SLTagFiles == NULL)
		 {
			m_SLTagFiles = new TStringList();

		 }
		 m_iTagFileCount = 0;
		 for(int i=0;i<OpenDialog1->Files->Count;i++)
		 {
			m_SLTagFiles->Add(OpenDialog1->Files->Strings[i]);
			m_iTagFileCount++;
		 }


		 //Reorder stringlist to ensure entries are in alphabetical order:
		 int index= 1;
		 char numStr[10];
		 itoa(index,numStr,10);
		 strcat(numStr,".tag");
		 TStringList *tempList = new TStringList();
		 //Initialise temp string:
		 for(int i=0;i<m_iTagFileCount;i++)
		 {
			tempList->Add("");
		 }

		 bool processing = true;

		 //Check files are numbered correctly (1...n in steps of 1)
		 bool match = false;
		 while(processing)
		 {
			for(int i=0;i<m_iTagFileCount;i++)
			{
			   char str[200];
#ifdef WIDE_CHAR_APP
			   wcstombs(str, m_SLTagFiles->Strings[i].c_str(), 199);
#else
               strcpy(str, m_SLTagFiles->Strings[i].c_str());
#endif

			   //if( strstr(m_SLTagFiles->Strings[i].c_str(),numStr) != NULL )
			   if( strstr(str,numStr) != NULL )
			   {
				 match = true;
				 tempList->Strings[index-1] = m_SLTagFiles->Strings[i];
				 break;
			   }
			}//i

			if(!match)
			{
			   //char mes[200];
			   //sprintf(mes,"%s%s","Tag files not numbered in correct sequence.\n",
			   //		   "Please try again.");
#ifdef WIDE_CHAR_APP
			   String str = "BEST_MESSAGE";
			   wchar_t mes[] = L"Tag files not numbered in correct sequence.\nPlease try again.";
			   wchar_t cap[100];
			   StringToWideChar(str,cap,99);

			   MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
			   return;
#else
               MessageBox(NULL,"Tag files not numbered in correct sequence.\nPlease try again.",
                          BEST_MESSAGE,MB_ICONINFORMATION);
			   return;
#endif

			}
			else
			{
			   index++;
			   itoa(index,numStr,10);
			   strcat(numStr,".tag");
			   match = false;
			}
			if(index==m_iTagFileCount+1)
			{
			   processing = false;
			}

		 }//while


		 for(int i=0;i<m_iTagFileCount;i++)
		 {
			m_SLTagFiles->Strings[i] = tempList->Strings[i];

			String str = "BEST_MESSAGE";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			//MessageBox(NULL,m_SLTagFiles->Strings[i].c_str(),cap,0);
		 }//i

		 //Assign first tsg file name:
		 char str[200];
#ifdef WIDE_CHAR_APP
		 wcstombs(str,m_SLTagFiles->Strings[0].c_str() , 199);
#else
         strcpy(str,m_SLTagFiles->Strings[0].c_str());
#endif
		 //strcpy(m_sTagFile,m_SLTagFiles->Strings[0].c_str());
		 strcpy(m_sTagFile,str);
	  }


	  setUpSubDir();
	  /*
	  //Create required subdir for target files:
	  char tmp[10];

	  getDirectoryTag();
	  m_iDirCnt = 1;
	  m_iAniCumCnt = 1;
	 itoa(m_iDirCnt,tmp,10);
	 char dir[200];
	 sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
	 strcpy(m_sMovieTarget,dir);
	 if(!DirectoryExists(m_sMovieTarget))
	 {
		if( mkdir(m_sMovieTarget) == -1 )
		{
			String str = "BEST_MESSAGE";
			wchar_t mes[] = L"Problem creating target sub-directory.";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);

		}
	 }

	  //Change back to original directory:
	  if(chdir(old_dir) == -1)
	  {

		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
	  }
	  */
   }
   else
   {
	  //Change back to original directory:
	  if(chdir(old_dir) == -1)
	  {
#ifdef WIDE_CHAR_APP
		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
        MessageBox(NULL,"Problem changing to old dir!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif
	  }
	  return;
   }

   //Initialise flag for <VAR> section
   m_bVARPresent = true;
   //Parse the animation program file
   //parseTagFile();
   parseTagFile1();

   //Initialise variables for horizontal/vertical ref lines:
	m_fHPos = 0.0;
	m_fVPos = 0.0;

	//Delete any previous instance of graphFrm:
	if(m_GraphFrm != NULL)
	{
	   delete m_GraphFrm;
	   m_GraphFrm = NULL;
	}


   //Launch the animation production process:
   m_bAnimation = true;
   m_iAniCnt = 1;
   m_iAniCumCnt = 1;
   m_iMaxTotAniCnt = 0;
   m_bStartTimer1 = true;
   Timer1->Enabled = true;

}

void TEasyPlotForm::setUpSubDir()
{

      //Create required subdir for target files:
	  char tmp[10];

	  getDirectoryTag();
	  m_iDirCnt = 1;
	  m_iAniCumCnt = 1;
	 itoa(m_iDirCnt,tmp,10);
	 char dir[200];
	 sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
	 strcpy(m_sMovieTarget,dir);
	 if(!DirectoryExists(m_sMovieTarget))
	 {
		if( mkdir(m_sMovieTarget) == -1 )
		{
#ifdef WIDE_CHAR_APP
			String str = "BEST_MESSAGE";
			wchar_t mes[] = L"Problem creating target sub-directory.";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
            MessageBox(NULL,"Problem creating target sub-directory.",
                       BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif


		}
	 }

	  //Change back to original directory:
	  //if(chdir(old_dir) == -1)
	  if(chdir(m_sOldDir) == -1)
	  {
#ifdef WIDE_CHAR_APP

		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
         MessageBox(NULL,"Problem changing to old dir!",
                       BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif

	  }




}//setUpSubDir



//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::File1Click(TObject *Sender)
{
   //TBD
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Close1Click(TObject *Sender)
{
   LPCTSTR path = "Help\\easyplotter.hlp";
   WinHelp(Handle,path,HELP_QUIT,0);
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::HelpTopics1Click(TObject *Sender)
{
   //Launch EasyPlotter Help
   //APPLaunch app;

   LPCTSTR path = "Help\\easyplotter.hlp";
   WinHelp(Handle,path,HELP_FINDER,0);

}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Open1Click(TObject *Sender)
{
   Button4Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Timer1Timer(TObject *Sender)
{
   
   char mes[200],index[10];

   //Timer control for animation effects:

   //Close the previous graph

   if(m_GraphFrm != NULL)
   {


	  //Message to status bar:
	  if(!m_bOverride)
	  {
		itoa(m_iAniCnt,index,10);
	  }
	  else
	  {
		itoa(m_iAniCumCnt,index,10);
	  }

	  sprintf(mes,"%s%s%s","Processing movie still  ",index," ..........");
	  StatusBar1->Panels->Items[1]->Text = mes;

	  //Produce bitmap of current graph:
	  char filename[200],tmp[10];
	  if(!m_bOverride)
	  {
		itoa(m_iAniCnt,tmp,10);

	  }
	  else
	  {
		itoa(m_iAniCumCnt,tmp,10);
	  }
	  sprintf(filename,"%s%s%s%s%s",m_sMovieTarget,"\\",m_sMovieTag,tmp,".bmp");
	  m_GraphFrm->getImage()->Picture->SaveToFile(filename);

	  //Delete graph instance:
	  delete m_GraphFrm;
	  m_GraphFrm = NULL;

	  m_iAniCnt++;
	  m_iAniCumCnt++;

	  //if(m_iTagFileCount == m_iDirCnt)
	  if(m_iDirCnt > m_iTagFileCount)
	  {
		 Timer1->Enabled = false;
		 m_bAnimation = false;
		 //Message to status bar:
		 sprintf(mes,"%s","Processing of movie stills has been completed!");
		 StatusBar1->Panels->Items[1]->Text = mes;
		 //MessageBox(NULL,"Movie stills have been completed.",BEST_MESSAGE,MB_ICONINFORMATION);
		 m_bFileIgnore = false;
		 return;
	  }

   }

   //Launch plot routine
	PlotBtnClick(Sender);
	if(m_bStartTimer1)
	{
	   m_bStartTimer1 = false;
	   //m_iAniCnt++;
	   //m_iAniCumCnt++;
	}

   //Set required no of points to plot and keep record
   //if(m_iAniCnt > m_iTotAniCnt)

   if(m_iAniCnt > m_iMaxTotAniCnt)
   {
	  //Perform any other animations required here:
	  if(m_iTagFileCount == m_iDirCnt)
	  {
		  /*
		  Timer1->Enabled = false;
		  m_bAnimation = false;
		 //Message to status bar:
		 sprintf(mes,"%s","Processing of movie stills has been completed!");
		 StatusBar1->Panels->Items[1]->Text = mes;
		 //MessageBox(NULL,"Movie stills have been completed.",BEST_MESSAGE,MB_ICONINFORMATION);
		 m_bFileIgnore = false;
		 */
		 m_iDirCnt++;
	  }
	  else
	  {
			 //Prepare for next program tag file and create next sub dir:
			 char tmp[10];

			 getDirectoryTag();
			 m_iDirCnt++;
			itoa(m_iDirCnt,tmp,10);
			if(!m_bOverride)
			{
				char dir[200];
				sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
				strcpy(m_sMovieTarget,dir);
				if(!DirectoryExists(m_sMovieTarget))
				{
				   if( mkdir(dir) == -1 )
				   {
#ifdef WIDE_CHAR_APP
					   String str = "BEST_MESSAGE";
					   wchar_t mes[] = L"Problem creating target sub-directory.";
					   wchar_t cap[100];
					   StringToWideChar(str,cap,99);

					   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
                       MessageBox(NULL,"Problem creating target sub-directory.",
                                  BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif

				   }
				}
			}
			//Initialise flag for <VAR> section
			 m_bVARPresent = true;
			 m_iMaxTotAniCnt = 0;
			 //Parse the animation program file
			 //Get new tag file name:

			 //wchar_t wstr[100] = L"hello world";
			 char str[200];
#ifdef WIDE_CHAR_APP
			 wcstombs(str, m_SLTagFiles->Strings[m_iDirCnt-1].c_str(),199);
#else
             strcpy(str, m_SLTagFiles->Strings[m_iDirCnt-1].c_str());
#endif
			 //strcpy(m_sTagFile,m_SLTagFiles->Strings[m_iDirCnt-1].c_str());
			 strcpy(m_sTagFile,str);
			 //parseTagFile();
			 parseTagFile1();

			 //Initialise variables for horizontal/vertical ref lines:
			  m_fHPos = 0.0;
			  m_fVPos = 0.0;

			 m_iAniCnt = 0;//1;

	  }
   }



}
//---------------------------------------------------------------------------


void TEasyPlotForm::parseTagFile1()
{
   char contents[9000];
   char ch[2];

   m_bProcFIXED = false;
   m_bProcVAR = false;
   bool m_bValid = true;

   if( (m_fpTagFile=fopen(m_sTagFile,"rt")) == NULL)
   {
	  //char mes[200];
	  //sprintf(mes,"%s%s","Cannot open required Animation Program File.\n",
	  //		  "Please try again.");
#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open required Animation Program File.\nPlease try again.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  //return;
	  exit(1);
#else
      MessageBox(NULL,"Cannot open required Animation Program File.\nPlease try again.",
                 BEST_MESSAGE,MB_ICONEXCLAMATION);
	  //return;
	  exit(1);
#endif
   }
   //Read contents of file:
   int byteCnt = 0;
   while( !feof(m_fpTagFile))
   {
	  fread(ch,sizeof(char),1,m_fpTagFile);
	  contents[byteCnt] = ch[0];
	  //contents++;
	 byteCnt++;
   }
   contents[byteCnt] = '\0';
   fclose(m_fpTagFile);

   //Search for <FIXED> section
   m_bProcFIXED = true;
   m_iNumFIXEDPlots = 0;
   char *fixed_ptr1 = NULL;
   char *fixed_ptr2 = NULL;
   char fixed_str[1000];
   if( strstr(contents,"<FIXED>") != NULL)
   {
	  //Process <FIXED> animation instructions:
	  fixed_ptr1 = strstr(contents,"<FIXED>");
	  fixed_ptr2 = strstr(contents,"</FIXED>");
	  int len = strlen(fixed_ptr1) - strlen(fixed_ptr2);
	  strcpy(fixed_str,fixed_ptr1);
	  fixed_str[len-1] = '\0';


	  //Parsing the <FIXED> instructions


	  int fixedCnt = 0;
	  bool plotExcess = false;
	  for(int i=0;i<m_iNumPlotTypes;i++)
	  {
		 char *p = NULL;
		 p= fixed_str;
		 if(plotExcess) break;

		  while( strstr(p,m_sPLOTTYPES[i]) != NULL )
		 {
			strcpy(m_sFIXED[fixedCnt],m_sPLOTTYPES[i]);
			p = strstr(p,m_sFIXED[fixedCnt]);
			//Get the argument list:
			//if( !getArgList(p,fixedCnt) )
			if( !getArgList1(p,fixedCnt) )
			{
			   //Problem with argument list:
			   return;
			}

			//Advance to end of parameter list:
			p = strstr(p,";");
			fixedCnt++;
			if(fixedCnt == MAX_NUM_FIXEDPLOTS)
			{
			   plotExcess = true;
			   m_iNumFIXEDPlots = MAX_NUM_FIXEDPLOTS;
			}
			else
			{
			  m_iNumFIXEDPlots = fixedCnt;
			}
			if(p==NULL) break;
		 }//while

	  }//i


   }//<FIXED>



   if( strstr(contents,"<VAR>") == NULL)
   {

	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Warning: A <VAR> section has not been defined in tag file.\nOK.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  //MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
	  m_bVARPresent = false;
	  m_iTotAniCnt = 1;  //Prevent stills series production:
	  return;
   }
   else
   {
		//Search for <VAR> section
	   m_bProcVAR = true;
	   m_bProcFIXED = false;
	   m_iNumVARPlots = 0;
	   char *var_ptr1 = NULL;
	   char *var_ptr2 = NULL;
	   char var_str[1000];
	   if( strstr(contents,"<VAR>") != NULL)
	   {
		  //Process <VAR> animation instructions:
		  var_ptr1 = strstr(contents,"<VAR>");
		  var_ptr2 = strstr(contents,"</VAR>");
		  int len = strlen(var_ptr1) - strlen(var_ptr2);
		  strcpy(var_str,var_ptr1);
		  var_str[len-1] = '\0';


		  //Parsing the <VAR> instructions
		  int varCnt = 0;
		  bool plotExcess = false;
		  for(int i=0;i<m_iNumPlotTypes;i++)
		  {
			 char *p = NULL;
			 p= var_str;
			 if(plotExcess) break;

			  while( strstr(p,m_sPLOTTYPES[i]) != NULL )
			 {
				strcpy(m_sVAR[varCnt],m_sPLOTTYPES[i]);
				p = strstr(p,m_sVAR[varCnt]);
				//Get the argument list:
				//if( !getArgList(p,fixedCnt) )
				if( !getArgList1(p,varCnt) )
				{
				   //Problem with argument list:
				   return;
				}
				m_bValid = true;
				//Advance to end of parameter list:
				p = strstr(p,";");
				varCnt++;
				if(varCnt == MAX_NUM_VARPLOTS)
				{
				   plotExcess = true;
				   m_iNumVARPlots = MAX_NUM_VARPLOTS;
				}
				else
				{
				  m_iNumVARPlots = varCnt;
				}
				if(p==NULL) break;
			 }//while

		  }//i

	   }

   }//else (VAR)

   if(!m_bValid)
  {
	 //char mes[200];
	 //sprintf(mes,"%s%s","A valid plot instruction has not been defined in tag file.\n",
	 //		"                     OK ");

#ifdef WIDE_CHAR_APP
	 String str = "BEST_MESSAGE";
	 wchar_t mes[] = L"A valid plot instruction has not been defined in tag file.\nOK ";
	 wchar_t cap[100];
	 StringToWideChar(str,cap,99);

	 MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	 return;
#else
     MessageBox(NULL,"A valid plot instruction has not been defined in tag file.\nOK ",
                BEST_MESSAGE,MB_ICONEXCLAMATION);
	 return;
#endif
  }

}//parseTagFile1()

bool TEasyPlotForm::getArgList1(char *p,int index)
{

   char ch[2],tmp[100];

   //Check that an end of parameter list delimiter is present
   if( strstr(p,";") == NULL)
   {
	   //char mes[200];
	   //sprintf(mes,"%s%s","An end of argument list delimiter is not present.\n",
	   //		  "Please edit the program file.");

#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"An end of argument list delimiter is not present.\nPlease edit the program file. ";;
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);

	  return false;
#else
      MessageBox(NULL,"An end of argument list delimiter is not present.\nPlease edit the program file. "
                ,BEST_MESSAGE,MB_ICONEXCLAMATION);

	  return false;
#endif

   }


   //if(m_bProcVAR)
   //{

	  //Set pointer and initialise argument list:
	  if(m_bProcVAR)
	  {
		 //for(int k=0;k<strlen(m_sVARSingle);k++) p++;
		 //for(int i=0;i<MAX_ARGS;i++) strcpy(m_sVARSingleParams[i],"");
		 for(int k=0;k<strlen(m_sVAR[index]);k++) p++;
		 for(int i=0;i<MAX_ARGS;i++) strcpy(m_sVARParams[index][i],"");
	  }
	  else if(m_bProcFIXED)
	  {
		 for(int k=0;k<strlen(m_sFIXED[index]);k++) p++;
		 for(int i=0;i<MAX_ARGS;i++) strcpy(m_sFIXEDParams[index][i],"");
	  }

	  bool arg_parsing = true;

	  strcpy(tmp,"");
	  int argCnt = 0;
	  int charCnt = 0;
	  while (arg_parsing)
	  {
		 ch[0] = p[charCnt];
		 ch[1] = '\0';
		 if( (ch[0] != ',') )
		 {
			if( ch[0] != ';')
			{
			   strcat(tmp,ch);
			   charCnt++;
			}
			else
			{
			   if(m_bProcVAR)
			   {
				  strcpy(m_sVARParams[index][argCnt],tmp);
			   }
			   else if(m_bProcFIXED)
			   {
				  strcpy(m_sFIXEDParams[index][argCnt],tmp);
			   }
			   //MessageBox(NULL,m_sVARParams[argCnt],"",0);
			}
		 }
		 else
		 {
			if(m_bProcVAR)
			{
			   strcpy(m_sVARParams[index][argCnt],tmp);
			}
			else if(m_bProcFIXED)
			{
			   strcpy(m_sFIXEDParams[index][argCnt],tmp);
			}
			//MessageBox(NULL,m_sVARParams[argCnt],"",0);
			strcpy(tmp,"");
			argCnt++;
			charCnt++;
		 }

		 //Terminate parsing
		 if(ch[0] == ';')
		 {
			arg_parsing = false;
			//Define final argument count:
			m_iNumArgs = argCnt+1;
			if(m_bProcFIXED)
			{
				m_iNumFIXEDPlotArgs[index] = m_iNumArgs;
			}
			if(m_bProcVAR)
			{
				m_iNumVARPlotArgs[index] = m_iNumArgs;
			}
		 }


	  }//while
   //}

   return true;

}//getArgList


void __fastcall TEasyPlotForm::previewBtnClick(TObject *Sender)
{

   int intvar, index;
   char tmp[20], filename[200], fStr[20];

  //Read in slide and get dimensions:
  //Set up required image canvas:
  if(m_ImMovie == NULL)
  {
	m_ImMovie = new TImage(this);
  }

  //Get the directory tag:
  getDirectoryTag();

  m_iPreviewIndex = m_iPreviewIndexStart;
  itoa(m_iPreviewIndex,tmp,10);
  sprintf(fStr,"%s%s",m_sMovieTag,tmp);
  m_iDirCnt = 1;
  itoa(m_iDirCnt,tmp,10);
  sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

  FILE *fp;
  if( (fp=fopen(filename,"rb")) == NULL )
  {

#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open specified files.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);
      MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  return;
#else
      MessageBox(NULL,"Cannot open specified files.",BEST_MESSAGE,MB_ICONEXCLAMATION);
	  return;
#endif

  }
  fclose(fp);

  StatusBar1->Panels->Items[0]->Text = "Preview mode";
  StatusBar1->Panels->Items[1]->Text = "";

  /*
  if(m_bDisableGraphMenu)
  {
	   //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
  }
  */

  m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
   TBorderStyle bStyle;
   if(m_bDisableGraphMenu)
   {
	   //Removes top border area
	   bStyle = bsNone;
   }
   else
   {
	 bStyle = bsSizeable;
   }
   m_MovieForm = new TMovieForm(this,m_ImMovie,m_ImMovie->Picture->Height,
									  m_ImMovie->Picture->Width,Timer2,bStyle);
   m_MovieForm->Show();

   Timer2->Interval = m_iPreviewInt;
   if(!m_bMovieControl)
   {
	Timer2->Enabled = true;
   }
   else
   {
	Timer2->Enabled = false;

   }

}
//---------------------------------------------------------------------------
void TEasyPlotForm::getDirectoryTag()
{

  int cnt = 0;
  strcpy(m_sDirTag,"");
  //for(int i=strlen(m_sMovieTargetRoot);i>=0;i--)
  for(int i=wcslen(m_swMovieTargetRoot);i>=0;i--)
  {
	if(m_swMovieTargetRoot[i] != '\\')
	{
	   cnt++;
	}
	else
	{
	  break;
	}

  }
  //int len = strlen(m_sMovieTargetRoot)-cnt;
  int len = wcslen(m_swMovieTargetRoot)-cnt;
  for(int i=0;i<cnt;i++)
  {
	  m_sDirTag[i] = m_swMovieTargetRoot[len+1+i];
  }


}//getDirectoryTag



void __fastcall TEasyPlotForm::Timer2Timer(TObject *Sender)
{


   int intvar, index;
   char tmp[20], filename[200], fStr[20];

   m_bProgrammedControl = true; //Set here temporarily

   //Get the next preview slide:
   if(m_ImMovie != NULL)
   {
	 delete m_ImMovie;
	 m_ImMovie = new TImage(this);
   }

   m_iPreviewIndex++;
   itoa(m_iPreviewIndex,tmp,10);
   sprintf(fStr,"%s%s",m_sMovieTag,tmp);
   itoa(m_iDirCnt,tmp,10);
   sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

   FILE *fp;
   if( (fp=fopen(filename,"rb")) == NULL )
   {
		 //Find next set of stills in next directory:
		 m_iDirCnt++;
		 itoa(m_iDirCnt,tmp,10);
		 char dir[200];
		 sprintf(dir,"%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
		 if(DirectoryExists(dir))
		 {
			m_iPreviewIndex = 0;
			return;
		 }
		 else
		 {
			//No more directories available - end process:
			Timer2->Enabled = false;
			StatusBar1->Panels->Items[0]->Text = "Preview mode";
			StatusBar1->Panels->Items[1]->Text = "Preview completed";
			return;

		 }

   }
   fclose(fp);
   m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
	m_MovieForm->Update(m_ImMovie,m_ImMovie->Picture->Height,
					   m_ImMovie->Picture->Width);

	char mes[200];
	itoa(m_iPreviewIndex,tmp,10);
	sprintf(mes,"%s%s%s","Previewing movie still  ",tmp," ..........");
	StatusBar1->Panels->Items[1]->Text = mes;

	if(m_bMovieControl && m_bProgrammedControl)
	{
		if(checkForPrompt()) //Check for programmed pause control points
		{
		   Timer2->Enabled = false;

		}//checkForPrompt()
	}


}

bool TEasyPlotForm::checkForPrompt()
{

   bool contflg = false;
   //Check that a control index list exsists:
   if(m_SLContIndex != NULL)
   {

	   for(int i=0;i<m_SLContIndex->Count;i++)
	   {
		 if(m_iPreviewIndex == m_SLContIndex->Strings[i])
		 {
			contflg = true;
			break;
		 }

	   }//i
   }
   return contflg;

}//checkForPrompt


void __fastcall TEasyPlotForm::SelPrevBtnClick(TObject *Sender)
{
   
   int intvar, index;
   char tmp[20], filename[200], fStr[20];

  //Read in slide and get dimensions:
  //Set up required image canvas:
  if(m_ImMovie == NULL)
  {
	m_ImMovie = new TImage(this);
  }

  //Get the directory tag:
  getDirectoryTag();


  m_iPreviewIndex = m_iPreviewIndexStart;

  itoa(m_iPreviewIndex,tmp,10);
  sprintf(fStr,"%s%s",m_sMovieTag,tmp);
  m_iDirCnt = 1;
  itoa(m_iDirCnt,tmp,10);
  sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

  FILE *fp;
  if( (fp=fopen(filename,"rb")) == NULL )
  {
#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open specified files.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  return;
#else
      MessageBox(NULL,"Cannot open specified files.",BEST_MESSAGE,MB_ICONEXCLAMATION);
	  return;
#endif

  }
  fclose(fp);

  StatusBar1->Panels->Items[0]->Text = "Preview mode";
  StatusBar1->Panels->Items[1]->Text = "";
  /*
  if(m_bDisableGraphMenu)
  {
	   //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
  }
  */

  m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
   TBorderStyle bStyle;
   if(m_bDisableGraphMenu)
   {
	   //Removes top border area
	   bStyle = bsNone;
   }
   else
   {
	 bStyle = bsSizeable;
   }
   m_MovieForm = new TMovieForm(this,m_ImMovie,m_ImMovie->Picture->Height,
									  m_ImMovie->Picture->Width,Timer2,bStyle);
   m_MovieForm->Show();

   Timer2->Interval = m_iPreviewInt;
   if(!m_bMovieControl)
   {
	Timer2->Enabled = true;
   }
   else
   {
	Timer2->Enabled = false;

   }


}


//---------------------------------------------------------------------------
#else      //LINUX INTEGRATED CODE STARTS HERE
//---------------------------------------------------------------------------
void __fastcall TEasyPlotForm::animateBtnClick(TObject *Sender)
{

   //Run the plot code:

   //Dialog box to choose 'program file':
  //View requested wavefile:
   char old_dir[MAXPATH],new_dir[MAXPATH];
   char fileName[200];

   //Get current directory:
   strcpy(old_dir,"X:\\");
   old_dir[0] = 'A' + getdisk();
   getcurdir(0,old_dir+3);
   strcpy(m_sOldDir,old_dir);

   int endWordPos;
   char tmp1[10];

   strcpy(tmp1,"");


   //OpenDialog1->Filter = "wav;dat;txt;csv";
   OpenDialog1->Filter =  "Tag program files (*.tag)|*.TAG|Def files (*.def)|*.DEF";
   OpenDialog1->Title = "Select required animation program file ...";
   OpenDialog1->FileName = "";
   OpenDialog1->Options << ofHideReadOnly << ofEnableSizing << ofAllowMultiSelect;
   OpenDialog1->FilterIndex = 2; //Show  DEF files first
   if( OpenDialog1->Execute())
   {

	  if(OpenDialog1->Files->Count == 1 )
	  {
		 //Process for only one .tag or one .def file
		 if(OpenDialog1->FilterIndex == 1)
		 {
			//Process a single .tag file
			char str[200];
#ifdef WIDE_CHAR_APP
			wcstombs(str, (OpenDialog1->FileName).c_str(), 199);
#else
            strcpy(str, (OpenDialog1->FileName).c_str());
#endif
			//strcpy(m_sTagFile,(OpenDialog1->FileName).c_str());
			strcpy(m_sTagFile,str);
			m_iTagFileCount = 1;

		 }
		 if(OpenDialog1->FilterIndex == 2)
		 {

		   char str[200];
#ifdef WIDE_CHAR_APP
		   wcstombs(str, (OpenDialog1->FileName).c_str(), 199);
#else
           strcpy(str, (OpenDialog1->FileName).c_str());
#endif
		   strcpy(m_sDefFile,str);

		   //Set up the tag file list from control (DEF) file:
		   parseDefFile();
		 }

	  }
	  else
	  {
		 //Check that multi fils are of .tsg type (.def type multi choice invalid)
		 if(OpenDialog1->FilterIndex == 2)
		 {
#ifdef WIDE_CHAR_APP
		   String str = "BEST_MESSAGE";
		   wchar_t mes[] = L"Multi Selection for a .DEF file is invalid\nPlease try again.";
		   wchar_t cap[100];
		   StringToWideChar(str,cap,99);

		   MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
		   return;
#else
           MessageBox(NULL,"Multi Selection for a .DEF file is invalid\nPlease try again.",
           BEST_MESSAGE,MB_ICONINFORMATION);
		   return;
#endif

		 }

		 //Multi tagfile select:
		 m_bMultiTagFiles = true;
		 if(m_SLTagFiles == NULL)
		 {
			m_SLTagFiles = new TStringList();

		 }
		 m_iTagFileCount = 0;
		 for(int i=0;i<OpenDialog1->Files->Count;i++)
		 {
			m_SLTagFiles->Add(OpenDialog1->Files->Strings[i]);
			m_iTagFileCount++;
		 }


		 //Reorder stringlist to ensure entries are in alphabetical order:
		 int index= 1;
		 char numStr[10];
		 itoa(index,numStr,10);
		 strcat(numStr,".tag");
		 TStringList *tempList = new TStringList();
		 //Initialise temp string:
		 for(int i=0;i<m_iTagFileCount;i++)
		 {
			tempList->Add("");
		 }

		 bool processing = true;

		 //Check files are numbered correctly (1...n in steps of 1)
		 bool match = false;
		 while(processing)
		 {
			for(int i=0;i<m_iTagFileCount;i++)
			{
			   char str[200];
#ifdef WIDE_CHAR_APP
			   wcstombs(str, m_SLTagFiles->Strings[i].c_str(), 199);
#else
               strcpy(str, m_SLTagFiles->Strings[i].c_str());
#endif

			   //if( strstr(m_SLTagFiles->Strings[i].c_str(),numStr) != NULL )
			   if( strstr(str,numStr) != NULL )
			   {
				 match = true;
				 tempList->Strings[index-1] = m_SLTagFiles->Strings[i];
				 break;
			   }
			}//i

			if(!match)
			{
			   //char mes[200];
			   //sprintf(mes,"%s%s","Tag files not numbered in correct sequence.\n",
			   //		   "Please try again.");
#ifdef WIDE_CHAR_APP
			   String str = "BEST_MESSAGE";
			   wchar_t mes[] = L"Tag files not numbered in correct sequence.\nPlease try again.";
			   wchar_t cap[100];
			   StringToWideChar(str,cap,99);

			   MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
			   return;
#else
               MessageBox(NULL,"Tag files not numbered in correct sequence.\nPlease try again.",
                          BEST_MESSAGE,MB_ICONINFORMATION);
			   return;
#endif

			}
			else
			{
			   index++;
			   itoa(index,numStr,10);
			   strcat(numStr,".tag");
			   match = false;
			}
			if(index==m_iTagFileCount+1)
			{
			   processing = false;
			}

		 }//while


		 for(int i=0;i<m_iTagFileCount;i++)
		 {
			m_SLTagFiles->Strings[i] = tempList->Strings[i];

			String str = "BEST_MESSAGE";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			//MessageBox(NULL,m_SLTagFiles->Strings[i].c_str(),cap,0);
		 }//i

		 //Assign first tsg file name:
		 char str[200];
#ifdef WIDE_CHAR_APP
		 wcstombs(str,m_SLTagFiles->Strings[0].c_str() , 199);
#else
         strcpy(str,m_SLTagFiles->Strings[0].c_str());
#endif
		 //strcpy(m_sTagFile,m_SLTagFiles->Strings[0].c_str());
		 strcpy(m_sTagFile,str);
	  }


	  setUpSubDir();
	  /*
	  //Create required subdir for target files:
	  char tmp[10];

	  getDirectoryTag();
	  m_iDirCnt = 1;
	  m_iAniCumCnt = 1;
	 itoa(m_iDirCnt,tmp,10);
	 char dir[200];
	 sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
	 strcpy(m_sMovieTarget,dir);
	 if(!DirectoryExists(m_sMovieTarget))
	 {
		if( mkdir(m_sMovieTarget) == -1 )
		{
			String str = "BEST_MESSAGE";
			wchar_t mes[] = L"Problem creating target sub-directory.";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);

		}
	 }

	  //Change back to original directory:
	  if(chdir(old_dir) == -1)
	  {

		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
	  }
	  */
   }
   else
   {
	  //Change back to original directory:
	  if(chdir(old_dir) == -1)
	  {
#ifdef WIDE_CHAR_APP
		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
        MessageBox(NULL,"Problem changing to old dir!",BEST_MESSAGE,
                   MB_ICONEXCLAMATION);
#endif
	  }
	  return;
   }

   //Initialise flag for <VAR> section
   m_bVARPresent = true;
   //Parse the animation program file
   //parseTagFile();
   parseTagFile1();

   //Initialise variables for horizontal/vertical ref lines:
	m_fHPos = 0.0;
	m_fVPos = 0.0;

	//Delete any previous instance of graphFrm:
	if(m_GraphFrm != NULL)
	{
	   delete m_GraphFrm;
	   m_GraphFrm = NULL;
	}


   //Launch the animation production process:
   m_bAnimation = true;
   m_iAniCnt = 1;
   m_iAniCumCnt = 1;
   m_iMaxTotAniCnt = 0;
   m_bStartTimer1 = true;
   Timer1->Enabled = true;

}//animateBtnCLick


void TEasyPlotForm::setUpSubDir()
{

      //Create required subdir for target files:
	  char tmp[10];

	  getDirectoryTag();
	  m_iDirCnt = 1;
	  m_iAniCumCnt = 1;
	 itoa(m_iDirCnt,tmp,10);
	 char dir[200];
	 sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
	 strcpy(m_sMovieTarget,dir);
	 if(!DirectoryExists(m_sMovieTarget))
	 {
		if( mkdir(m_sMovieTarget) == -1 )
		{
#ifdef WIDE_CHAR_APP
			String str = "BEST_MESSAGE";
			wchar_t mes[] = L"Problem creating target sub-directory.";
			wchar_t cap[100];
			StringToWideChar(str,cap,99);

			MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
            MessageBox(NULL,"Problem creating target sub-directory.",
                       BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif


		}
	 }

	  //Change back to original directory:
	  //if(chdir(old_dir) == -1)
	  if(chdir(m_sOldDir) == -1)
	  {
#ifdef WIDE_CHAR_APP

		 String str = "BEST_MESSAGE";
		 wchar_t mes[] = L"Problem changing to old dir!";
		 wchar_t cap[100];
		 StringToWideChar(str,cap,99);

		 MessageBox(NULL,mes,cap,0);
#else
         MessageBox(NULL,"Problem changing to old dir!",
                       BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif

	  }


}//setUpSubDir

//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::File1Click(TObject *Sender)
{
   //TBD
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Close1Click(TObject *Sender)
{
   LPCTSTR path = "Help\\easyplotter.hlp";
   WinHelp(Handle,path,HELP_QUIT,0);
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::HelpTopics1Click(TObject *Sender)
{
   //Launch EasyPlotter Help
   //APPLaunch app;

   LPCTSTR path = "Help\\easyplotter.hlp";
   WinHelp(Handle,path,HELP_FINDER,0);

}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Open1Click(TObject *Sender)
{
   Button4Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TEasyPlotForm::Timer1Timer(TObject *Sender)
{
  char mes[200],index[10];

   //Timer control for animation effects:

   //Close the previous graph

   if(m_GraphFrm != NULL)
   {


	  //Message to status bar:
	  if(!m_bOverride)
	  {
		itoa(m_iAniCnt,index,10);
	  }
	  else
	  {
		itoa(m_iAniCumCnt,index,10);
	  }

	  sprintf(mes,"%s%s%s","Processing movie still  ",index," ..........");
	  StatusBar1->Panels->Items[1]->Text = mes;

	  //Produce bitmap of current graph:
	  char filename[200],tmp[10];
	  if(!m_bOverride)
	  {
		itoa(m_iAniCnt,tmp,10);

	  }
	  else
	  {
		itoa(m_iAniCumCnt,tmp,10);
	  }
	  sprintf(filename,"%s%s%s%s%s",m_sMovieTarget,"\\",m_sMovieTag,tmp,".bmp");
	  m_GraphFrm->getImage()->Picture->SaveToFile(filename);

	  //Delete graph instance:
	  delete m_GraphFrm;
	  m_GraphFrm = NULL;

	  m_iAniCnt++;
	  m_iAniCumCnt++;

	  //if(m_iTagFileCount == m_iDirCnt)
	  if(m_iDirCnt > m_iTagFileCount)
	  {
		 Timer1->Enabled = false;
		 m_bAnimation = false;
		 //Message to status bar:
		 sprintf(mes,"%s","Processing of movie stills has been completed!");
		 StatusBar1->Panels->Items[1]->Text = mes;
		 //MessageBox(NULL,"Movie stills have been completed.",BEST_MESSAGE,MB_ICONINFORMATION);
		 m_bFileIgnore = false;
		 return;
	  }

   }

   //Launch plot routine
	PlotBtnClick(Sender);
	if(m_bStartTimer1)
	{
	   m_bStartTimer1 = false;
	   //m_iAniCnt++;
	   //m_iAniCumCnt++;
	}

   //Set required no of points to plot and keep record
   //if(m_iAniCnt > m_iTotAniCnt)

   if(m_iAniCnt > m_iMaxTotAniCnt)
   {
	  //Perform any other animations required here:
	  if(m_iTagFileCount == m_iDirCnt)
	  {
		  /*
		  Timer1->Enabled = false;
		  m_bAnimation = false;
		 //Message to status bar:
		 sprintf(mes,"%s","Processing of movie stills has been completed!");
		 StatusBar1->Panels->Items[1]->Text = mes;
		 //MessageBox(NULL,"Movie stills have been completed.",BEST_MESSAGE,MB_ICONINFORMATION);
		 m_bFileIgnore = false;
		 */
		 m_iDirCnt++;
	  }
	  else
	  {
			 //Prepare for next program tag file and create next sub dir:
			 char tmp[10];

			 getDirectoryTag();
			 m_iDirCnt++;
			itoa(m_iDirCnt,tmp,10);
			if(!m_bOverride)
			{
				char dir[200];
				sprintf(dir,"%s%s%s%s",m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
				strcpy(m_sMovieTarget,dir);
				if(!DirectoryExists(m_sMovieTarget))
				{
				   if( mkdir(dir) == -1 )
				   {
#ifdef WIDE_CHAR_APP
					   String str = "BEST_MESSAGE";
					   wchar_t mes[] = L"Problem creating target sub-directory.";
					   wchar_t cap[100];
					   StringToWideChar(str,cap,99);

					   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
#else
                       MessageBox(NULL,"Problem creating target sub-directory.",
                                  BEST_MESSAGE,MB_ICONEXCLAMATION);
#endif

				   }
				}
			}
			//Initialise flag for <VAR> section
			 m_bVARPresent = true;
			 m_iMaxTotAniCnt = 0;
			 //Parse the animation program file
			 //Get new tag file name:

			 //wchar_t wstr[100] = L"hello world";
			 char str[200];
#ifdef WIDE_CHAR_APP
			 wcstombs(str, m_SLTagFiles->Strings[m_iDirCnt-1].c_str(),199);
#else
             strcpy(str, m_SLTagFiles->Strings[m_iDirCnt-1].c_str());
#endif
			 //strcpy(m_sTagFile,m_SLTagFiles->Strings[m_iDirCnt-1].c_str());
			 strcpy(m_sTagFile,str);
			 //parseTagFile();
			 parseTagFile1();

			 //Initialise variables for horizontal/vertical ref lines:
			  m_fHPos = 0.0;
			  m_fVPos = 0.0;

			 m_iAniCnt = 0;//1;

	  }
   }



}
//---------------------------------------------------------------------------


void TEasyPlotForm::parseTagFile1()
{
   
   char contents[9000];
   char ch[2];

   m_bProcFIXED = false;
   m_bProcVAR = false;
   bool m_bValid = true;

   if( (m_fpTagFile=fopen(m_sTagFile,"rt")) == NULL)
   {
	  //char mes[200];
	  //sprintf(mes,"%s%s","Cannot open required Animation Program File.\n",
	  //		  "Please try again.");
#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open required Animation Program File.\nPlease try again.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  //return;
	  exit(1);
#else
      MessageBox(NULL,"Cannot open required Animation Program File.\nPlease try again.",
                 BEST_MESSAGE,MB_ICONEXCLAMATION);
	  //return;
	  exit(1);
#endif
   }
   //Read contents of file:
   int byteCnt = 0;
   while( !feof(m_fpTagFile))
   {
	  fread(ch,sizeof(char),1,m_fpTagFile);
	  contents[byteCnt] = ch[0];
	  //contents++;
	 byteCnt++;
   }
   contents[byteCnt] = '\0';
   fclose(m_fpTagFile);

   //Search for <FIXED> section
   m_bProcFIXED = true;
   m_iNumFIXEDPlots = 0;
   char *fixed_ptr1 = NULL;
   char *fixed_ptr2 = NULL;
   char fixed_str[1000];
   if( strstr(contents,"<FIXED>") != NULL)
   {
	  //Process <FIXED> animation instructions:
	  fixed_ptr1 = strstr(contents,"<FIXED>");
	  fixed_ptr2 = strstr(contents,"</FIXED>");
	  int len = strlen(fixed_ptr1) - strlen(fixed_ptr2);
	  strcpy(fixed_str,fixed_ptr1);
	  fixed_str[len-1] = '\0';


	  //Parsing the <FIXED> instructions


	  int fixedCnt = 0;
	  bool plotExcess = false;
	  for(int i=0;i<m_iNumPlotTypes;i++)
	  {
		 char *p = NULL;
		 p= fixed_str;
		 if(plotExcess) break;

		  while( strstr(p,m_sPLOTTYPES[i]) != NULL )
		 {
			strcpy(m_sFIXED[fixedCnt],m_sPLOTTYPES[i]);
			p = strstr(p,m_sFIXED[fixedCnt]);
			//Get the argument list:
			//if( !getArgList(p,fixedCnt) )
			if( !getArgList1(p,fixedCnt) )
			{
			   //Problem with argument list:
			   return;
			}

			//Advance to end of parameter list:
			p = strstr(p,";");
			fixedCnt++;
			if(fixedCnt == MAX_NUM_FIXEDPLOTS)
			{
			   plotExcess = true;
			   m_iNumFIXEDPlots = MAX_NUM_FIXEDPLOTS;
			}
			else
			{
			  m_iNumFIXEDPlots = fixedCnt;
			}
			if(p==NULL) break;
		 }//while

	  }//i


   }//<FIXED>



   if( strstr(contents,"<VAR>") == NULL)
   {

	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Warning: A <VAR> section has not been defined in tag file.\nOK.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  //MessageBox(NULL,mes,cap,MB_ICONINFORMATION);
	  m_bVARPresent = false;
	  m_iTotAniCnt = 1;  //Prevent stills series production:
	  return;
   }
   else
   {
		//Search for <VAR> section
	   m_bProcVAR = true;
	   m_bProcFIXED = false;
	   m_iNumVARPlots = 0;
	   char *var_ptr1 = NULL;
	   char *var_ptr2 = NULL;
	   char var_str[1000];
	   if( strstr(contents,"<VAR>") != NULL)
	   {
		  //Process <VAR> animation instructions:
		  var_ptr1 = strstr(contents,"<VAR>");
		  var_ptr2 = strstr(contents,"</VAR>");
		  int len = strlen(var_ptr1) - strlen(var_ptr2);
		  strcpy(var_str,var_ptr1);
		  var_str[len-1] = '\0';


		  //Parsing the <VAR> instructions
		  int varCnt = 0;
		  bool plotExcess = false;
		  for(int i=0;i<m_iNumPlotTypes;i++)
		  {
			 char *p = NULL;
			 p= var_str;
			 if(plotExcess) break;

			  while( strstr(p,m_sPLOTTYPES[i]) != NULL )
			 {
				strcpy(m_sVAR[varCnt],m_sPLOTTYPES[i]);
				p = strstr(p,m_sVAR[varCnt]);
				//Get the argument list:
				//if( !getArgList(p,fixedCnt) )
				if( !getArgList1(p,varCnt) )
				{
				   //Problem with argument list:
				   return;
				}
				m_bValid = true;
				//Advance to end of parameter list:
				p = strstr(p,";");
				varCnt++;
				if(varCnt == MAX_NUM_VARPLOTS)
				{
				   plotExcess = true;
				   m_iNumVARPlots = MAX_NUM_VARPLOTS;
				}
				else
				{
				  m_iNumVARPlots = varCnt;
				}
				if(p==NULL) break;
			 }//while

		  }//i

	   }

   }//else (VAR)

   if(!m_bValid)
  {
	 //char mes[200];
	 //sprintf(mes,"%s%s","A valid plot instruction has not been defined in tag file.\n",
	 //		"                     OK ");

#ifdef WIDE_CHAR_APP
	 String str = "BEST_MESSAGE";
	 wchar_t mes[] = L"A valid plot instruction has not been defined in tag file.\nOK ";
	 wchar_t cap[100];
	 StringToWideChar(str,cap,99);

	 MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	 return;
#else
     MessageBox(NULL,"A valid plot instruction has not been defined in tag file.\nOK ",
                BEST_MESSAGE,MB_ICONEXCLAMATION);
	 return;
#endif
  }

}//parseTagFile()

bool TEasyPlotForm::getArgList1(char *p,int index)
{

   char ch[2],tmp[100];

   //Check that an end of parameter list delimiter is present
   if( strstr(p,";") == NULL)
   {
	   //char mes[200];
	   //sprintf(mes,"%s%s","An end of argument list delimiter is not present.\n",
	   //		  "Please edit the program file.");

#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"An end of argument list delimiter is not present.\nPlease edit the program file. ";;
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);

	  return false;
#else
      MessageBox(NULL,"An end of argument list delimiter is not present.\nPlease edit the program file. "
                ,BEST_MESSAGE,MB_ICONEXCLAMATION);

	  return false;
#endif

   }


   //if(m_bProcVAR)
   //{

	  //Set pointer and initialise argument list:
	  if(m_bProcVAR)
	  {
		 //for(int k=0;k<strlen(m_sVARSingle);k++) p++;
		 //for(int i=0;i<MAX_ARGS;i++) strcpy(m_sVARSingleParams[i],"");
		 for(int k=0;k<strlen(m_sVAR[index]);k++) p++;
		 for(int i=0;i<MAX_ARGS;i++) strcpy(m_sVARParams[index][i],"");
	  }
	  else if(m_bProcFIXED)
	  {
		 for(int k=0;k<strlen(m_sFIXED[index]);k++) p++;
		 for(int i=0;i<MAX_ARGS;i++) strcpy(m_sFIXEDParams[index][i],"");
	  }

	  bool arg_parsing = true;

	  strcpy(tmp,"");
	  int argCnt = 0;
	  int charCnt = 0;
	  while (arg_parsing)
	  {
		 ch[0] = p[charCnt];
		 ch[1] = '\0';
		 if( (ch[0] != ',') )
		 {
			if( ch[0] != ';')
			{
			   strcat(tmp,ch);
			   charCnt++;
			}
			else
			{
			   if(m_bProcVAR)
			   {
				  strcpy(m_sVARParams[index][argCnt],tmp);
			   }
			   else if(m_bProcFIXED)
			   {
				  strcpy(m_sFIXEDParams[index][argCnt],tmp);
			   }
			   //MessageBox(NULL,m_sVARParams[argCnt],"",0);
			}
		 }
		 else
		 {
			if(m_bProcVAR)
			{
			   strcpy(m_sVARParams[index][argCnt],tmp);
			}
			else if(m_bProcFIXED)
			{
			   strcpy(m_sFIXEDParams[index][argCnt],tmp);
			}
			//MessageBox(NULL,m_sVARParams[argCnt],"",0);
			strcpy(tmp,"");
			argCnt++;
			charCnt++;
		 }

		 //Terminate parsing
		 if(ch[0] == ';')
		 {
			arg_parsing = false;
			//Define final argument count:
			m_iNumArgs = argCnt+1;
			if(m_bProcFIXED)
			{
				m_iNumFIXEDPlotArgs[index] = m_iNumArgs;
			}
			if(m_bProcVAR)
			{
				m_iNumVARPlotArgs[index] = m_iNumArgs;
			}
		 }


	  }//while
   //}

   return true;

}//getArgList


void __fastcall TEasyPlotForm::previewBtnClick(TObject *Sender)
{

   int intvar, index;
   char tmp[20], filename[200], fStr[20];

  //Read in slide and get dimensions:
  //Set up required image canvas:
  if(m_ImMovie == NULL)
  {
	m_ImMovie = new TImage(this);
  }

  //Get the directory tag:
  getDirectoryTag();

  m_iPreviewIndex = m_iPreviewIndexStart;
  itoa(m_iPreviewIndex,tmp,10);
  sprintf(fStr,"%s%s",m_sMovieTag,tmp);
  m_iDirCnt = 1;
  itoa(m_iDirCnt,tmp,10);
  sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

  FILE *fp;
  if( (fp=fopen(filename,"rb")) == NULL )
  {

#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open specified files.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);
      MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  return;
#else
      MessageBox(NULL,"Cannot open specified files.",BEST_MESSAGE,MB_ICONEXCLAMATION);
	  return;
#endif

  }
  fclose(fp);

  StatusBar1->Panels->Items[0]->Text = "Preview mode";
  StatusBar1->Panels->Items[1]->Text = "";

  /*
  if(m_bDisableGraphMenu)
  {
	   //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
  }
  */

  m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
   TBorderStyle bStyle;
   if(m_bDisableGraphMenu)
   {
	   //Removes top border area
	   bStyle = bsNone;
   }
   else
   {
	 bStyle = bsSizeable;
   }
   m_MovieForm = new TMovieForm(this,m_ImMovie,m_ImMovie->Picture->Height,
									  m_ImMovie->Picture->Width,Timer2,bStyle);
   m_MovieForm->Show();

   Timer2->Interval = m_iPreviewInt;
   if(!m_bMovieControl)
   {
	Timer2->Enabled = true;
   }
   else
   {
	Timer2->Enabled = false;

   }

}
//---------------------------------------------------------------------------
void TEasyPlotForm::getDirectoryTag()
{
  /*
   int cnt = 0;
  strcpy(m_sDirTag,"");
  for(int i=strlen(m_sMovieTargetRoot);i>=0;i--)
  {
    if(m_sMovieTargetRoot[i] != '\\')
    {
       cnt++;
    }
    else
    {
      break;
    }

  }
  int len = strlen(m_sMovieTargetRoot)-cnt;
  for(int i=0;i<cnt;i++)
  {
      m_sDirTag[i] = m_sMovieTargetRoot[len+1+i];
  }
  */

   int cnt = 0;
  strcpy(m_sDirTag,"");
  //for(int i=strlen(m_sMovieTargetRoot);i>=0;i--)
  for(int i=wcslen(m_swMovieTargetRoot);i>=0;i--)
  {
	if(m_swMovieTargetRoot[i] != '\\')
	{
	   cnt++;
	}
	else
	{
	  break;
	}

  }
  //int len = strlen(m_sMovieTargetRoot)-cnt;
  int len = wcslen(m_swMovieTargetRoot)-cnt;
  for(int i=0;i<cnt;i++)
  {
	  m_sDirTag[i] = m_swMovieTargetRoot[len+1+i];
  }

}//getDirectoryTag



void __fastcall TEasyPlotForm::Timer2Timer(TObject *Sender)
{

   int intvar, index;
   char tmp[20], filename[200], fStr[20];

   m_bProgrammedControl = true; //Set here temporarily

   //Get the next preview slide:
   if(m_ImMovie != NULL)
   {
	 delete m_ImMovie;
	 m_ImMovie = new TImage(this);
   }

   m_iPreviewIndex++;
   itoa(m_iPreviewIndex,tmp,10);
   sprintf(fStr,"%s%s",m_sMovieTag,tmp);
   itoa(m_iDirCnt,tmp,10);
   sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

   FILE *fp;
   if( (fp=fopen(filename,"rb")) == NULL )
   {
		 //Find next set of stills in next directory:
		 m_iDirCnt++;
		 itoa(m_iDirCnt,tmp,10);
		 char dir[200];
		 sprintf(dir,"%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp);
		 if(DirectoryExists(dir))
		 {
			m_iPreviewIndex = 0;
			return;
		 }
		 else
		 {
			//No more directories available - end process:
			Timer2->Enabled = false;
			StatusBar1->Panels->Items[0]->Text = "Preview mode";
			StatusBar1->Panels->Items[1]->Text = "Preview completed";
			return;

		 }

   }
   fclose(fp);
   m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
	m_MovieForm->Update(m_ImMovie,m_ImMovie->Picture->Height,
					   m_ImMovie->Picture->Width);

	char mes[200];
	itoa(m_iPreviewIndex,tmp,10);
	sprintf(mes,"%s%s%s","Previewing movie still  ",tmp," ..........");
	StatusBar1->Panels->Items[1]->Text = mes;

	if(m_bMovieControl && m_bProgrammedControl)
	{
		if(checkForPrompt()) //Check for programmed pause control points
		{
		   Timer2->Enabled = false;

		}//checkForPrompt()
	}

}


bool TEasyPlotForm::checkForPrompt()
{

   bool contflg = false;
   //Check that a control index list exsists:
   if(m_SLContIndex != NULL)
   {

	   for(int i=0;i<m_SLContIndex->Count;i++)
	   {
		 if(m_iPreviewIndex == m_SLContIndex->Strings[i])
		 {
			contflg = true;
			break;
		 }

	   }//i
   }
   return contflg;

}//checkForPrompt


void __fastcall TEasyPlotForm::SelPrevBtnClick(TObject *Sender)
{
   int intvar, index;
   char tmp[20], filename[200], fStr[20];

  //Read in slide and get dimensions:
  //Set up required image canvas:
  if(m_ImMovie == NULL)
  {
	m_ImMovie = new TImage(this);
  }

  //Get the directory tag:
  getDirectoryTag();


  m_iPreviewIndex = m_iPreviewIndexStart;

  itoa(m_iPreviewIndex,tmp,10);
  sprintf(fStr,"%s%s",m_sMovieTag,tmp);
  m_iDirCnt = 1;
  itoa(m_iDirCnt,tmp,10);
  sprintf(filename,"%s%s%s%s%s%s%s", m_sMovieTargetRoot,"\\",m_sDirTag,tmp,"\\",fStr,".bmp");

  FILE *fp;
  if( (fp=fopen(filename,"rb")) == NULL )
  {
#ifdef WIDE_CHAR_APP
	  String str = "BEST_MESSAGE";
	  wchar_t mes[] = L"Cannot open specified files.";
	  wchar_t cap[100];
	  StringToWideChar(str,cap,99);

	  MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
	  return;
#else
      MessageBox(NULL,"Cannot open specified files.",BEST_MESSAGE,MB_ICONEXCLAMATION);
	  return;
#endif

  }
  fclose(fp);

  StatusBar1->Panels->Items[0]->Text = "Preview mode";
  StatusBar1->Panels->Items[1]->Text = "";
  /*
  if(m_bDisableGraphMenu)
  {
	   //Inform user that the DELETE button needs to be pressed to close the form;
	   String str = BEST_MESSAGE;
	   wchar_t mes[] = L"In this Graph Mode, the menu and top border controls are not available.\nYou may close the form by pressing the DELETE button.";
	   wchar_t cap[100];
	   StringToWideChar(str,cap,99);
	   MessageBox(NULL,mes,cap,MB_ICONEXCLAMATION);
  }
  */

  m_ImMovie->Picture->LoadFromFile(filename);

   //Preview slide:
   TBorderStyle bStyle;
   if(m_bDisableGraphMenu)
   {
	   //Removes top border area
	   bStyle = bsNone;
   }
   else
   {
	 bStyle = bsSizeable;
   }
   m_MovieForm = new TMovieForm(this,m_ImMovie,m_ImMovie->Picture->Height,
									  m_ImMovie->Picture->Width,Timer2,bStyle);
   m_MovieForm->Show();

   Timer2->Interval = m_iPreviewInt;
   if(!m_bMovieControl)
   {
	Timer2->Enabled = true;
   }
   else
   {
	Timer2->Enabled = false;

   }

}
//---------------------------------------------------------------------------

#endif

