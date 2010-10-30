#ifndef _C_IRR_ODE_MESSAGE_FACTORY
  #define _C_IRR_ODE_MESSAGE_FACTORY

  #include <IEventFactory.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace ode;

class CIrrOdeDefaultEventFactory : public IEventFactory {
  protected:

  public:
    CIrrOdeDefaultEventFactory();
    virtual IIrrOdeEvent *createMessage(CSerializer *pData, ISceneManager *pSmgr);
};

class CIrrOdeEventFactory {
  protected:
    ISceneManager *m_pSmgr;
    CIrrOdeDefaultEventFactory *m_pDefaultFact;
    list<IEventFactory *> m_lFactories;
    CIrrOdeEventFactory();
    ~CIrrOdeEventFactory();

  public:
    static CIrrOdeEventFactory *getSharedEventFactory();
    IIrrOdeEvent *createMessage(CSerializer *pData, ISceneManager *pSmgr);
    void registerEventFactory(IEventFactory *p);
    void setSceneManager(ISceneManager *pSmgr);
    ISceneManager *getSceneManager() { return m_pSmgr; }
};

#endif
