  #include <irrlicht.h>
  #include <irrCC.h>

CIrrCC::CIrrCCItem::CIrrCCItem(irr::core::stringw sName, irr::IrrlichtDevice *pDevice, irr::u32 iSet) {
  m_pDevice=pDevice;
  m_sName=sName;
  m_iSet=iSet;
  m_iType=CC_TYPE_NUL;
  m_eKey=irr::KEY_KEY_CODES_COUNT;
  m_eType=eCtrlAxis;

  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_bConflict=false;
  m_fValue=0.0f;
  m_pCrsCtrl=pDevice->getCursorControl();
  m_fMouseSensitivity=10.0f;
  m_pCtrlText=NULL;
  m_pBrother=NULL;

  m_iFaderStep=0;
  m_iFaderLastStep=0;
  m_fFaderDiff=0.0f;
  m_fFaderValue=0.0f;
}

CIrrCC::CIrrCCItem::~CIrrCCItem() {
}

bool CIrrCC::CIrrCCItem::isButton() {
  return m_eType==CIrrCC::eCtrlButton || m_eType==CIrrCC::eCtrlToggleButton;
}

irr::core::stringw CIrrCC::CIrrCCItem::keyCodeToString(irr::EKEY_CODE e) {
  if (e == irr::KEY_LBUTTON)    return irr::core::stringw(L"irr::KEY_LBUTTON");
  if (e == irr::KEY_RBUTTON)    return irr::core::stringw(L"irr::KEY_RBUTTON");
  if (e == irr::KEY_CANCEL)     return irr::core::stringw(L"irr::KEY_CANCEL");
  if (e == irr::KEY_MBUTTON)    return irr::core::stringw(L"irr::KEY_MBUTTON");
  if (e == irr::KEY_XBUTTON1)   return irr::core::stringw(L"irr::KEY_XBUTTON1");
  if (e == irr::KEY_XBUTTON2)   return irr::core::stringw(L"irr::KEY_XBUTTON2");
  if (e == irr::KEY_BACK)       return irr::core::stringw(L"irr::KEY_BACK");
  if (e == irr::KEY_TAB)        return irr::core::stringw(L"irr::KEY_TAB");
  if (e == irr::KEY_CLEAR)      return irr::core::stringw(L"irr::KEY_CLEAR");
  if (e == irr::KEY_RETURN)     return irr::core::stringw(L"irr::KEY_RETURN");
  if (e == irr::KEY_SHIFT)      return irr::core::stringw(L"irr::KEY_SHIFT");
  if (e == irr::KEY_CONTROL)    return irr::core::stringw(L"irr::KEY_CONTROL");
  if (e == irr::KEY_MENU)       return irr::core::stringw(L"irr::KEY_MENU");
  if (e == irr::KEY_PAUSE)      return irr::core::stringw(L"irr::KEY_PAUSE");
  if (e == irr::KEY_CAPITAL)    return irr::core::stringw(L"irr::KEY_CAPITAL");
  if (e == irr::KEY_KANA)       return irr::core::stringw(L"irr::KEY_KANA");
  if (e == irr::KEY_HANGUEL)    return irr::core::stringw(L"irr::KEY_HANGUEL");
  if (e == irr::KEY_HANGUL)     return irr::core::stringw(L"irr::KEY_HANGUL");
  if (e == irr::KEY_JUNJA)      return irr::core::stringw(L"irr::KEY_JUNJA");
  if (e == irr::KEY_FINAL)      return irr::core::stringw(L"irr::KEY_FINAL");
  if (e == irr::KEY_HANJA)      return irr::core::stringw(L"irr::KEY_HANJA");
  if (e == irr::KEY_KANJI)      return irr::core::stringw(L"irr::KEY_KANJI");
  if (e == irr::KEY_ESCAPE)     return irr::core::stringw(L"irr::KEY_ESCAPE");
  if (e == irr::KEY_CONVERT)    return irr::core::stringw(L"irr::KEY_CONVERT");
  if (e == irr::KEY_NONCONVERT) return irr::core::stringw(L"irr::KEY_NONCONVERT");
  if (e == irr::KEY_ACCEPT)     return irr::core::stringw(L"irr::KEY_ACCEPT");
  if (e == irr::KEY_MODECHANGE) return irr::core::stringw(L"irr::KEY_MODECHANGE");
  if (e == irr::KEY_SPACE)      return irr::core::stringw(L"irr::KEY_SPACE");
  if (e == irr::KEY_PRIOR)      return irr::core::stringw(L"irr::KEY_PRIOR");
  if (e == irr::KEY_NEXT)       return irr::core::stringw(L"irr::KEY_NEXT");
  if (e == irr::KEY_END)        return irr::core::stringw(L"irr::KEY_END");
  if (e == irr::KEY_HOME)       return irr::core::stringw(L"irr::KEY_HOME");
  if (e == irr::KEY_LEFT)       return irr::core::stringw(L"irr::KEY_LEFT");
  if (e == irr::KEY_UP)         return irr::core::stringw(L"irr::KEY_UP");
  if (e == irr::KEY_RIGHT)      return irr::core::stringw(L"irr::KEY_RIGHT");
  if (e == irr::KEY_DOWN)       return irr::core::stringw(L"irr::KEY_DOWN");
  if (e == irr::KEY_SELECT)     return irr::core::stringw(L"irr::KEY_SELECT");
  if (e == irr::KEY_PRINT)      return irr::core::stringw(L"irr::KEY_PRINT");
  if (e == irr::KEY_EXECUT)     return irr::core::stringw(L"irr::KEY_EXECUT");
  if (e == irr::KEY_SNAPSHOT)   return irr::core::stringw(L"irr::KEY_SNAPSHOT");
  if (e == irr::KEY_INSERT)     return irr::core::stringw(L"irr::KEY_INSERT");
  if (e == irr::KEY_DELETE)     return irr::core::stringw(L"irr::KEY_DELETE");
  if (e == irr::KEY_HELP)       return irr::core::stringw(L"irr::KEY_HELP");
  if (e == irr::KEY_KEY_0)      return irr::core::stringw(L"irr::KEY_KEY_0");
  if (e == irr::KEY_KEY_1)      return irr::core::stringw(L"irr::KEY_KEY_1");
  if (e == irr::KEY_KEY_2)      return irr::core::stringw(L"irr::KEY_KEY_2");
  if (e == irr::KEY_KEY_3)      return irr::core::stringw(L"irr::KEY_KEY_3");
  if (e == irr::KEY_KEY_4)      return irr::core::stringw(L"irr::KEY_KEY_4");
  if (e == irr::KEY_KEY_6)      return irr::core::stringw(L"irr::KEY_KEY_6");
  if (e == irr::KEY_KEY_5)      return irr::core::stringw(L"irr::KEY_KEY_5");
  if (e == irr::KEY_KEY_7)      return irr::core::stringw(L"irr::KEY_KEY_7");
  if (e == irr::KEY_KEY_8)      return irr::core::stringw(L"irr::KEY_KEY_8");
  if (e == irr::KEY_KEY_9)      return irr::core::stringw(L"irr::KEY_KEY_9");
  if (e == irr::KEY_KEY_A)      return irr::core::stringw(L"irr::KEY_KEY_A");
  if (e == irr::KEY_KEY_B)      return irr::core::stringw(L"irr::KEY_KEY_B");
  if (e == irr::KEY_KEY_C)      return irr::core::stringw(L"irr::KEY_KEY_C");
  if (e == irr::KEY_KEY_D)      return irr::core::stringw(L"irr::KEY_KEY_D");
  if (e == irr::KEY_KEY_E)      return irr::core::stringw(L"irr::KEY_KEY_E");
  if (e == irr::KEY_KEY_F)      return irr::core::stringw(L"irr::KEY_KEY_F");
  if (e == irr::KEY_KEY_G)      return irr::core::stringw(L"irr::KEY_KEY_G");
  if (e == irr::KEY_KEY_H)      return irr::core::stringw(L"irr::KEY_KEY_H");
  if (e == irr::KEY_KEY_I)      return irr::core::stringw(L"irr::KEY_KEY_I");
  if (e == irr::KEY_KEY_J)      return irr::core::stringw(L"irr::KEY_KEY_J");
  if (e == irr::KEY_KEY_K)      return irr::core::stringw(L"irr::KEY_KEY_K");
  if (e == irr::KEY_KEY_L)      return irr::core::stringw(L"irr::KEY_KEY_L");
  if (e == irr::KEY_KEY_M)      return irr::core::stringw(L"irr::KEY_KEY_M");
  if (e == irr::KEY_KEY_N)      return irr::core::stringw(L"irr::KEY_KEY_N");
  if (e == irr::KEY_KEY_O)      return irr::core::stringw(L"irr::KEY_KEY_O");
  if (e == irr::KEY_KEY_P)      return irr::core::stringw(L"irr::KEY_KEY_P");
  if (e == irr::KEY_KEY_Q)      return irr::core::stringw(L"irr::KEY_KEY_Q");
  if (e == irr::KEY_KEY_R)      return irr::core::stringw(L"irr::KEY_KEY_R");
  if (e == irr::KEY_KEY_S)      return irr::core::stringw(L"irr::KEY_KEY_S");
  if (e == irr::KEY_KEY_T)      return irr::core::stringw(L"irr::KEY_KEY_T");
  if (e == irr::KEY_KEY_U)      return irr::core::stringw(L"irr::KEY_KEY_U");
  if (e == irr::KEY_KEY_V)      return irr::core::stringw(L"irr::KEY_KEY_V");
  if (e == irr::KEY_KEY_W)      return irr::core::stringw(L"irr::KEY_KEY_W");
  if (e == irr::KEY_KEY_X)      return irr::core::stringw(L"irr::KEY_KEY_X");
  if (e == irr::KEY_KEY_Y)      return irr::core::stringw(L"irr::KEY_KEY_Y");
  if (e == irr::KEY_KEY_Z)      return irr::core::stringw(L"irr::KEY_KEY_Z");
  if (e == irr::KEY_LWIN)       return irr::core::stringw(L"irr::KEY_LWIN");
  if (e == irr::KEY_RWIN)       return irr::core::stringw(L"irr::KEY_RWIN");
  if (e == irr::KEY_APPS)       return irr::core::stringw(L"irr::KEY_APPS");
  if (e == irr::KEY_SLEEP)      return irr::core::stringw(L"irr::KEY_SLEEP");
  if (e == irr::KEY_NUMPAD0)    return irr::core::stringw(L"irr::KEY_NUMPAD0");
  if (e == irr::KEY_NUMPAD1)    return irr::core::stringw(L"irr::KEY_NUMPAD1");
  if (e == irr::KEY_NUMPAD2)    return irr::core::stringw(L"irr::KEY_NUMPAD2");
  if (e == irr::KEY_NUMPAD3)    return irr::core::stringw(L"irr::KEY_NUMPAD3");
  if (e == irr::KEY_NUMPAD4)    return irr::core::stringw(L"irr::KEY_NUMPAD4");
  if (e == irr::KEY_NUMPAD5)    return irr::core::stringw(L"irr::KEY_NUMPAD5");
  if (e == irr::KEY_NUMPAD6)    return irr::core::stringw(L"irr::KEY_NUMPAD6");
  if (e == irr::KEY_NUMPAD7)    return irr::core::stringw(L"irr::KEY_NUMPAD7");
  if (e == irr::KEY_NUMPAD8)    return irr::core::stringw(L"irr::KEY_NUMPAD8");
  if (e == irr::KEY_NUMPAD9)    return irr::core::stringw(L"irr::KEY_NUMPAD9");
  if (e == irr::KEY_MULTIPLY)   return irr::core::stringw(L"irr::KEY_MULTIPLY");
  if (e == irr::KEY_ADD)        return irr::core::stringw(L"irr::KEY_ADD");
  if (e == irr::KEY_SEPARATOR)  return irr::core::stringw(L"irr::KEY_SEPARATOR");
  if (e == irr::KEY_SUBTRACT)   return irr::core::stringw(L"irr::KEY_SUBTRACT");
  if (e == irr::KEY_DECIMAL)    return irr::core::stringw(L"irr::KEY_DECIMAL");
  if (e == irr::KEY_DIVIDE)     return irr::core::stringw(L"irr::KEY_DIVIDE");
  if (e == irr::KEY_F1)         return irr::core::stringw(L"irr::KEY_F1");
  if (e == irr::KEY_F2)         return irr::core::stringw(L"irr::KEY_F2");
  if (e == irr::KEY_F3)         return irr::core::stringw(L"irr::KEY_F3");
  if (e == irr::KEY_F4)         return irr::core::stringw(L"irr::KEY_F4");
  if (e == irr::KEY_F5)         return irr::core::stringw(L"irr::KEY_F5");
  if (e == irr::KEY_F6)         return irr::core::stringw(L"irr::KEY_F6");
  if (e == irr::KEY_F7)         return irr::core::stringw(L"irr::KEY_F7");
  if (e == irr::KEY_F8)         return irr::core::stringw(L"irr::KEY_F8");
  if (e == irr::KEY_F9)         return irr::core::stringw(L"irr::KEY_F9");
  if (e == irr::KEY_F10)        return irr::core::stringw(L"irr::KEY_F10");
  if (e == irr::KEY_F11)        return irr::core::stringw(L"irr::KEY_F11");
  if (e == irr::KEY_F12)        return irr::core::stringw(L"irr::KEY_F12");
  if (e == irr::KEY_F13)        return irr::core::stringw(L"irr::KEY_F13");
  if (e == irr::KEY_F14)        return irr::core::stringw(L"irr::KEY_F14");
  if (e == irr::KEY_F15)        return irr::core::stringw(L"irr::KEY_F15");
  if (e == irr::KEY_F16)        return irr::core::stringw(L"irr::KEY_F16");
  if (e == irr::KEY_F17)        return irr::core::stringw(L"irr::KEY_F17");
  if (e == irr::KEY_F18)        return irr::core::stringw(L"irr::KEY_F18");
  if (e == irr::KEY_F19)        return irr::core::stringw(L"irr::KEY_F19");
  if (e == irr::KEY_F20)        return irr::core::stringw(L"irr::KEY_F20");
  if (e == irr::KEY_F21)        return irr::core::stringw(L"irr::KEY_F21");
  if (e == irr::KEY_F22)        return irr::core::stringw(L"irr::KEY_F22");
  if (e == irr::KEY_F23)        return irr::core::stringw(L"irr::KEY_F23");
  if (e == irr::KEY_F24)        return irr::core::stringw(L"irr::KEY_F24");
  if (e == irr::KEY_NUMLOCK)    return irr::core::stringw(L"irr::KEY_NUMLOCK");
  if (e == irr::KEY_SCROLL)     return irr::core::stringw(L"irr::KEY_SCROLL");
  if (e == irr::KEY_LSHIFT)     return irr::core::stringw(L"irr::KEY_LSHIFT");
  if (e == irr::KEY_RSHIFT)     return irr::core::stringw(L"irr::KEY_RSHIFT");
  if (e == irr::KEY_LCONTROL)   return irr::core::stringw(L"irr::KEY_LCONTROL");
  if (e == irr::KEY_RCONTROL)   return irr::core::stringw(L"irr::KEY_RCONTROL");
  if (e == irr::KEY_LMENU)      return irr::core::stringw(L"irr::KEY_LMENU");
  if (e == irr::KEY_RMENU)      return irr::core::stringw(L"irr::KEY_RMENU");
  if (e == irr::KEY_PLUS)       return irr::core::stringw(L"irr::KEY_PLUS");
  if (e == irr::KEY_COMMA)      return irr::core::stringw(L"irr::KEY_COMMA");
  if (e == irr::KEY_MINUS)      return irr::core::stringw(L"irr::KEY_MINUS");
  if (e == irr::KEY_PERIOD)     return irr::core::stringw(L"irr::KEY_PERIOD");
  if (e == irr::KEY_ATTN)       return irr::core::stringw(L"irr::KEY_ATTN");
  if (e == irr::KEY_CRSEL)      return irr::core::stringw(L"irr::KEY_CRSEL");
  if (e == irr::KEY_EXSEL)      return irr::core::stringw(L"irr::KEY_EXSEL");
  if (e == irr::KEY_EREOF)      return irr::core::stringw(L"irr::KEY_EREOF");
  if (e == irr::KEY_PLAY)       return irr::core::stringw(L"irr::KEY_PLAY");
  if (e == irr::KEY_ZOOM)       return irr::core::stringw(L"irr::KEY_ZOOM");
  if (e == irr::KEY_PA1)        return irr::core::stringw(L"irr::KEY_PA1");
  if (e == irr::KEY_OEM_CLEAR)  return irr::core::stringw(L"irr::KEY_OEM_CLEAR");
  return irr::core::stringw(L"UNKNOWN");
}

irr::gui::IGUITab *CIrrCC::CIrrCCItem::getGuiElement(irr::gui::IGUIElement *pParent, irr::core::position2di cPos, irr::core::position2di cEditPos) {
  irr::gui::IGUIFont *pFont=m_pGuiEnv->getSkin()->getFont();
  irr::core::dimension2du dim=pFont->getDimension(m_sName.c_str());

  irr::core::position2di ulc(cPos.X,cPos.Y),lrc(cPos.Y+dim.Width+cEditPos.X+250,cPos.Y+dim.Height+10);

  m_pGuiElement=m_pGuiEnv->addTab(irr::core::rect<irr::s32>(ulc.X,ulc.Y,lrc.X,lrc.Y),pParent,-1);
  m_pGuiEnv->addStaticText(m_sName.c_str(),irr::core::rect<irr::s32>(0,0,dim.Width+10,dim.Height+10),false,true,m_pGuiElement);
  m_pCtrlText=m_pGuiEnv->addStaticText(L"Not set",irr::core::rect<irr::s32>(cEditPos.X,cEditPos.Y,cEditPos.X+200,cEditPos.Y+dim.Height+2),true,true,m_pGuiElement,65536+m_iIdx);
  this->updateCtrlText();
  return m_pGuiElement;
}

void CIrrCC::CIrrCCItem::setKey(irr::EKEY_CODE e) {
  m_eKey=e;
  m_iType=CC_TYPE_KEY;
}

