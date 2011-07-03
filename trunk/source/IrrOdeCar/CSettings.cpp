  #include <CSettings.h>

/**
 * The constructor
 * @param scene to be loaded and displayed in the background. Can be "NULL" if no scene should be shown
 * @param the XML file to read and store the settings
 * @param title of the window
 * @param background color of the window
 */
CSettings::CSettings(const c8 *sSettingsFile, const wchar_t *sTitle, SColor cBackground) {
  strcpy(m_sSettingsFile,sSettingsFile);
  wcscpy(m_sTitle,sTitle);
  m_cBackground=cBackground;
  m_iMinResolution=dimension2di(320,200);
}

void CSettings::createGUI() {
  //Create a software device
  m_pDevice=createDevice(EDT_SOFTWARE, dimension2d<u32>(230,360), 16, false, false, false, 0);
  m_pDevice->setWindowCaption(m_sTitle);

  strcpy(m_sSettingsFile,m_sSettingsFile);

  //read the settings from the settings file
  IXMLReaderUTF8 *pXml=m_pDevice->getFileSystem()->createXMLReaderUTF8(m_sSettingsFile);

  m_iResolution=0;
  m_iDriver    =0;
  m_bFullscreen=false;

  for (u32 i=0; i<8; i++) m_aAct[i]=true;

  bool bFillDriverList=m_aDrvs.size()==0;

  if (pXml!=NULL) {
    while (pXml->read()) {
      //This is special: the video drivers to choose from need to be in the XML file
      if (bFillDriverList)
        if (!strcmp(pXml->getNodeName(),"videodriver")) {
          _DRV *drv=new _DRV();
          wcscpy(drv->sName,stringw(pXml->getAttributeValue("name")).c_str());
          drv->iDriver=(E_DRIVER_TYPE)atoi(pXml->getAttributeValue("id"  ));
          m_aDrvs.push_back(drv);
        }

      if (!strcmp(pXml->getNodeName(),"resolution")) m_iResolution=atoi(pXml->getAttributeValue("value"));
      if (!strcmp(pXml->getNodeName(),"fullscreen")) m_bFullscreen=atoi(pXml->getAttributeValue("value"));
      if (!strcmp(pXml->getNodeName(),"driver"    )) m_iDriver    =(E_DRIVER_TYPE)atoi(pXml->getAttributeValue("value"));

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
  m_pGuiEnv->addStaticText(m_sTitle,rect<s32>(5,5,125,18),true,true,0,-1,true);
  m_pGuiEnv->addStaticText(L"graphics driver",rect<s32>(5,40,75, 52),false,true,0,-1,true);
  m_pGuiEnv->addStaticText(L"resolution"     ,rect<s32>(5,65,75, 77),false,true,0,-1,true);
  m_pGuiEnv->addStaticText(L"fullscreen"     ,rect<s32>(5,90,75,102),false,true,0,-1,true);

  //add the "driver" and "resolution" comboboxes
  m_pDrivers   =m_pGuiEnv->addComboBox(rect<s32>(80,39,180,53),0,3);
  m_pResolution=m_pGuiEnv->addComboBox(rect<s32>(80,64,180,78),0,4);

  //add the "fullscreen" checkboxes
  m_pFullscreen=m_pGuiEnv->addCheckBox(false,rect<s32>(80,88,94,102),0,5);

  position2di pos=position2di(5,130),pos2=position2di(35,130),pos3=position2di(45,130);
  dimension2di dim=dimension2di(14,14),dim2=dimension2di(150,14),dim3=dimension2di(30,14);

  IGUICheckBox *p=NULL;
  IGUIComboBox *cb=NULL;

  cb=m_pGuiEnv->addComboBox(rect<s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2"); cb->addItem(L"3"); cb->addItem(L"4");
  m_pGuiEnv->addStaticText(L"cars",rect<s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[0]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(rect<s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2"); cb->addItem(L"3"); cb->addItem(L"4");
  m_pGuiEnv->addStaticText(L"planes",rect<s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[1]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(rect<s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"tanks",rect<s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[2]);
  m_aBodyCount.push_back(cb);

  cb=m_pGuiEnv->addComboBox(rect<s32>(pos,dim3));
  cb->addItem(L"0"); cb->addItem(L"1"); cb->addItem(L"2");
  m_pGuiEnv->addStaticText(L"helicopters",rect<s32>(pos3,dim2),false,true,0,-1,true);
  pos.Y+=20; pos2.Y+=20; pos3.Y+=20;
  cb->setSelected(m_iCnt[3]);
  m_aBodyCount.push_back(cb);

  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"roads"          ,rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[0]);
  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"bumps"          ,rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[1]);
  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"targets"        ,rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[2]);
  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"plane course"   ,rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[3]);
  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"forests"        ,rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[4]);
  p=m_pGuiEnv->addCheckBox(false,rect<s32>(pos,dim),0,23); m_pGuiEnv->addStaticText(L"trimesh terrain",rect<s32>(pos2,dim2),false,true,0,-1,true); pos.Y+=20; pos2.Y+=20; m_aActiveBodies.push_back(p); p->setChecked(m_aAct[5]);


  //add the "OK" and "Cancel" buttons
  m_pOk    =m_pGuiEnv->addButton(rect<s32>( 80,340,120,355),0,1,L"Start");
  m_pCancel=m_pGuiEnv->addButton(rect<s32>(125,340,175,355),0,2,L"Close");

  //If no driver information was found in the settings file we initialize the driver list with all available drivers
  if (m_aDrvs.size()==0) {
    _DRV *drv;
    drv=new _DRV(); drv->iDriver=EDT_SOFTWARE     ; wcscpy(drv->sName,L"Software"     ); m_aDrvs.push_back(drv);
    drv=new _DRV(); drv->iDriver=EDT_BURNINGSVIDEO; wcscpy(drv->sName,L"BurningsVideo"); m_aDrvs.push_back(drv);
    drv=new _DRV(); drv->iDriver=EDT_DIRECT3D8    ; wcscpy(drv->sName,L"Direct3D8"    ); m_aDrvs.push_back(drv);
    drv=new _DRV(); drv->iDriver=EDT_DIRECT3D9    ; wcscpy(drv->sName,L"Direct3D9"    ); m_aDrvs.push_back(drv);
    drv=new _DRV(); drv->iDriver=EDT_OPENGL       ; wcscpy(drv->sName,L"OpenGL"       ); m_aDrvs.push_back(drv);
  }

  //If driver information were found we just add the drivers from the XML file
  for (u32 i=0; i<m_aDrvs.size(); i++) m_pDrivers->addItem(m_aDrvs[i]->sName);

  //we add all video modes with a bit depth of at least 16 to the "resolution" combobox
  m_pVModes=m_pDevice->getVideoModeList();
  for (s32 i=0; i<m_pVModes->getVideoModeCount(); i++)
    if (m_pVModes->getVideoModeDepth(i)>=16) {
      dimension2du res=m_pVModes->getVideoModeResolution(i);

      if (res.Width>=m_iMinResolution.Width && res.Height>=m_iMinResolution.Height) {
        _VRES *vRes=new _VRES();
        vRes->iWidth =res.Width ;
        vRes->iHeight=res.Height;
        vRes->iBpp   =m_pVModes->getVideoModeDepth(i);
        m_aVModes.push_back(vRes);
      }
    }

  //now we fill the "resolution" combobox with the drivers we added to the array
  for (u32 i=0; i<m_aVModes.size(); i++) {
    wchar_t s[0xFF];
    _VRES *res=m_aVModes[i];
    swprintf(s,0xFE,L"%ix%i %i bpp",res->iWidth,res->iHeight,res->iBpp);
    m_pResolution->addItem(s);
  }

  //Now add an event receiver
  m_pDevice->setEventReceiver(this);

  //we init the GUI items with the values read from the settings XML file
  m_pDrivers->setSelected(m_iDriver);

  m_pResolution->setSelected(m_iResolution);

  m_pFullscreen->setChecked(m_bFullscreen);

  m_pDevice->getCursorControl()->setVisible(true);
}

/**
 * The destructor saves the settings file if the settings were changed
 */
CSettings::~CSettings() {
  //Settings changed and "OK" button clicked?
  if (m_bSettingsChanged && m_iClose==1) {
    wchar_t res[0xFF],drv[0xFF];

    //Write settings to the settings file
    IXMLWriter *pXml=m_pDevice->getFileSystem()->createXMLWriter(m_sSettingsFile);

    swprintf(res,0xFE,L"%i",m_iResolution);
    swprintf(drv,0xFE,L"%i",m_iDriver);

    array<stringw> aResN; aResN.push_back(stringw(L"value"));
    array<stringw> aResV; aResV.push_back(stringw(res));
    array<stringw> aDrvN; aDrvN.push_back(stringw(L"value"));
    array<stringw> aDrvV; aDrvV.push_back(stringw(drv));
    array<stringw> aFlsN; aFlsN.push_back(stringw(L"value"));
    array<stringw> aFlsV; aFlsV.push_back(stringw(m_bFullscreen?L"1":L"0"));
    array<stringw> aShdN; aShdN.push_back(stringw(L"value"));

    pXml->writeXMLHeader();
    pXml->writeElement(L"settings"); pXml->writeLineBreak();
    for (u32 i=0; i<m_aDrvs.size(); i++) {
      wchar_t s[0xFF];
      swprintf(s,0xFE,L"%i",m_aDrvs[i]->iDriver);
      pXml->writeElement(L"videodriver",true,L"name",m_aDrvs[i]->sName,L"id",s);
      pXml->writeLineBreak();
    }
    pXml->writeElement(L"resolution",true,aResN,aResV); pXml->writeLineBreak();
    pXml->writeElement(L"fullscreen",true,aFlsN,aFlsV); pXml->writeLineBreak();
    pXml->writeElement(L"driver"    ,true,aDrvN,aDrvV); pXml->writeLineBreak();

    for (u32 i=0; i<9; i++) {
      wchar_t s[0xFF];
      swprintf(s,0xFE,L"%i",i);
      pXml->writeElement(L"active",true,L"num",s,L"active",m_aAct[i]?L"1":L"0");
      pXml->writeLineBreak();
    }

    for (u32 i=0; i<4; i++) {
      wchar_t s[0xFF],s2[0xFF];
      swprintf(s,0xFF,L"%i",i);
      swprintf(s2,0xFF,L"%i",m_iCnt[i]);
      pXml->writeElement(L"count",true,L"num",s,L"cnt",s2);
      pXml->writeLineBreak();
    }

    pXml->writeClosingTag(L"settings");
    pXml->drop();
  }

  for (u32 i=0; i<m_aVModes.size(); i++) delete m_aVModes[i];
}

/**
 * Run the dialog
 */
u32 CSettings::run() {
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
bool CSettings::OnEvent(const SEvent &event) {
  if (event.EventType==EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==EGET_BUTTON_CLICKED) {
      m_iClose=event.GUIEvent.Caller->getID();
    }

    if (event.GUIEvent.EventType==EGET_COMBO_BOX_CHANGED) {
      m_bSettingsChanged=true;
      switch (event.GUIEvent.Caller->getID()) {
        case 3: m_iDriver=m_pDrivers->getSelected(); break;
        case 4: m_iResolution=m_pResolution->getSelected(); break;
      }
    }

    if (event.GUIEvent.EventType==EGET_CHECKBOX_CHANGED) {
      m_bSettingsChanged=true;
      switch (event.GUIEvent.Caller->getID()) {
        case 5: m_bFullscreen=m_pFullscreen->isChecked(); break;
      }

      for (u32 i=0; i<m_aActiveBodies.size(); i++)
        if (event.GUIEvent.Caller==m_aActiveBodies[i]) {
          m_aAct[i]=m_aActiveBodies[i]->isChecked();
        }
    }

    if (event.GUIEvent.EventType==irr::gui::EGET_COMBO_BOX_CHANGED) {
      m_bSettingsChanged=true;
      for (u32 i=0; i<m_aBodyCount.size(); i++)
        if (event.GUIEvent.Caller==m_aBodyCount[i])
          m_iCnt[i]=m_aBodyCount[i]->getSelected();
    }
  }

  if (event.EventType==EET_KEY_INPUT_EVENT) {
    if (!event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case KEY_RETURN:
          m_iClose=1;
          break;

        case KEY_ESCAPE:
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
IrrlichtDevice *CSettings::createDeviceFromSettings() {
  m_pDevice->closeDevice();
  m_pDevice->run();
  m_pDevice->drop();
  _VRES *res=m_aVModes[m_iResolution];
  printf("\n\tcreating device with settings:\n\ndriver: %i\nresolution: (%i, %i)\n%i bits per pixel\nfullscreen: %s\n",m_iDriver,res->iWidth,res->iHeight,res->iBpp,m_bFullscreen?"YES":"NO");
  m_pDevice=createDevice(m_aDrvs[m_iDriver]->iDriver,dimension2du(res->iWidth,res->iHeight),res->iBpp,m_bFullscreen,false,false,0);
  return m_pDevice;
}

void CSettings::addValidDriver(const wchar_t *sName, E_DRIVER_TYPE iDriver) {
  _DRV *drv=new _DRV();
  wcscpy(drv->sName,sName);
  drv->iDriver=iDriver;
  m_aDrvs.push_back(drv);
}
