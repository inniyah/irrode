#ifndef _I_RENDER_TO_TEXTURE
  #define _I_RENDER_TO_TEXTURE

  #include <irrlicht.h>

static int s_iNum = 0;

class IRenderToTexture {
  protected:
    bool m_bActive;

    irr::IrrlichtDevice *m_pDevice;
    irr::gui::IGUIEnvironment *m_pGuienv;
    irr::video::IVideoDriver  *m_pDrv;
    irr::scene::ISceneManager *m_pSmgr;
    irr::core::stringc         m_sName;
    irr::video::ITexture      *m_pTarget;

    irr::u32 processTextureReplace(irr::scene::ISceneNode *pNode) {
      irr::u32 iRet=0;

      irr::core::list<irr::scene::ISceneNode *> lChildren=pNode->getChildren();
      irr::core::list<irr::scene::ISceneNode *>::Iterator it;

      for (irr::u32 i=0; i<pNode->getMaterialCount(); i++) {
        for (irr::u32 j=0; j<4; j++) {
          irr::video::ITexture *p=pNode->getMaterial(i).getTexture(j);
          if (p!=NULL) {
            irr::core::stringc s=pNode->getMaterial(i).getTexture(j)->getName();
            s.make_lower();
            if (s.find(m_sName.c_str())!=-1) {
              pNode->getMaterial(i).setTexture(j,m_pTarget);
              printf("--> \"%s\" (%p)\n",pNode->getName(),(void *)m_pTarget);
              iRet++;
            }
          }
        }
      }

      for (it=lChildren.begin(); it!=lChildren.end(); it++) iRet+=processTextureReplace(*it);
      return iRet;
    }

    void startRttUpdate() { m_pDrv->setRenderTarget(m_pTarget,true,true,irr::video::SColor(0xFF,0xFF,0xD0,0x80)); }
    void endRttUpdate  () { m_pDrv->setRenderTarget(irr::video::ERT_FRAME_BUFFER); }

  public:
    IRenderToTexture(irr::IrrlichtDevice *pDevice, const char *sName, irr::core::dimension2du cSize) {
      m_pDevice=pDevice;

      m_pGuienv=m_pDevice->getGUIEnvironment();
      m_pDrv=m_pDevice->getVideoDriver();
      m_pSmgr=m_pDevice->getSceneManager();

      m_sName=sName;
      m_sName.make_lower();

      irr::c8 s[0xFF];
      sprintf(s,"rt_%i_%s",s_iNum++,sName);
      m_pTarget=m_pDrv->addRenderTargetTexture(cSize,s,irr::video::ECF_A8R8G8B8);

      m_bActive = true;
    }

    virtual ~IRenderToTexture() {
      m_pTarget->drop();
    }

    virtual void update()=0;

    irr::video::ITexture *getTexture() {
      return m_pTarget;
    }

    void setActive(bool b) { m_bActive = b; }

    bool isActive() { return m_bActive; }
};

#endif