void CIrrCC::CIrrCCItem::setMouseMove(irr::u32 iAxis, irr::s32 iDirection) {
  m_iMouseAxis=iAxis;
  m_iMouseDirection=iDirection;
  m_iType=CC_TYPE_MOU;
}

void CIrrCC::CIrrCCItem::setMouseButton(irr::u32 iBtn) {
  m_iMouseBtn=iBtn;
  m_iType=CC_TYPE_MBT;
}

void CIrrCC::CIrrCCItem::setJoyAxis(irr::u8 id, irr::u32 iAxis, irr::s32 iDirection) {
  m_iJoyId=id;
  m_iJoyAxis=iAxis;
  m_iJoyDirection=iDirection;
  m_iType=CC_TYPE_JOY;
}

void CIrrCC::CIrrCCItem::setJoyButton(irr::u8 id, irr::u32 iBtn) {
  m_iJoyId=id;
  m_iJoyButton=iBtn;
  m_iType=CC_TYPE_JBT;
}

void CIrrCC::CIrrCCItem::setPov(irr::u8 id, irr::u16 iPov) {
  m_iPov=iPov;
  m_iJoyId=id;
  m_iType=CC_TYPE_POV;
}

irr::gui::IGUIStaticText *CIrrCC::CIrrCCItem::getCtrlText() {
  return m_pCtrlText;
}

