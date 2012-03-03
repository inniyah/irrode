#ifndef _C_TEST_SLIPSTREAM
  #define _C_TEST_SLIPSTREAM

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

class CSlipStream {
  protected:
    irr::ode::CIrrOdeBody *m_pSource;
    irr::s32 m_iTtl,m_iInitTtl;
    irr::f32 m_fPwr,m_fSize,m_fGrow,m_fInitPwr;
    irr::core::vector3df m_vDir,m_vPos;
    irr::core::list<irr::ode::CIrrOdeBody *> m_lBodies;

  public:
    CSlipStream(irr::ode::CIrrOdeBody *pBody, irr::s32 iTtl, irr::f32 fPwr, irr::f32 fGrow, irr::core::list<irr::ode::CIrrOdeBody *> lBodies) {
      m_pSource=pBody;
      m_iInitTtl=iTtl;
      m_fInitPwr=fPwr;
      m_fGrow=fGrow;

      init();

      m_iTtl=0;

      irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it;
      for (it=lBodies.begin(); it!=lBodies.end(); it++)
        if ((*it)!=pBody) m_lBodies.push_back(*it);
    }

    void init() {
      m_iTtl=m_iInitTtl;
      m_fPwr=m_pSource->getLinearVelocity().getLength()/5.0f*m_fInitPwr;
      m_fSize=0.0f;

      m_vDir=m_pSource->getLinearVelocity();
      m_vDir.normalize();
      m_vPos=m_pSource->getPosition();
    }

    void step() {
      if (m_iTtl<=0) return;

      irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it;
      for (it=m_lBodies.begin(); it!=m_lBodies.end(); it++) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)(*it);
        irr::core::vector3df pos=p->getPosition();
        if ((pos-m_vPos).getLength()<m_fSize) {
          p->addForce(m_fPwr*m_vDir);
        }
      }

      m_fPwr-=m_fInitPwr/m_iTtl;
      m_fSize+=m_fGrow;
      m_iTtl--;
    }
};


/**
 * @class CTestSlipstream
 * Test for slipstream functionality
 */
class CTestSlipstream : public IState, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::ode::CIrrOdeBody *m_pMarble;
    irr::core::list<irr::ode::CIrrOdeBody *> m_lBodies;

    CSlipStream *m_aSlipStream[30];
    irr::s32 m_iCurrent;

  public:
    CTestSlipstream(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestSlipstream() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif


