/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Circle
					  Derived class for animating circles.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_circle.hpp"
//---------------------------------------------------------------------------

A_Circle::A_Circle(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
			  :A_Shape(plotProc,plotType,gph,args,aCnt)
{


}//A_Circle



void A_Circle::left2right()
{
   float posx,posy;
   char text[100];
   float radius;

   //Get arguments:

   if(m_bProcVAR)
   {

	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  radius = atof(m_vArgs[3].c_str());
	  posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[4].c_str());
	  posy = atof(m_vArgs[2].c_str());

	  //Set circle border color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[5].c_str());

	 //Set circle background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[5].c_str());



   }
   if(m_bProcFIXED)
   {
	  radius = atof(m_vArgs[3].c_str());
	  posx = atof(m_vArgs[0].c_str());
	  posy = atof(m_vArgs[2].c_str());


	   //Set circle border color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[5].c_str());

	  //Set circle background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[5].c_str());

   }

   m_Graph->setPenColor();
   ///Draw circle:
   m_Graph->circle(1,posx-radius,posy-radius,posx+radius,posy+radius);


}//left2right

void A_Circle::upDown()
{
	float posx,posy;
   char text[100];
   float radius;

   //Get arguments:

   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  radius = atof(m_vArgs[3].c_str());
	  posy = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[4].c_str());
	  posx = atof(m_vArgs[2].c_str());

	  //Set circle border color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[5].c_str());

	 //Set circle background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[5].c_str());



   }
   if(m_bProcFIXED)
   {
	  radius = atof(m_vArgs[3].c_str());
	  posy = atof(m_vArgs[0].c_str());
	  posx = atof(m_vArgs[2].c_str());


	   //Set circle border color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[5].c_str());

	  //Set circle background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[5].c_str());

   }

   m_Graph->setPenColor();
   ///Draw circle:
   m_Graph->circle(1,posx-radius,posy-radius,posx+radius,posy+radius);

}//upDown


void A_Circle::unconstrained()
{
	//Animation for unconstrained trajectories - can change along x  and y axes
   float posx,posy;
   char text[100];
   float radius;

   //Get arguments:

   if(m_bProcVAR)
   {

	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  radius = atof(m_vArgs[6].c_str());

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


	  //Set circle border color:
	  m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[7].c_str());

	  //Set circle background color
	  m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[7].c_str());



   }
   if(m_bProcFIXED)
   {
	  radius = atof(m_vArgs[6].c_str());
	  posx = atof(m_vArgs[0].c_str());
	  posy = atof(m_vArgs[3].c_str());


	   //Set circle border color:
	 m_Graph->getImage()->Canvas->Font->Color =  setColor(m_vArgs[7].c_str());

	  //Set circle background color
	 m_Graph->getImage()->Canvas->Brush->Color =  setColor1(m_vArgs[7].c_str());

   }

   m_Graph->setPenColor();
   ///Draw circle:
   m_Graph->circle(1,posx-radius,posy-radius,posx+radius,posy+radius);


}//unconstrained



void A_Circle::upDownChangeSize()
{

}//expand

void A_Circle::leftRightChangeSize()
{

}//leftRightChangeSize



void A_Circle::rotate()
{

}//rotate

void A_Circle::fadeIn()
{

}//fadeIn

void A_Circle::fadeOut()
{


}//fadeOut


int A_Circle::movieStillCnt()
{
  if(m_bProcVAR)
  {
	   m_iTotAniCnt = (atof(m_vArgs[1].c_str()) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[4].c_str());
	  if(m_iTotAniCnt < 0 )
	  {
		 m_iTotAniCnt = -1.0*m_iTotAniCnt;
		 m_iSign = -1;
	  }
  }


  if(m_bProcVAR)
  {

	   if( (strcmp(m_sPlotType,"CIRCLExy") == NULL) ) //unconstrained method will be called
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
			m_iTotAniCnt = (atof(m_vArgs[1].c_str()) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[4].c_str());

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