bool CIrrCC::CIrrCCItem::hasConflict() {
  return m_bConflict;
}

void CIrrCC::CIrrCCItem::setConflict(bool b) {
  m_bConflict=b;
}

void CIrrCC::CIrrCCItem::updateCtrlText() {
  if (!m_pCtrlText) return;
  wchar_t s[0xFF];

  switch (m_iType) {
    case CC_TYPE_KEY:
      m_pCtrlText->setText(keyCodeToString(m_eKey).c_str());
      break;

    case CC_TYPE_MOU:
      swprintf(s,0xFF,L"Mouse %s %s",m_iMouseAxis==0?L"X":L"Y",m_iMouseDirection>0?L"+":L"-");
      m_pCtrlText->setText(s);
      break;

    case CC_TYPE_MBT:
      swprintf(s,0xFF,L"Mouse Button %i",m_iMouseBtn);
      m_pCtrlText->setText(s);
      break;

    case CC_TYPE_JOY:
      swprintf(s,0xFF,L"Joy %i Axis %i %s",m_iJoyId,m_iJoyAxis,m_iJoyDirection>0?L"+":L"-");
      m_pCtrlText->setText(s);
      break;

    case CC_TYPE_JBT:
      swprintf(s,0xFF,L"Joy %i Button %i",m_iJoyId,m_iJoyButton);
      m_pCtrlText->setText(s);
      break;

    case CC_TYPE_POV:
      swprintf(s,0xFF,L"Joy %i POV %u",m_iJoyId,m_iPov/100);
      m_pCtrlText->setText(s);
      break;

    default:
      break;
  }
}

bool CIrrCC::CIrrCCItem::conflicts(CIrrCCItem *pOther) {
  bool bRet=m_iType==pOther->m_iType;

  if (bRet) {
    switch (m_iType) {
      case CC_TYPE_KEY:
        bRet=m_eKey==pOther->m_eKey;
        break;

      case CC_TYPE_MOU:
        bRet=m_iMouseAxis==pOther->m_iMouseAxis && m_iMouseDirection==pOther->m_iMouseDirection;
        break;

      case CC_TYPE_MBT:
        bRet=m_iMouseBtn==pOther->m_iMouseBtn;
        break;

      case CC_TYPE_JOY:
        bRet=m_iJoyId==pOther->m_iJoyId && m_iJoyAxis==pOther->m_iJoyAxis && m_iJoyDirection==pOther->m_iJoyDirection;
        break;

      case CC_TYPE_JBT:
        bRet=m_iJoyId==pOther->m_iJoyId && m_iJoyButton==pOther->m_iJoyButton;
        break;

      case CC_TYPE_POV:
        bRet=m_iJoyId==pOther->m_iJoyId && m_iPov==pOther->m_iPov;
        break;

      default:
        break;
    }
  }

  return bRet;
}

void CIrrCC::CIrrCCItem::setMouseSensitivity(irr::f32 f) {
  m_fMouseSensitivity=f;
}

void CIrrCC::CIrrCCItem::updateMouse() {
  m_fValue=0.0f;
  irr::core::position2di pos=m_pCrsCtrl->getPosition();
  if (m_iMouseAxis==0) {
    if (m_iMouseDirection>0 && pos.X>320) {
      m_fValue=((irr::f32)pos.X-320)/m_fMouseSensitivity;
      m_fDetectedValue=m_fValue;
      pos.X=320;
    }
    if (m_iMouseDirection<0 && pos.X<320) {
      m_fValue=((irr::f32)320-pos.X)/m_fMouseSensitivity;
      m_fDetectedValue=m_fValue;
      pos.X=320;
    }
  }

  if (m_iMouseAxis==1) {
    if (m_iMouseDirection>0 && pos.Y>200) {
      m_fValue=((irr::f32)pos.Y-200)/m_fMouseSensitivity;
      m_fDetectedValue=m_fValue;
      pos.Y=200;
    }
    if (m_iMouseDirection<0 && pos.Y<200) {
      m_fValue=((irr::f32)200-pos.Y)/m_fMouseSensitivity;
      m_fDetectedValue=m_fValue;
      pos.Y=200;
    }
  }

  if (m_fValue) m_pCrsCtrl->setPosition(pos);
}

