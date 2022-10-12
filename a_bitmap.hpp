#ifndef ABITMAP_HPP
#define ABITMAP_HPP
/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_BitMap
					  Derived class for animating bitmap images.


----------------------------------------------------------------*/
#include <stdio.h>
#include <ExtCtrls.hpp>
#include "global.hpp"
#include "a_shape.hpp"
//---------------------------------------------------------------------------

class A_BitMap: public A_Shape
{
   public:
	  A_BitMap(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt);
	  virtual void left2right();
	  virtual void upDown();
	  virtual void unconstrained();
	  virtual void rotate();
	  virtual void fadeIn();
	  virtual void fadeOut();
	  virtual void upDownChangeSize();
	  virtual void leftRightChangeSize();
	  virtual int movieStillCnt();
   protected:
	char m_sBitmapPath[100];


};//A_BitMap

#endif
