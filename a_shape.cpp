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

	if( strstr(color,"White") != NULL )
        return clWhite;
	if( strstr(color,"Black") != NULL )
        return clBlack;
	if( strstr(color,"Red") != NULL )
        return clRed;
	if( strstr(color,"Blue") != NULL )
        return clBlue;
	if( strstr(color,"Yellow") != NULL )
         return clYellow;
	if( strstr(color,"Green") != NULL )
         return clGreen;
	if( strstr(color,"Maroon") != NULL )
         return clMaroon;
	if( strstr(color,"Lime") != NULL )
         return clLime;
	if( strstr(color,"Purple") != NULL )
        return clPurple;
	if( strstr(color,"Grey") != NULL )
        return clGray;

}//setColor


TColor A_Shape::setColor1(const char *color)
{
	TColor Color;

    if( strstr(color,"white") != NULL )
	{
		Color = clWhite;

	}
	if( strstr(color,"black") != NULL )
	{
		Color = clBlack;

	}
	if( strstr(color,"red") != NULL )
	{
	   Color = clRed;

	}
	if( strstr(color,"blue") != NULL )
	{
	  Color = clBlue;

	}
	if( strstr(color,"yellow") != NULL )
	{
	   Color = clYellow;

	}
	if( strstr(color,"green") != NULL )
	{
	   Color = clGreen;

	}
	if( strstr(color,"maroon") != NULL )
	{
		Color = clMaroon;

	}
	if( strstr(color,"lime") != NULL )
	{
	  Color = clLime;

	}
	if( strstr(color,"purple") != NULL )
	{
		Color = clPurple;

	}
	if( strstr(color,"grey") != NULL )
	{
		Color = clGray;

	}
    
	return Color;

}//setColor1