irr::f32 CIrrCC::CIrrCCItem::get() {
  if (m_iType==CC_TYPE_MOU) updateMouse();
  return m_pBrother==NULL?m_fValue:m_fValue-m_pBrother->getRaw();
}

irr::f32 CIrrCC::CIrrCCItem::getRaw() {
  if (m_iType==CC_TYPE_MOU) updateMouse();
  return m_fValue;
}

void CIrrCC::CIrrCCItem::set(irr::f32 f) {
  m_fValue=f;
  if (m_eType==eCtrlFader && m_pBrother!=NULL) {
    m_fFaderValue=f;
    m_pBrother->m_fFaderValue=f;
  }
}

void CIrrCC::CIrrCCItem::setWithDetected(irr::f32 f) {
  if (m_eType==eCtrlToggleButton && f==m_fDetectedValue) return;
  m_fValue=f;
  m_fDetectedValue=f;
}

CIrrCC::CIrrCC(irr::IrrlichtDevice *pDevice) {
  m_pDevice=pDevice;
  m_iGuiItemHeight=pDevice->getGUIEnvironment()->getSkin()->getFont()->getDimension(L"Hello World").Height;
  m_pFocused=NULL;
  m_iFocusedIndex=-1;
  m_pDevice->activateJoysticks(m_aJoysticks);

  for (irr::u32 i=0; i<m_aJoysticks.size(); i++) {
    CJoyState *p=new CJoyState();
    m_aJoyState.push_back(p);
  }

  m_bAllowMouse=true;
  m_bAllowFKeys=true;
  m_bAllowJoystick=true;
  m_bLoaded=false;
  m_pCrsCtrl=m_pDevice->getCursorControl();
  m_fMouseSensitivity=10.0f;
}

CIrrCC::~CIrrCC() {
  for (irr::u32 i=0; i<m_aItems.size(); i++) delete m_aItems[i];
  m_aItems.clear();
  m_bSetsCanConflict=true;
}

irr::u32 CIrrCC::addItem(irr::u32 iSet, irr::core::stringw sName, irr::EKEY_CODE eDefault, eControllerType eType) {
  irr::u32 iRet=0;

  if (m_aMaxLenText.size()<=iSet)
    m_aMaxLenText.push_back(sName);
  else
    if (m_aMaxLenText[iSet].size()<sName.size())
      m_aMaxLenText[iSet]=sName;

  CIrrCCItem *item=new CIrrCCItem(sName,m_pDevice,iSet);
  item->setKey(eDefault);
  item->m_eType=eType;
  item->setMouseSensitivity(m_fMouseSensitivity);
  m_aItems.push_back(item);

  iRet=m_aItems.size()-1;
	item->m_iIdx=iRet;

  return iRet;
}

void CIrrCC::addGui(irr::u32 iSet, irr::gui::IGUIElement *pParent, irr::core::position2di cPos) {
  irr::u32 editX=m_pDevice->getGUIEnvironment()->getSkin()->getFont()->getDimension(m_aMaxLenText[iSet].c_str()).Width+10;

  for (irr::u32 i=0; i<m_aItems.size(); i++)
    if (m_aItems[i]->m_iSet==iSet) {
      m_aItems[i]->getGuiElement(pParent,cPos,irr::core::position2di(editX,0));
      cPos.Y+=(irr::s32)(2.5f*m_iGuiItemHeight);
    }
}

