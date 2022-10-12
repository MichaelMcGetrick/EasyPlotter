/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Line
					  Derived class for animating lines.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_line.hpp"
//---------------------------------------------------------------------------

A_Line::A_Line(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
			  :A_Shape(plotProc,plotType,gph,args,aCnt)
{


}//A_Line


void A_Line::left2right()
{
	 float pos;

   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  pos = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());

	 //Change to required color:
	 m_vArgs[3][0] = toupper(m_vArgs[3][0]);
	 m_Graph->setDataPlotColor(setColor(m_vArgs[3].c_str()));
   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  //Define position at hwixh to draw line
	  pos = atof(m_vArgs[0].c_str());

	  //Change to required color:
	  m_vArgs[3][0] = toupper(m_vArgs[3][0]);
	  m_Graph->setDataPlotColor(setColor(m_vArgs[3].c_str()));

   }

  m_Graph->setPenColor();
  m_Graph->drawHorLine(pos);


}//left2right

void A_Line::upDown()
{
	 float pos;

   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  pos = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());

	 //Change to required color:
	 m_vArgs[3][0] = toupper(m_vArgs[3][0]);
	 m_Graph->setDataPlotColor(setColor(m_vArgs[3].c_str()));
   }

   if(m_bProcFIXED)
   {
	  m_iTotAniCnt = 1;
	  //Define position at hwixh to draw line
	  pos = atof(m_vArgs[0].c_str());

	  //Change to required color:
	  m_vArgs[3][0] = toupper(m_vArgs[3][0]);
	  m_Graph->setDataPlotColor(setColor(m_vArgs[3].c_str()));

   }

  m_Graph->setPenColor();
  m_Graph->drawVerLine(pos);


}//upDown


void A_Line::unconstrained()
{
	//Animation for unconstrained trajectories - can change along x  and y axes


}//unconstrained


void A_Line::upDownChangeSize()
{

}//expand

void A_Line::leftRightChangeSize()
{

}//leftRightChangeSize


void A_Line::rotate()
{

}//rotate

void A_Line::fadeIn()
{

}//fadeIn

void A_Line::fadeOut()
{


}//fadeOut


int A_Line::movieStillCnt()
{
  if(m_bProcVAR)
  {

	  m_iSign = 1;
	  m_iTotAniCnt = (atof(m_vArgs[1].c_str()) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[2].c_str());
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










