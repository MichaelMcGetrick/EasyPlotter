#ifndef ASHAPE_HPP
#define ASHAPE_HPP
/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Shape
					  Abstract class for animating various shapes.


----------------------------------------------------------------*/
#include <stdio.h>
#include <vector>
#include "global.hpp"
#include "graph.h"


#define MAX_ARGS 10
//---------------------------------------------------------------------------

class A_Shape
{
   public:
	  A_Shape(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt);
	  virtual void left2right() = 0;
	  virtual void upDown() = 0;
	  virtual void unconstrained() = 0;
	  virtual void rotate() = 0;
	  virtual void fadeIn() = 0;
	  virtual void fadeOut()=  0;
	  virtual void upDownChangeSize()=  0;
	  virtual void leftRightChangeSize()=  0;
	  virtual int movieStillCnt() = 0;
	  TColor setColor(const char *color);
	  TColor setColor1(const char *color);
   protected:
	  int m_iPlotProc;
	  char m_sPlotType[20];
	  bool m_bProcVAR,m_bProcFIXED;
	  std::vector<std::string> m_vArgs;
	  TGraphForm *m_Graph;
	  int m_iTotAniCnt,m_iAniCnt;
	  int m_iSign;
      int m_iCntX, m_iCntY;


};//A_Shape

#endif
