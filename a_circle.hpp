#ifndef ACIRCLE_HPP
#define ACIRCLE_HPP
/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Circle
					  Derived class for animating circles.


----------------------------------------------------------------*/
#include <stdio.h>
#include "global.hpp"
#include "a_shape.hpp"
//---------------------------------------------------------------------------

class A_Circle: public A_Shape
{
   public:
	  A_Circle(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt);
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


};//A_Circle

#endif
