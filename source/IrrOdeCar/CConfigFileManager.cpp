  #include <CConfigFileManager.h>

CConfigFileManager *CConfigFileManager::getSharedInstance() {
  static CConfigFileManager cManager;
  return &cManager;
}

void CConfigFileManager::addReader(IConfigFileReader *pReader) {
  m_lReaders.push_back(pReader);
}

void CConfigFileManager::addWriter(IConfigFileWriter *pWriter) {
  m_lWriters.push_back(pWriter);
}

void CConfigFileManager::removeReader(IConfigFileReader *pReader) {
  irr::core::list<IConfigFileReader *>::Iterator it;
  for (it=m_lReaders.begin(); it!=m_lReaders.end(); it++) if ((*it)==pReader) { m_lReaders.erase(it); return; }
}

void CConfigFileManager::removeWriter(IConfigFileWriter *pWriter) {
  irr::core::list<IConfigFileWriter *>::Iterator it;
  for (it=m_lWriters.begin(); it!=m_lWriters.end(); it++) if ((*it)==pWriter) { m_lWriters.erase(it); return; }
}


void CConfigFileManager::clearReadersWriters() {
  m_lReaders.clear();
  m_lWriters.clear();
}

void CConfigFileManager::loadConfig(irr::IrrlichtDevice *pDevice, const irr::c8 *sFileName) {
  irr::io::IXMLReaderUTF8 *pXml=pDevice->getFileSystem()->createXMLReaderUTF8(sFileName);
  bool bSettings=false;

  if (pXml) {
    while (pXml->read()) {
      bSettings=bSettings || (!strcmp(pXml->getNodeName(),"settings") && pXml->getNodeType()==irr::io::EXN_ELEMENT);
      if (bSettings) {
        irr::core::list<IConfigFileReader *>::Iterator it;
        for (it=m_lReaders.begin(); it!=m_lReaders.end(); it++) {
          (*it)->readConfig(pXml);
        }
      }
    }

    pXml->drop();
  }
}

void CConfigFileManager::writeConfig(irr::IrrlichtDevice *pDevice, const irr::c8 *sFileName) {
  irr::io::IXMLWriter *pXml=pDevice->getFileSystem()->createXMLWriter(sFileName);

  if (pXml) {
    pXml->writeXMLHeader();
    pXml->writeElement(L"settings");
    pXml->writeLineBreak();

    irr::core::list<IConfigFileWriter *>::Iterator it;
    for (it=m_lWriters.begin(); it!=m_lWriters.end(); it++) {
      (*it)->writeConfig(pXml);
    }

    pXml->writeClosingTag(L"settings");
    pXml->writeLineBreak();
    pXml->drop();
  }
}
