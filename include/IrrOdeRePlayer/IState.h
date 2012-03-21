#ifndef _I_STATE
  #define _I_STATE

  #include <irrlicht.h>

class IState {
  public:
    virtual void activate()=0;
    virtual void deactivate()=0;

    virtual irr::u32 update()=0;
};

#endif
