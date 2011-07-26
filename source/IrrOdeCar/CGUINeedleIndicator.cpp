  #include <irrlicht.h>
  #include <CGUINeedleIndicator.h>

namespace irr {
  namespace gui {
    CGUINeedleIndicator::CNeedle::CNeedle(video::IVideoDriver *pDrv, video::SColor cColor, f32 fLength, f32 fWidth, f32 fFactor, core::position2di vCenter, core::dimension2di vDim) {
      m_cColor=cColor;
      m_fLength=fLength;
      m_fWidth=fWidth;
      m_fFactor=fFactor;
      m_pDrv=pDrv;
      m_vCenter=vCenter;
      m_vDim=core::dimension2df(vDim.Width,vDim.Height);
      m_fValue=0.0f;
    }

    void CGUINeedleIndicator::CNeedle::setValue(f32 fValue) {
      m_fValue=fValue*m_fFactor;
    }

    void CGUINeedleIndicator::CNeedle::draw() {
      core::vector2df vTip  =(m_vDim.Height/2*m_fLength*core::vector2df( 0.0f, 1.0f)).rotateBy(m_fValue),
                      vBase1=(m_vDim.Width /2*m_fWidth *core::vector2df( 1.0f,-0.5f)).rotateBy(m_fValue),
                      vBase2=(m_vDim.Width /2*m_fWidth *core::vector2df(-1.0f,-0.5f)).rotateBy(m_fValue);

      core::vector2di v[]={
        core::vector2di((s32)vTip  .X,(s32)vTip  .Y),
        core::vector2di((s32)vBase1.X,(s32)vBase1.Y),
        core::vector2di((s32)vBase2.X,(s32)vBase2.Y)
      };

      for (u32 i=0; i<3; i++) {
        m_pDrv->draw2DLine(m_vCenter+v[i],m_vCenter+v[i<2?i+1:0],m_cColor);
      }
    }

    CGUINeedleIndicator::CGUINeedleIndicator(IGUIEnvironment *pGuiEnv, IGUIElement *pParent, s32 iId, core::rect<s32> cRect) : IGUIElement(EGUIET_ELEMENT,pGuiEnv,pParent,iId,cRect) {
      m_pGuiEnv=pGuiEnv;
      m_pTexture=NULL;
      m_pDrv=pGuiEnv->getVideoDriver();
      m_fMin=-1.0f;
      m_fMax= 1.0f;
      m_fMinAngle=0.0f;
      m_fMaxAngle=0.0f;
      m_fAngleOffset=0.0f;
      m_fSum=m_fMax-m_fMin;
      m_eMode=eOverflowIgnore;
      m_bReverse=false;
      m_bVisible=true;
    }

    CGUINeedleIndicator::~CGUINeedleIndicator() {
      while (m_lNeedles.getSize()>0) {
        core::list<CNeedle *>::Iterator it=m_lNeedles.begin();
        CNeedle *p=*it;
        m_lNeedles.erase(it);
        delete p;
      }
    }

    void CGUINeedleIndicator::draw() {
      if (!m_bVisible) return;

      if (m_pTexture==NULL) {
        video::SColor cCol=video::SColor(0xFF,0,0,0xFF);
        m_pDrv->draw2DRectangle(AbsoluteRect,cCol,cCol,cCol,cCol);
      }
      else {
        m_pDrv->draw2DImage(m_pTexture,AbsoluteRect,core::rect<s32>(0,0,m_pTexture->getOriginalSize().Width,m_pTexture->getOriginalSize().Height));
      }

      core::list<CNeedle *>::Iterator it;
      for (it=m_lNeedles.begin(); it!=m_lNeedles.end(); it++) (*it)->draw();
    }

    void CGUINeedleIndicator::setBackground(video::ITexture *pBackground) {
      m_pTexture=pBackground;
    }

    void CGUINeedleIndicator::addNeedle(video::SColor cCol, f32 fLength, f32 fWidth, f32 fFactor) {
      core::position2di  cCenter=AbsoluteRect.getCenter();
      core::dimension2di cDim   =AbsoluteRect.getSize();

      CNeedle *p=new CNeedle(m_pDrv,cCol,fLength,fWidth,fFactor,cCenter,cDim);
      m_lNeedles.push_back(p);
      m_aNeedles.push_back(p);
    }

    void CGUINeedleIndicator::setRange(f32 fMin, f32 fMax) {
      m_fMin=fMin;
      m_fMax=fMax;

      m_fSum=m_fMax-m_fMin;
    }

    u32 CGUINeedleIndicator::getNeedleCount() {
      return m_aNeedles.size();
    }

    void CGUINeedleIndicator::setReverse(bool b) {
      m_bReverse=b;
    }

    CGUINeedleIndicator::CNeedle *CGUINeedleIndicator::getNeedle(u32 i) {
      return i<m_aNeedles.size()?m_aNeedles[i]:NULL;
    }

    void CGUINeedleIndicator::setOverflowMode(eOverflowMode eMode) {
      m_eMode=eMode;
    }

    void CGUINeedleIndicator::setAngleRange(f32 fMin, f32 fMax) {
      printf("setAngleRange: %.2f, %.2f\n",fMin,fMax);
      m_fMinAngle=fMin;
      m_fMaxAngle=fMax;
    }

    void CGUINeedleIndicator::setAngleOffset(f32 fOffset) {
      m_fAngleOffset=fOffset;
    }

    void CGUINeedleIndicator::setValue(f32 f) {
      switch (m_eMode) {
        case eOverflowCut:
          if (f>m_fMax) f=m_fMax;
          if (f<m_fMin) f=m_fMin;
          break;

        case eOverflowFit:
          while (f>m_fMax) f-=m_fSum;
          while (f<m_fMin) f+=m_fSum;
          break;

        default:
          break;
      }

      if (m_fMinAngle<m_fMaxAngle) {
        m_fValue=(m_fMaxAngle-m_fMinAngle)*(f/m_fSum);

        if (m_fValue>m_fMaxAngle) m_fValue=m_fMaxAngle;
        if (m_fValue<m_fMinAngle) m_fValue=m_fMinAngle;
      }
      else m_fValue=360.0f*(f/m_fSum);

      m_fValue+=m_fAngleOffset;
      if (m_bReverse) m_fValue=-m_fValue;

      core::list<CNeedle *>::Iterator it;
      for (it=m_lNeedles.begin(); it!=m_lNeedles.end(); it++) (*it)->setValue(m_fValue);
    }
  }
}
