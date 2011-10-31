  #include <irrlicht.h>
  #include <CRearView.h>
  #include <CGUINeedleIndicator.h>

using namespace irr;

CRearView::CRearView(IrrlichtDevice *pDevice, const char *sName, scene::ICameraSceneNode *pCam) : IRenderToTexture(pDevice,sName,core::dimension2d<u32>(256,256)) {
  m_pCam=pCam;
  m_pDevice=pDevice;
  u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CRearView: replaced %i texture.\n",iReplace);
}

CRearView::~CRearView() {
}

void CRearView::update(bool b) {
  startRttUpdate();
  scene::ICameraSceneNode *pActCam=m_pSmgr->getActiveCamera();
  m_pSmgr->setActiveCamera(m_pCam);
  m_pSmgr->drawAll();
  endRttUpdate();
  m_pSmgr->setActiveCamera(pActCam);
}

void CRearView::setCameraParameters(const core::vector3df cPos, const core::vector3df cTgt, const core::vector3df cUp) {
  m_pCam->setPosition(cPos);
  m_pCam->setTarget(cTgt);
  m_pCam->setUpVector(cUp);
}
