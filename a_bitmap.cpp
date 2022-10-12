/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_BitMap
					  Derived class for animating bitmap images.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_bitmap.hpp"
//---------------------------------------------------------------------------
A_BitMap::A_BitMap(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
			  :A_Shape(plotProc,plotType,gph,args,aCnt)
{




}//A_Bitmap



void A_BitMap::left2right()
{
   float posx,posy;

   //Get arguments:
   if(m_bProcVAR)
   {

	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
	  posy = atof(m_vArgs[3].c_str());


   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  posx = atof(m_vArgs[0].c_str());
	  posy = atof(m_vArgs[3].c_str());

   }


   //Draw bitmap to canvas
   m_Graph->drawBitmap(1,posx,posy, m_vArgs[4]);



}//left2right

void A_BitMap::upDown()
{
   float posx,posy;


   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  posy = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
	  posx = atof(m_vArgs[3].c_str());

   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  posy = atof(m_vArgs[0].c_str());
	  posx = atof(m_vArgs[3].c_str());

   }

   //Draw the bitmap to canvas:
   m_Graph->drawBitmap(1,posx,posy, m_vArgs[4]);



}//upDown

void A_BitMap::unconstrained()
{
	//Animation for unconstrained trajectories - can change along x  and y axes
	float posx,posy;

   //Get arguments:
   if(m_bProcVAR)
   {

	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }

	  if(m_iAniCnt > m_iCntX)
	  {
		posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iCntX-1)*atof(m_vArgs[2].c_str());
	  }
	  else
	  {
		posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
	  }

	  if(m_iAniCnt > m_iCntY)
	  {
		posy = atof(m_vArgs[3].c_str()) + m_iSign*(m_iCntY-1)*atof(m_vArgs[5].c_str());
	  }
	  else
	  {
		posy = atof(m_vArgs[3].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[5].c_str());

	  }

   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  posx = atof(m_vArgs[0].c_str());
	  posy = atof(m_vArgs[3].c_str());

   }


   //Draw bitmap to canvas
   m_Graph->drawBitmap(1,posx,posy, m_vArgs[6]);


}//unconstrained


void A_BitMap::upDownChangeSize()
{

	//This could be used for final credit captios at end of movie
	//Change size whilst moving up/down:

	//TBD

   /*
   float posx,posy;
   int size1,size2;
   char text[100];

   size1 = atoi(m_vArgs[5].c_str());
   size2 = atoi(m_vArgs[9].c_str());

   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  posy = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
	  posx = atof(m_vArgs[3].c_str());

	 //Get text:
	 strcpy(text,m_vArgs[4].c_str());


	 //Set font size:
	 int size = size1 + (m_iAniCnt-1)*atoi(m_vArgs[10].c_str());
	 if(size < size2 )
	 {
		m_Graph->getImage()->Canvas->Font->Size = size;
	 }
	 else
	 {
		m_Graph->getImage()->Canvas->Font->Size = size2;
	 }

	 //Set font
	 m_Graph->getImage()->Canvas->Font->Name = m_vArgs[6].c_str();
	 //Change to required color:
	 m_vArgs[7][0] = toupper(m_vArgs[7][0]);
	 //Set color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[7].c_str());
	 //Set text background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[8].c_str());

   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  posy = atof(m_vArgs[0].c_str());
	  posx = atof(m_vArgs[3].c_str());


	 //Get text:
	 strcpy(text,m_vArgs[4].c_str());
	 //Set font size:
	 m_Graph->getImage()->Canvas->Font->Size = atoi(m_vArgs[5].c_str());
	 //Set font
	 m_Graph->getImage()->Canvas->Font->Name = m_vArgs[6].c_str();
	 //Change to required color:
	 m_vArgs[7][0] = toupper(m_vArgs[7][0]);
	 //Set color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[7].c_str());
	 //Set text background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[8].c_str());


   }

  m_Graph->setPenColor();
  m_Graph->text(1,posx,posy,text);
  */


}//upDownChangeSize


void A_BitMap::leftRightChangeSize()
{

}//leftRightChangeSize


void A_BitMap::rotate()
{

}//rotate

void A_BitMap::fadeIn()
{

}//fadeIn

void A_BitMap::fadeOut()
{


}//fadeOut


int A_BitMap::movieStillCnt()
{

  if(m_bProcVAR)
  {

	   if( (strcmp(m_sPlotType,"BITMAPxy") == NULL) ) //unconstrained method will be called
	   {

			m_iSign = 1;
			m_iCntX = (atof( m_vArgs[1].c_str() ) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[2].c_str());
			m_iCntY = (atof( m_vArgs[4].c_str() ) - atof(m_vArgs[3].c_str()))/atof(m_vArgs[5].c_str());
			m_iTotAniCnt = m_iCntX;
			if(m_iCntY > m_iCntX)
			{
			   m_iTotAniCnt = m_iCntX;
			}

	   }
	   else
	   {
			m_iSign = 1;
			m_iTotAniCnt = (atof( m_vArgs[1].c_str() ) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[2].c_str());

	   }

	   if(m_iTotAniCnt < 0 )
	   {
		 m_iTotAniCnt = -1.0*m_iTotAniCnt;
		 m_iSign = -1;
	   }


  }


  if(m_bProcFIXED)
  {
	  m_iTotAniCnt = 1;
	  //m_iTotAniCnt++; //add one to get final slide with end position
  }


  return m_iTotAniCnt;

}//movieStillCnt()










