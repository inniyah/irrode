#ifndef _I_RENDER_TO_TEXTURE
  #define _I_RENDER_TO_TEXTURE

  #include <irrlicht.h>

class IRenderToTexture {
  protected:
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

      m_pTarget=m_pDrv->addRenderTargetTexture(cSize,"rt",irr::video::ECF_A8R8G8B8);
    }

    virtual ~IRenderToTexture() {
      m_pTarget->drop();
    }

    virtual void update(bool bPlane)=0;

    irr::video::ITexture *getTexture() {
      return m_pTarget;
    }
};

#endif


