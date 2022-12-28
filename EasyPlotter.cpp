//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("EasyPlotter.res");
USEFORM("Easy.cpp", EasyPlotForm);
USEUNIT("..\class_library\floatvec.cpp");
USEUNIT("..\class_library\floatmat.cpp");
USEUNIT("..\class_library\wavetool.cpp");
USEFORM("graph.cpp", GraphForm);
USEFORM("legend.cpp", SettingsDlg);
USEUNIT("..\class_library\audiodev.cpp");
USEUNIT("..\class_library\applaunch.cpp");
USEFORM("movie.cpp", MovieForm);
USEUNIT("a_shape.cpp");
USEUNIT("a_line.cpp");
USEUNIT("a_arrow.cpp");
USEUNIT("a_barplot.cpp");
USEUNIT("a_bitmap.cpp");
USEUNIT("a_circle.cpp");
USEUNIT("a_text.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   try
   {
       Application->Initialize();
       Application->HelpFile = "";
       Application->CreateForm(__classid(TEasyPlotForm), &EasyPlotForm);
         Application->CreateForm(__classid(TGraphForm), &GraphForm);
         Application->CreateForm(__classid(TSettingsDlg), &SettingsDlg);
         Application->CreateForm(__classid(TMovieForm), &MovieForm);
         Application->Run();
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }
   return 0;
}
//---------------------------------------------------------------------------
