  #include <CSettings.h>

/**
 * The constructor
 * @param scene to be loaded and displayed in the background. Can be "NULL" if no scene should be shown
 * @param the XML file to read and store the settings
 * @param title of the window
 * @param background color of the window
 */
CSettings::CSettings(const irr::c8 *sSettingsFile, const wchar_t *sTitle, irr::video::SColor cBackground) {
  strcpy(m_sSettingsFile,sSettingsFile);
  wcscpy(m_sTitle,sTitle);
  m_cBackground=cBackground;
  m_iMinResolution=irr::core::dimension2di(320,200);
  m_eDriver=irr::video::EDT_NULL;
}

void CSettings::createGUI() {
  //Create a software device
  m_pDevice=irr::createDevice(irr::video::EDT_SOFTWARE,irr::core::dimension2d<irr::u32>(230,365),16,false,false,false,0);
  m_pDevice->setWindowCaption(m_sTitle);

  strcpy(m_sSettingsFile,m_sSettingsFile);

  //read the settings from the settings file
  irr::io::IXMLReaderUTF8 *pXml=m_pDevice->getFileSystem()->createXMLReaderUTF8(m_sSettingsFile);

  m_iResolution=0;
  m_iDriver    =0;
  m_bFullscreen=false;

  for (irr::u32 i=0; i<8; i++) m_aAct[i]=true;

  if (pXml!=NULL) {
    while (pXml->read()) {
      if (!strcmp(pXml->getNodeName(),"resolution")) m_iResolution=atoi(pXml->getAttributeValue("value"));
      if (!strcmp(pXml->getNodeName(),"fullscreen")) m_bFullscreen=atoi(pXml->getAttributeValue("value"));
      if (!strcmp(pXml->getNodeName(),"driver"    )) m_iDriver=(irr::video::E_DRIVER_TYPE)atoi(pXml->getAttributeValue("value"));

      if (!strcmp(pXml->getNodeName(),"active")) {
        int iIdx=atoi(pXml->getAttributeValue("num"));
        m_aAct[iIdx]=atoi(pXml->getAttributeValue("active"))!=0;
      }

      if (!strcmp(pXml->getNodeName(),"count")) {
        int iIdx=atoi(pXml->getAttributeValue("num"));
        m_iCnt[iIdx]=atoi(pXml->getAttributeValue("cnt"));
      }
    }

    pXml->drop();
  }

  //init the Irrlicht classes
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_pSceneManager=m_pDevice->getSceneManager();
  m_pDriver=m_pDevice->getVideoDriver();

  m_bSettingsChanged=false;
  m_iClose=0;

  //add the static textfields
  m_pGuiEnv->addStaticText(m_sTitle,irr::core::rect<irr::s32>(5,5,125,18),true,true,0,-1,true);
  m_pGuiEnv->addStaticText(L"resolution"     ,irr::core::rect<irr::s32>(5,40,75,52),false,true,0,-1,true);
  m_pGuiEnv->addStaticText(L"fullscreen"     ,irr::core::rect<irr::s32>(5,65,75,77),false,true,0,-1,true);

  //add the "resolution" combobox
  m_pResolution=m_pGuiEnv->addComboBox(irr::core::rect<irr::s32>(80,39,180,53),0,4);

  //add the "fullscreen" checkboxes
  m_pFullscreen=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(80,65,94,77),0,5);

  irr::core::position2di  pos=irr::core::position2di ( 5,85),pos2=irr::core::position2di ( 35,85),pos3=irr::core::position2di (45,85);
  irr::core::dimension2di dim=irr::core::dimension2di(14,14),dim2=irr::core::dimension2di(150,14),dim3=irr::core::dimension2di(30,14);

  irr::gui::IGUICheckBox *p=NULL;
  irr::gui::IGUIComboBox *cb=NULL;

  cb=m_pGuiEnv->addComboBox(irr::core::rect<irr::s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"cars",irr::core::rect<irr::s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[0]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(irr::core::rect<irr::s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"planes",irr::core::rect<irr::s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[1]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(irr::core::rect<irr::s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"tanks",irr::core::rect<irr::s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[2]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(irr::core::rect<irr::s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"helicopters",irr::core::rect<irr::s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[3]);
  m_aBodyCount.push_back(cb);

  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"roads"           ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[0]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"road signs"      ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[1]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"targets"         ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[2]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"plane course"    ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[3]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"forests"         ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[4]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"trimesh terrain" ,irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=40; pos2.Y+=40; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[5]);
  p=m_pGuiEnv->addCheckBox(false,irr::core::rect<irr::s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"rearview monitor",irr::core::rect<irr::s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[6]);


  //add the "OK" and "Cancel" buttons
  m_pOk    =m_pGuiEnv->addButton(irr::core::rect<irr::s32>( 80,330,120,345),0,1,L"Start");
  m_pCancel=m_pGuiEnv->addButton(irr::core::rect<irr::s32>(125,330,175,345),0,2,L"Close");

  //we add all video modes with a bit depth of at least 16 to the "resolution" combobox
  m_pVModes=m_pDevice->getVideoModeList();
  for (irr::s32 i=0; i<m_pVModes->getVideoModeCount(); i++)
    if (m_pVModes->getVideoModeDepth(i)>=16) {
      irr::core::dimension2du res=m_pVModes->getVideoModeResolution(i);

      if (res.Width>=m_iMinResolution.Width && res.Height>=m_iMinResolution.Height) {
        _VRES *vRes=new _VRES();
        vRes->iWidth =res.Width ;
        vRes->iHeight=res.Height;
        vRes->iBpp   =m_pVModes->getVideoModeDepth(i);
        m_aVModes.push_back(vRes);
      }
    }

  //now we fill the "resolution" combobox with the drivers we added to the array
  for (irr::u32 i=0; i<m_aVModes.size(); i++) {
    wchar_t s[0xFF];
    _VRES *res=m_aVModes[i];
    swprintf(s,0xFE,L"%ix%i %i bpp",res->iWidth,res->iHeight,res->iBpp);
    m_pResolution->addItem(s);
  }

  //Now add an event receiver
  m_pDevice->setEventReceiver(this);

  m_pResolution->setSelected(m_iResolution);

  m_pFullscreen->setChecked(m_bFullscreen);
}

/**
 * The destructor saves the settings file if the settings were changed
 */
CSettings::~CSettings() {
  //Settings changed and "OK" button clicked?
  if (m_bSettingsChanged && m_iClose==1) {
    wchar_t res[0xFF],drv[0xFF];

    //Write settings to the settings file
    irr::io::IXMLWriter *pXml=m_pDevice->getFileSystem()->createXMLWriter(m_sSettingsFile);

    swprintf(res,0xFE,L"%i",m_iResolution);
    swprintf(drv,0xFE,L"%i",m_iDriver);

    irr::core::array<irr::core::stringw> aResN; aResN.push_back(irr::core::stringw(L"value"));
    irr::core::array<irr::core::stringw> aResV; aResV.push_back(irr::core::stringw(res));
    irr::core::array<irr::core::stringw> aDrvN; aDrvN.push_back(irr::core::stringw(L"value"));
    irr::core::array<irr::core::stringw> aDrvV; aDrvV.push_back(irr::core::stringw(drv));
    irr::core::array<irr::core::stringw> aFlsN; aFlsN.push_back(irr::core::stringw(L"value"));
    irr::core::array<irr::core::stringw> aFlsV; aFlsV.push_back(irr::core::stringw(m_bFullscreen?L"1":L"0"));
    irr::core::array<irr::core::stringw> aShdN; aShdN.push_back(irr::core::stringw(L"value"));

    pXml->writeXMLHeader();
    pXml->writeElement(L"settings"); pXml->writeLineBreak();
    pXml->writeElement(L"resolution",true,aResN,aResV); pXml->writeLineBreak();
    pXml->writeElement(L"fullscreen",true,aFlsN,aFlsV); pXml->writeLineBreak();
    pXml->writeElement(L"driver"    ,true,aDrvN,aDrvV); pXml->writeLineBreak();

    for (irr::u32 i=0; i<9; i++) {
      wchar_t s[0xFF];
      swprintf(s,0xFE,L"%i",i);
      pXml->writeElement(L"active",true,L"num",s,L"active",m_aAct[i]?L"1":L"0");
      pXml->writeLineBreak();
    }

    for (irr::u32 i=0; i<4; i++) {
      wchar_t s[0xFF],s2[0xFF];
      swprintf(s,0xFF,L"%i",i);
      swprintf(s2,0xFF,L"%i",m_iCnt[i]);
      pXml->writeElement(L"count",true,L"num",s,L"cnt",s2);
      pXml->writeLineBreak();
    }

    pXml->writeClosingTag(L"settings");
    pXml->drop();
  }

  for (irr::u32 i=0; i<m_aVModes.size(); i++) delete m_aVModes[i];
}

/**
 * Run the dialog
 */
irr::u32 CSettings::run() {
  createGUI();
  while (m_pDevice->run() && !m_iClose) {
    m_pDriver->beginScene(true,true,m_cBackground);

    m_pSceneManager->drawAll();
    m_pGuiEnv->drawAll();

    m_pDriver->endScene();
  }

  return m_iClose?m_iClose:2;
}

/**
 * Event receiver for the Irrlicht event pipeline
 */
bool CSettings::OnEvent(const irr::SEvent &event) {
  if (event.EventType==irr::EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED) {
      m_iClose=event.GUIEvent.Caller->getID();
    }

    if (event.GUIEvent.EventType==irr::gui::EGET_COMBO_BOX_CHANGED) {
      m_bSettingsChanged=true;
      switch (event.GUIEvent.Caller->getID()) {
        case 4: m_iResolution=m_pResolution->getSelected(); break;
      }
    }

    if (event.GUIEvent.EventType==irr::gui::EGET_CHECKBOX_CHANGED) {
      m_bSettingsChanged=true;
      switch (event.GUIEvent.Caller->getID()) {
        case 5: m_bFullscreen=m_pFullscreen->isChecked(); break;
      }

      for (irr::u32 i=0; i<m_aActiveBodies.size(); i++)
        if (event.GUIEvent.Caller==m_aActiveBodies[i]) {
          m_aAct[i]=m_aActiveBodies[i]->isChecked();
        }
    }

    if (event.GUIEvent.EventType==irr::gui::EGET_COMBO_BOX_CHANGED) {
      m_bSettingsChanged=true;
      for (irr::u32 i=0; i<m_aBodyCount.size(); i++)
        if (event.GUIEvent.Caller==m_aBodyCount[i])
          m_iCnt[i]=m_aBodyCount[i]->getSelected();
    }
  }

  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (!event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_RETURN:
          m_iClose=1;
          break;

        case irr::KEY_ESCAPE:
          m_iClose=2;
          break;

        default:
          break;
      }
    }
  }

  return false;
}

/**
 * This function drops the software device created to display the window and creates a new device
 * with the settings of the user
 */
irr::IrrlichtDevice *CSettings::createDeviceFromSettings() {
  m_pDevice->closeDevice();
  m_pDevice->run();
  m_pDevice->drop();
  _VRES *res=m_aVModes[m_iResolution];
  printf("\n\tcreating device with settings:\n\ndriver: %i\nresolution: (%i, %i)\n%i bits per pixel\nfullscreen: %s\n",m_iDriver,res->iWidth,res->iHeight,res->iBpp,m_bFullscreen?"YES":"NO");
  m_eDriver=irr::video::EDT_OPENGL;
  m_pDevice=irr::createDevice(m_eDriver,irr::core::dimension2du(res->iWidth,res->iHeight),res->iBpp,m_bFullscreen,false,false,0);
  return m_pDevice;
}
