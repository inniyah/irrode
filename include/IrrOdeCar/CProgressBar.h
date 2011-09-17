#ifndef IPROGRESSBAR_H_
#define IPROGRESSBAR_H_
#include <irrlicht.h>

using namespace irr;

class CProgressBar : public gui::IGUIElement {
  public:
     CProgressBar(gui::IGUIEnvironment * guienv,const core::rect<s32>& rectangle,s32 id=-1,gui::IGUIElement * parent=0);


     /*Set percentage in positive percentual (0~100). Please note that a call to this function with others values, will set the progress bar to 0.*/
     void setProgress(irr::u32 progress);

     /*Set bar Colors*/
     void setColors(irr::video::SColor progress= irr::video::SColor(255,255,255,255),irr::video::SColor filling= irr::video::SColor(255,0,0,0));

     /*Allow you to add a "border" into your bar. You MUST specify the size (of course in pixel) of the border. You can also pass a color parameter (Black by default)*/
     void addBorder(irr::s32 size,irr::video::SColor color = irr::video::SColor(255,0,0,0));


     virtual void draw();

  private:
     gui::IGUIEnvironment * gui; //GUI ENV. pointer
     irr::s32 total; //Dimension (X) of the bar, to calculate relative percentage.
     core::rect<s32> bar; //Dimension of the bar
     core::rect<s32> position; //Bar
     core::rect<s32> border; //Border
     core::rect<s32> tofill; //Percentage indicator
     core::rect<s32> empty; //"Empty" indicator

     irr::video::SColor fillcolor;
     irr::video::SColor emptycolor;
     irr::video::SColor bordercolor;
     irr::video::IVideoDriver * vdriver;

};
#endif
