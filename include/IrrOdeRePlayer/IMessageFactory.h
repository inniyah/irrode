#ifndef _I_MESSAGE_FACTORY
  #define _I_MESSAGE_FACTORY

  #include <event/IIrrOdeEvent.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace ode;

class CIrrOdeMessageFactory;

class IMessageFactory {
  protected:

  public:
    virtual IIrrOdeEvent *createMessage(CSerializer *pData, ISceneManager *pSmgr)=0;
};

#endif