bool CIrrCC::ConfigEvent (const irr::SEvent &event) {
  bool bRet=false;
  irr::core::stringw s;
  irr::u32 iItem=m_iFocusedIndex-65536;

  if (event.EventType==irr::EET_GUI_EVENT && event.GUIEvent.EventType==irr::gui::EGET_ELEMENT_FOCUSED) {
    m_cMousePos=m_pDevice->getCursorControl()->getPosition();
		if (event.GUIEvent.Caller->getID()>=65536) {
      if (m_pFocused) m_pFocused->setBackgroundColor(irr::video::SColor(128,192,192,192));
      s=irr::core::stringw(event.GUIEvent.Caller->getText());
      m_pFocused=(irr::gui::IGUIStaticText *)event.GUIEvent.Caller;
      m_pFocused->setBackgroundColor(irr::video::SColor(128,0,255,0));
      m_iFocusedIndex=m_pFocused->getID();
    }
  }

  if (m_pFocused) {
    CIrrCCItem *pItem=iItem<m_aItems.size()?m_aItems[iItem]:NULL,
               *pBrother=pItem!=NULL?pItem->m_pBrother:NULL;

    if (event.EventType==irr::EET_KEY_INPUT_EVENT) {

			if ((m_bAllowFKeys || event.KeyInput.Key<irr::KEY_F1 || event.KeyInput.Key>irr::KEY_F24) && event.KeyInput.Key!=irr::KEY_ESCAPE) {
        pItem->setKey(event.KeyInput.Key);
				bRet=true;
			}

      for (irr::u32 i=0; i<m_aItems.size(); i++) {
        m_aItems[i]->setConflict(false);
        m_aItems[i]->getCtrlText()->setBackgroundColor(irr::video::SColor(128,192,192,192));
      }
    }

    if (event.EventType==irr::EET_MOUSE_INPUT_EVENT && m_bAllowMouse) {
      irr::core::position2di mPos=m_pDevice->getCursorControl()->getPosition();
      if ((mPos.X>m_cMousePos.X+50 || mPos.X<m_cMousePos.X-50) && !pItem->isButton()) {
        irr::s32 iDirection=mPos.X>m_cMousePos.X+50?1:-1;
        pItem->setMouseMove(0,iDirection);
        if (pBrother!=NULL) {
          if (pBrother->m_iMouseAxis!=pItem->m_iMouseAxis && pBrother->m_iMouseDirection!=-pItem->m_iMouseDirection) {
            pBrother->setMouseMove(0,-iDirection);
          }
        }
        bRet=true;
      }

      if ((mPos.Y>m_cMousePos.Y+50 || mPos.Y<m_cMousePos.Y-50) && !pItem->isButton()) {
        irr::s32 iDirection=mPos.Y>m_cMousePos.Y+50?1:-1;
        pItem->setMouseMove(1,iDirection);
        if (pBrother->m_iMouseAxis!=pItem->m_iMouseAxis && pBrother->m_iMouseDirection!=-pItem->m_iMouseDirection) {
          pBrother->setMouseMove(1,-iDirection);
        }
        bRet=true;
      }

      if (event.MouseInput.Event==irr::EMIE_LMOUSE_PRESSED_DOWN) {
        pItem->setMouseButton(0);
        bRet=true;
      }

      if (event.MouseInput.Event==irr::EMIE_RMOUSE_PRESSED_DOWN) {
        pItem->setMouseButton(1);
        bRet=true;
      }

      if (event.MouseInput.Event==irr::EMIE_MMOUSE_PRESSED_DOWN) {
        pItem->setMouseButton(2);
        bRet=true;
      }
    }

    if (event.EventType==irr::EET_JOYSTICK_INPUT_EVENT && m_bAllowJoystick) {
      irr::u8 joyId=event.JoystickEvent.Joystick;
      for (irr::u32 i=0; i<6; i++) {
        irr::s16 pos1=(m_aJoyState[joyId]->iAxis[i]),
            pos2=event.JoystickEvent.Axis[i];

				if (pos2<16000 && pos2>-16000) pos2=0;
        if (pos1!=pos2 && !pItem->isButton()) {
          irr::s32 iDirection=pos1>pos2?1:-1;
          pItem->setJoyAxis(joyId,i,iDirection);
          if (pBrother!=NULL) {
            if (pItem->m_iJoyId!=pBrother->m_iJoyId || pItem->m_iJoyAxis!=pBrother->m_iJoyAxis)
              pBrother->setJoyAxis(joyId,i,-iDirection);
          }
          bRet=true;
        }

        if (m_aJoyState[joyId]->iPov!=event.JoystickEvent.POV) {
          pItem->setPov(joyId,event.JoystickEvent.POV);
          bRet=true;
        }

        if (m_aJoyState[joyId]->iButtons!=event.JoystickEvent.ButtonStates)
          for (irr::u32 i=0; i<32; i++)
            if (m_aJoyState[joyId]->buttonPressed(i)!=event.JoystickEvent.IsButtonPressed(i)) {
              pItem->setJoyButton(joyId,i);
              bRet=true;
            }
      }
    }

    if (bRet) {
      pItem->updateCtrlText();
      pItem->setConflict(false);

      if (pBrother!=NULL) {
        pBrother->updateCtrlText();
        pBrother->setConflict(false);
        pBrother->getCtrlText()->setBackgroundColor(irr::video::SColor(128,192,192,192));
      }

      m_pFocused->setBackgroundColor(irr::video::SColor(128,192,192,192));
      m_pFocused=NULL;
      m_iFocusedIndex=-1;
      m_pDevice->getGUIEnvironment()->setFocus(NULL);
    }
  }

  for (irr::u32 i=0; i<m_aItems.size(); i++) {
    for (irr::u32 j=i+1; j<m_aItems.size(); j++) {
      if (m_aItems[i]->conflicts(m_aItems[j]) && (m_aItems[i]->m_iSet==m_aItems[j]->m_iSet || m_bSetsCanConflict)) {
        m_aItems[j]->getCtrlText()->setBackgroundColor(irr::video::SColor(128,255,0,0)); m_aItems[j]->setConflict(true);
        m_aItems[i]->getCtrlText()->setBackgroundColor(irr::video::SColor(128,255,0,0)); m_aItems[i]->setConflict(true);
      }
    }
  }

  if (!m_pFocused) {
    if (event.EventType==irr::EET_JOYSTICK_INPUT_EVENT) {
      for (irr::u32 i=0; i<6; i++) {
        irr::s16 pos=event.JoystickEvent.Axis[i];
				if (pos<16000 && pos>-16000) pos=0;
        m_aJoyState[event.JoystickEvent.Joystick]->iAxis[i]=pos;
      }

      m_aJoyState[event.JoystickEvent.Joystick]->iPov=event.JoystickEvent.POV;
    }
  }

  return bRet;
}

void CIrrCC::setAllowMouse(bool b) {
  m_bAllowMouse=b;
}

void CIrrCC::setAllowFKeys(bool b) {
  m_bAllowFKeys=b;
}

void CIrrCC::setAllowJoystick(bool b) {
  m_bAllowJoystick=b;
}

irr::f32 CIrrCC::get(irr::u32 idx) {
  //if the queried item is a fader we have to do something special.
  if (m_aItems[idx]->m_eType==eCtrlFader && m_aItems[idx]->m_pBrother!=NULL) {
    //get the two items that make the fader
    CIrrCCItem *p1=m_aItems[idx],*p2=m_aItems[idx]->m_pBrother;
    irr::u32 iTime=m_pDevice->getTimer()->getTime();

    //if the fader is a joy or mouse axis we simply return it's value
    if (p1->m_iType==CC_TYPE_JOY || p1->m_iType==CC_TYPE_MOU) {
      p1->m_fFaderValue=p1->get();
      p2->m_fFaderValue=p1->m_fFaderValue;
    }
    else {
      //if if's a button or a key we need to calculate it's current
      //value from using it's time step and it's diff per step value

      //in case the value is queried for the first time we need to initialize the timer stuff...
      if (p1->m_iFaderLastStep==0) {
        p1->m_iFaderLastStep=iTime;
        p2->m_iFaderLastStep=iTime;
      }
      else {
        //otherwise we calculate and return it's value
        irr::f32 fValue=p1->get();
        while (p1->m_iFaderLastStep<iTime) {
          p1->m_fFaderValue+=fValue*p1->m_fFaderDiff;
          p1->m_iFaderLastStep+=p1->m_iFaderStep;
        }
        if (p1->m_fFaderValue> 1.0f) p1->m_fFaderValue= 1.0f;
        if (p1->m_fFaderValue<-1.0f) p1->m_fFaderValue=-1.0f;
        p2->m_fFaderValue=p1->m_fFaderValue;
      }
    }

    return p1->m_fFaderValue;
  }

  //if it's not a fader we simply return it's value
  return m_aItems[idx]->get();
}

