#ifndef AARROW_HPP
#define AARROW_HPP
/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Arrow
					  Derived class for animating arrows.


----------------------------------------------------------------*/
#include <stdio.h>
#include "global.hpp"
#include "a_shape.hpp"
//---------------------------------------------------------------------------

class A_Arrow: public A_Shape
{
   public:
	  A_Arrow(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt);
	  virtual void left2right();
	  virtual void upDown();
      virtual void unconstrained();
	  virtual void rotate();
	  virtual void fadeIn();
	  virtual void fadeOut();
	  virtual void upDownChangeSize();
      virtual void leftRightChangeSize();
	  virtual int movieStillCnt();
	  void setArrowType(char *type);
   protected:
	  char *m_sArrowType;



};//A_Arrow

#endif
