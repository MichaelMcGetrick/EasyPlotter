/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_BarPlot
					  Derived class for animating bar plots.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_barplot.hpp"
//---------------------------------------------------------------------------
A_BarPlot::A_BarPlot(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
			  :A_Shape(plotProc,plotType,gph,args,aCnt)
{


}//A_BarPlot



void A_BarPlot::left2right()
{


}//left2right

void A_BarPlot::upDown()
{

   //Plot a single bar element:
   float posy1,posy2,posx,width;
   TColor Color;

   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  posy2 = atof(m_vArgs[0].c_str()) + (m_iAniCnt-1)*atof(m_vArgs[2].c_str());
	  posy1 = atof(m_vArgs[0].c_str());
	  posx =  atof(m_vArgs[3].c_str());
	  width =  atoi(m_vArgs[4].c_str());

	  //Set the required brush color:
	  //m_vArgs[5][0] = toupper(m_vArgs[5][0]);
	  Color = setColor1(m_vArgs[5].c_str());

   }
   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  posy2 = atof(m_vArgs[1].c_str());
	  posy1 = atof(m_vArgs[0].c_str());
	  posx =  atof(m_vArgs[3].c_str());
	  width =  atoi(m_vArgs[4].c_str());

	  //Set the required brush color:
	  //m_vArgs[5][0] = toupper(m_vArgs[5][0]);
	  Color = setColor1(m_vArgs[5].c_str());

   }

   //Draw the bar:
   m_Graph->drawBar(posy1,posy2,posx,width,Color);


}//upDown


void A_BarPlot::unconstrained()
{
	//Animation for unconstrained trajectories - can change along x  and y axes


}//unconstrained


void A_BarPlot::upDownChangeSize()
{

}//expand

void A_BarPlot::leftRightChangeSize()
{

}//leftRightChangeSize


void A_BarPlot::rotate()
{

	//This can be implenented as a bar which circles around and enlarges
	// as it moves tound a central point - like a circular progress monitor

}//rotate

void A_BarPlot::fadeIn()
{

}//fadeIn

void A_BarPlot::fadeOut()
{


}//fadeOut


int A_BarPlot::movieStillCnt()
{
  if(m_bProcVAR)
  {
	  m_iSign = 1;
	  m_iTotAniCnt = (atof(m_vArgs[1].c_str()) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[2].c_str());

	  if(m_iTotAniCnt < 0 )
	  {
		 m_iTotAniCnt = -1.0*m_iTotAniCnt;
		 m_iSign = -1;
		 m_iTotAniCnt++; //add one to get final slide with end position
	  }
  }


  if(m_bProcFIXED)
  {
	  m_iTotAniCnt = 1;
	  //m_iTotAniCnt++; //add one to get final slide with end position
  }


  return m_iTotAniCnt;

}//movieStillCnt()