void CIrrCC::set(irr::u32 idx, irr::f32 f) {
  m_aItems[idx]->set(f);
}

bool CIrrCC::OnEvent (const irr::SEvent &event) {
  bool bRet=false;
  irr::u32 i,j;

  if (event.EventType==irr::EET_KEY_INPUT_EVENT)
    for (i=0; i<m_aItems.size(); i++) {
      CIrrCCItem *p=m_aItems[i];
      if (p->m_iType==CC_TYPE_KEY && p->m_eKey==event.KeyInput.Key) {
        p->setWithDetected(event.KeyInput.PressedDown?1.0f:0.0f);
        bRet=true;
      }
    }

  if (event.EventType==irr::EET_JOYSTICK_INPUT_EVENT)
    for (i=0; i<m_aItems.size(); i++) {
      CIrrCCItem *p=m_aItems[i];
      switch (p->m_iType) {
        case CC_TYPE_JOY:
          if (p->m_iJoyId==event.JoystickEvent.Joystick)
            for (j=0; j<6; j++)
              if (p->m_iJoyAxis==j) {
                irr::f32 pos=((irr::f32)event.JoystickEvent.Axis[j])/256.0f;
                if (pos>10.0f) {
                  pos/=127;
                  if (p->m_iJoyDirection<0) p->setWithDetected(pos);
                }
                else
								  if (pos<-10.0f) {
                    pos/=128;
                    if (p->m_iJoyDirection>0) p->setWithDetected(-pos);
                  }
									else p->setWithDetected(0.0f);

                bRet=true;
              }
          break;

        case CC_TYPE_JBT:
          if (p->m_iJoyId==event.JoystickEvent.Joystick)
            p->setWithDetected(event.JoystickEvent.IsButtonPressed(p->m_iJoyButton)?1.0f:0.0f);
          break;

        case CC_TYPE_POV:
          if (p->m_iJoyId==event.JoystickEvent.Joystick) {
            p->setWithDetected(event.JoystickEvent.POV==p->m_iPov?1.0f:0.0f);
            bRet=true;
          }
          break;

        default:
          break;
      }
    }

  if (event.EventType==irr::EET_MOUSE_INPUT_EVENT)
    for (i=0; i<m_aItems.size(); i++) {
      CIrrCCItem *p=m_aItems[i];
      if (p->m_iType==CC_TYPE_MBT) {
        if (p->m_iMouseBtn==0) {
          if (event.MouseInput.Event==irr::EMIE_LMOUSE_PRESSED_DOWN) { p->setWithDetected(1.0f); bRet=true; }
          if (event.MouseInput.Event==irr::EMIE_LMOUSE_LEFT_UP     ) { p->setWithDetected(0.0f); bRet=true; }
        }
        if (p->m_iMouseBtn==1) {
          if (event.MouseInput.Event==irr::EMIE_RMOUSE_PRESSED_DOWN) { p->setWithDetected(1.0f); bRet=true; }
          if (event.MouseInput.Event==irr::EMIE_RMOUSE_LEFT_UP     ) { p->setWithDetected(0.0f); bRet=true; }
        }
        if (p->m_iMouseBtn==2) {
          if (event.MouseInput.Event==irr::EMIE_MMOUSE_PRESSED_DOWN) { p->setWithDetected(1.0f); bRet=true; }
          if (event.MouseInput.Event==irr::EMIE_MMOUSE_LEFT_UP     ) { p->setWithDetected(0.0f); bRet=true; }
        }
      }
    }

  return bRet;
}

void CIrrCC::setMouseSensitivity(irr::f32 f) {
  m_fMouseSensitivity=f;
  for (irr::u32 i=0; i<m_aItems.size(); i++) m_aItems[i]->setMouseSensitivity(f);
}

void CIrrCC::writeConfig(irr::io::IXMLWriter *pXml) {
  pXml->writeElement(L"IrrConfigControl");
  pXml->writeLineBreak();

  for (irr::u32 i=0; i<m_aItems.size(); i++) {
    CIrrCCItem *p=m_aItems[i];

    irr::core::array<irr::core::stringw> names,values;
    wchar_t s[0xFF];

                                       names.push_back(irr::core::stringw(L"name")); values.push_back(p->m_sName.c_str());
    swprintf(s,0xFF,L"%u",p->m_iType); names.push_back(irr::core::stringw(L"type")); values.push_back(s                 );
    swprintf(s,0xFF,L"%u",p->m_iSet ); names.push_back(irr::core::stringw(L"set" )); values.push_back(s                 );

    switch (p->m_iType) {
      case CC_TYPE_KEY:
        names.push_back(irr::core::stringw(L"key")); swprintf(s,0xFF,L"%u",(irr::u32)p->m_eKey); values.push_back(s);
        break;

      case CC_TYPE_MOU:
        names.push_back(irr::core::stringw(L"m_axis")); swprintf(s,0xFF,L"%u",p->m_iMouseAxis     ); values.push_back(s);
        names.push_back(irr::core::stringw(L"m_dir" )); swprintf(s,0xFF,L"%i",p->m_iMouseDirection); values.push_back(s);
        break;

      case CC_TYPE_MBT:
        names.push_back(irr::core::stringw(L"m_btn")); swprintf(s,0xFF,L"%u",p->m_iMouseBtn); values.push_back(s);
        break;

      case CC_TYPE_JOY:
        names.push_back(irr::core::stringw(L"joy"   )); swprintf(s,0xFF,L"%u",p->m_iJoyId       ); values.push_back(s);
        names.push_back(irr::core::stringw(L"j_axis")); swprintf(s,0xFF,L"%u",p->m_iJoyAxis     ); values.push_back(s);
        names.push_back(irr::core::stringw(L"j_dir" )); swprintf(s,0xFF,L"%i",p->m_iJoyDirection); values.push_back(s);
        break;

      case CC_TYPE_JBT:
        names.push_back(irr::core::stringw(L"joy"  )); swprintf(s,0xFF,L"%u",p->m_iJoyId    ); values.push_back(s);
        names.push_back(irr::core::stringw(L"j_btn")); swprintf(s,0xFF,L"%u",p->m_iJoyButton); values.push_back(s);
        break;

      case CC_TYPE_POV:
        names.push_back(irr::core::stringw(L"joy")); swprintf(s,0xFF,L"%u",p->m_iJoyId); values.push_back(s);
        names.push_back(irr::core::stringw(L"pov")); swprintf(s,0xFF,L"%u",p->m_iPov  ); values.push_back(s);
        break;

      default:
        break;
    }

    pXml->writeElement(L"control",true,names,values);
    pXml->writeLineBreak();
  }

  pXml->writeClosingTag(L"IrrConfigControl");
  pXml->writeLineBreak();
}

