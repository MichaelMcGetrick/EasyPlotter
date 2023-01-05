/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Arrow
					  Derived class for animating arrows.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_arrow.hpp"
//---------------------------------------------------------------------------
A_Arrow::A_Arrow(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
			  :A_Shape(plotProc,plotType,gph,args,aCnt)
{
	m_sArrowType = NULL;


}//A_Arrow

void A_Arrow::setArrowType(char *type)
{
   //strcpy(m_sArrowType,type);
   m_sArrowType = type;

}//setArrowType

void A_Arrow::left2right()
{
   float posx,posy, height,width;
   char text[100];
   //char *color;
   TColor color;

   //Get arguments:
   if(m_bProcVAR)
   {
	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  if( strcmp(m_sArrowType,"rARROW") == 0 )
	  {
		posy = atof(m_vArgs[3].c_str());
		posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);
	  }
	  else if( strcmp(m_sArrowType,"lARROW") == 0 )
	  {
		posy = atof(m_vArgs[3].c_str());
		posx = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		//m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
        m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);
	  }

   }
   if(m_bProcFIXED)
   {
	  if( strcmp(m_sArrowType,"rARROW") == 0 )
	  {
		posy = atof(m_vArgs[3].c_str());
		posx = atof(m_vArgs[0].c_str()) + (m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		//m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);
	  }
	  else if( strcmp(m_sArrowType,"lARROW") == 0 )
	  {
		posy = atof(m_vArgs[3].c_str());
		posx = atof(m_vArgs[0].c_str()) + (m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
        //m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
        m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);
	  }

   }


}//left2right

void A_Arrow::upDown()
{

   float posx,posy, height,width;
   char text[100];
   //char *color;
   TColor color;

   //Get arguments:
   if(m_bProcVAR)
   {

	  if(m_iAniCnt > m_iTotAniCnt)
	  {
		 m_iAniCnt = m_iTotAniCnt+1;     //Limit animation to range of this plot
	  }
	  if( strcmp(m_sArrowType,"uARROW") == 0 )
	  {
		posx = atof(m_vArgs[3].c_str());
		posy = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		//m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);

	  }
	  else if( strcmp(m_sArrowType,"dARROW") == 0 )
	  {
		posx = atof(m_vArgs[3].c_str());
		posy = atof(m_vArgs[0].c_str()) + m_iSign*(m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);

	  }

   }
   if(m_bProcFIXED)
   {
	  if( strcmp(m_sArrowType,"uARROW") == 0 )
	  {
		posx = atof(m_vArgs[3].c_str());
		posy = atof(m_vArgs[0].c_str()) + (m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);

	  }
	  else if( strcmp(m_sArrowType,"dARROW") == 0 )
	  {
		posx = atof(m_vArgs[3].c_str());
		posy = atof(m_vArgs[0].c_str()) + (m_iAniCnt-1)*atof(m_vArgs[2].c_str());
		height = atof(m_vArgs[4].c_str());

		//Change to required color:
		//m_vArgs[5][0] = toupper(m_vArgs[5][0]);
		//Draw an arrow pointing to the right:
		m_Graph->drawArrow(posx,posy,height,width,setColor1(m_vArgs[5].c_str()),m_sArrowType);

	  }

   }


}//upDown


void A_Arrow::unconstrained()
{
	//Animation for unconstrained trajectories - can change along x  and y axes


}//unconstrained



void A_Arrow::upDownChangeSize()
{

}//upDownChangeSize

void A_Arrow::leftRightChangeSize()
{

}//leftRightChangeSize



void A_Arrow::rotate()
{

}//rotate

void A_Arrow::fadeIn()
{

}//fadeIn

void A_Arrow::fadeOut()
{


}//fadeOut


int A_Arrow::movieStillCnt()
{
  if(m_bProcVAR)
  {
	   m_iSign = 1;
	   m_iTotAniCnt = (atof( m_vArgs[1].c_str() ) - atof(m_vArgs[0].c_str()))/atof(m_vArgs[2].c_str());

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










