/*---------------------------------------------------------------
   AUTHOR:            M.J. McGetrick
					  (c) Copyright 2016 M.J. McGetrick
   DATE:              12/14/2016
   DESCRIPTION:       A_Shape
					  Abstract class for animating various shapes.

----------------------------------------------------------------*/
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include "a_shape.hpp"
#include "global.hpp"


/*---------------------------------------------------------------------------
CONSTRUCTOR PLACEHOLDER:
N.B.: Constructor not necessary for an abstract class.
	  However, it can be useful to include one if some
	  intialisations common to all derived classes are required.
----------------------------------------------------------------------------*/
A_Shape::A_Shape(int plotProc,char *plotType,TGraphForm *gph,std::vector<std::string> args,int aCnt)
{

	m_iPlotProc = plotProc;
	strcpy(m_sPlotType,plotType);
	if(m_iPlotProc == 0)    //FIXED background plot
	{
		m_bProcVAR = false;
		m_bProcFIXED = true;

	}
	if(m_iPlotProc == 1)  //VARIABLE (animation) plot
	{
		m_bProcVAR = true;
		m_bProcFIXED = false;

	}

	m_iAniCnt = aCnt;

	m_Graph = gph;
	m_vArgs = (std::vector<std::string>)(args);
	int size = m_vArgs.size();
	std::string str;
	for(int i=0;i<size;i++)
	{
		str = m_vArgs[i];
	}

	m_iSign = 1;
	m_iCntX = 0;
    m_iCntY = 0;



}//A_Shape
//---------------------------------------------------------------------------
TColor A_Shape::setColor(const char *color)
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


TColor A_Shape::setColor1(const char *color)
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