void CIrrCC::readConfig(irr::io::IXMLReaderUTF8 *pXml) {
  irr::u32 iNum=0;
  if (pXml && !strcmp(pXml->getNodeName(),"IrrConfigControl") && pXml->getNodeType()==irr::io::EXN_ELEMENT)
    while (pXml->read() && iNum<m_aItems.size() && strcmp(pXml->getNodeName(),"IrrConfigControl"))
      if (!strcmp(pXml->getNodeName(),"control")) {
        CIrrCCItem *p=m_aItems[iNum];
        p->m_iType=(IrrConfigControlTypes)(atoi(pXml->getAttributeValue("type")));
        p->m_iSet =atoi(pXml->getAttributeValue("set"));

        switch (p->m_iType) {
          case CC_TYPE_KEY:
            p->setKey((irr::EKEY_CODE)atoi(pXml->getAttributeValue("key")));
            break;

          case CC_TYPE_MOU:
            p->setMouseMove(atoi(pXml->getAttributeValue("m_axis")),atoi(pXml->getAttributeValue("m_dir")));
            break;

          case CC_TYPE_MBT:
            p->setMouseButton(atoi(pXml->getAttributeValue("m_btn")));
            break;

          case CC_TYPE_JOY:
            p->setJoyAxis(atoi(pXml->getAttributeValue("joy")),atoi(pXml->getAttributeValue("j_axis")),atoi(pXml->getAttributeValue("j_dir")));
            break;

          case CC_TYPE_JBT:
            p->setJoyButton(atoi(pXml->getAttributeValue("joy")),atoi(pXml->getAttributeValue("j_btn")));
            break;

          case CC_TYPE_POV:
            p->setPov(atoi(pXml->getAttributeValue("joy")),atoi(pXml->getAttributeValue("pov")));
            break;

          default:
            break;
        }

        p->updateCtrlText();
        iNum++;
      }

  m_bLoaded=true;
}

void CIrrCC::reset() {
  for (irr::u32 i=0; i<m_aItems.size(); i++) {
    m_aItems[i]->m_fValue=0.0f;
    m_aItems[i]->m_fDetectedValue=0.0f;
  }
}

void CIrrCC::createAxis(irr::u32 idx1, irr::u32 idx2) {
  m_aItems[idx1]->m_pBrother=m_aItems[idx2];
  m_aItems[idx2]->m_pBrother=m_aItems[idx1];
}

void CIrrCC::createFader(irr::u32 idx1, irr::u32 idx2, irr::u32 iStep, irr::f32 fInc) {
  CIrrCCItem *item1=idx1<m_aItems.size()?m_aItems[idx1]:NULL,
             *item2=idx2<m_aItems.size()?m_aItems[idx2]:NULL;

  if (item1==NULL || item2==NULL) return;

  item1->m_pBrother=item2;
  item2->m_pBrother=item1;
  item1->m_iFaderStep=iStep;
  item2->m_iFaderStep=iStep;
  item1->m_fFaderDiff= fInc;
  item2->m_fFaderDiff=-fInc;
  item1->m_fFaderValue=0.0f;
  item2->m_fFaderValue=0.0f;
}

const wchar_t *CIrrCC::getSettingsText(irr::u32 iSet) {
  static wchar_t sSettingsText[1024];
  sSettingsText[0]=L'\0';
  irr::u32 iLen=0;
  for (irr::u32 i=0; i<m_aItems.size(); i++) {
    CIrrCCItem *p=m_aItems[i];
    if (p->m_iSet==iSet && p->m_sName.size()>iLen) iLen=p->m_sName.size();
  }

  for (irr::u32 i=0; i<m_aItems.size(); i++) {
    CIrrCCItem *p=m_aItems[i];
    if (p->m_iSet==iSet) {
      irr::gui::IGUIStaticText *t=p->getCtrlText();
      swprintf(sSettingsText,1023,L"%s%s:\t%s\n",sSettingsText,p->m_sName.c_str(),t->getText());
    }
  }
  return sSettingsText;
}

void CIrrCC::dumpState(irr::f32 *fBuffer) {
  for (irr::u32 i=0; i<m_aItems.size(); i++)
    fBuffer[i]=m_aItems[i]->m_eType==eCtrlFader?m_aItems[i]->m_fFaderValue:m_aItems[i]->get();
}

void CIrrCC::restoreState(irr::f32 *fBuffer) {
  for (irr::u32 i=0; i<m_aItems.size(); i++)
    if (m_aItems[i]->m_eType==eCtrlFader)
      m_aItems[i]->m_fFaderValue=fBuffer[i];
    else
      m_aItems[i]->set(fBuffer[i]);
}
