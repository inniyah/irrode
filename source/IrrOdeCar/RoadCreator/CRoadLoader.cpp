  #include <CRoadLoader.h>
  #include <SAnimatedMesh.h>

  #include <CSegment.h>
  #include <CSurface.h>
  #include <CConnection.h>
  #include <CTextureParameters.h>

using namespace irr;

bool CRoadLoader::sameMaterial(const video::SMaterial &m1, const video::SMaterial &m2) {
  #ifdef _IRREDIT_PLUGIN
    core::stringc s1=m1.getTexture(0)->getName(),
                  s2=m2.getTexture(0)->getName();
  #else
    core::stringc s1=m1.getTexture(0)->getName().getPath(),
                  s2=m2.getTexture(0)->getName().getPath();
  #endif

  printf("%s .. %s\n",s1.c_str(),s2.c_str());
  return s1==s2;
}

void CRoadLoader::addBufferToArray(scene::IMeshBuffer *p, core::array<scene::IMeshBuffer *> &aBuffers) {
  bool bAdded=false;

  for (u32 j=0; j<aBuffers.size(); j++) {
    if (sameMaterial(p->getMaterial(),aBuffers[j]->getMaterial())) {
      scene::IMeshBuffer *pBuffer=aBuffers[j];
      pBuffer->append(p->getVertices(),p->getVertexCount(),p->getIndices(),p->getIndexCount());
      pBuffer->recalculateBoundingBox();
      bAdded=true;
    }
  }

  if (!bAdded) {
    p->getMaterial().Lighting=false;
    p->recalculateBoundingBox();
    aBuffers.push_back(p);
  }
}

CRoadLoader::CRoadLoader(IrrlichtDevice *pDevice) {
  m_pDevice=pDevice;
  m_sCurrentRoad="";
  m_pSurface=new CSurface(pDevice->getVideoDriver());
}

bool CRoadLoader::loadRoad(const core::stringc sName) {
  m_sCurrentRoad=sName;

  bool bRet=true;
  io::IReadFile *pFile=m_pDevice->getFileSystem()->createAndOpenFile(sName.c_str());

  if (pFile) {
    io::IXMLReader *pReader=m_pDevice->getFileSystem()->createXMLReader(pFile);
    if (pReader) {
      u32 iState=0,
          iSegTex=0,
          iConTex=0,
          iGndTex=0;

      CSegment    *pSeg=NULL;
      CConnection *pCon=NULL;

      while (pReader->read()) {
        if (pReader->getNodeType()==io::EXN_ELEMENT) {
          if (core::stringw(pReader->getNodeName())==L"Segment"   ) iState=1;
          if (core::stringw(pReader->getNodeName())==L"Connection") iState=2;
          if (core::stringw(pReader->getNodeName())==L"Materials" ) iState=3;
          if (core::stringw(pReader->getNodeName())==L"Surface"   ) iState=6;

          if (iState==1 && core::stringw(pReader->getNodeName())==L"TextureParams") iState=4;
          if (iState==2 && core::stringw(pReader->getNodeName())==L"TextureParams") iState=5;
          if (iState==6 && core::stringw(pReader->getNodeName())==L"TextureParams") iState=7;

          if (core::stringw(pReader->getNodeName())==L"attributes") {
            //Load a segment
            if (iState==1) {
              io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
              pAttr->read(pReader,true);
              pSeg=new CSegment(m_pDevice->getVideoDriver());
              m_lSegments.push_back(pSeg);
              pSeg->load(pAttr);
              pAttr->drop();
              iSegTex=0;
            }

            //Load a connection
            if (iState==2) {
              io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
              pAttr->read(pReader,true);
              pCon=new CConnection(m_pDevice->getVideoDriver());
              iConTex=0;
              pCon->load(pAttr);

              core::list<CSegment *>::Iterator it;
              for (it=m_lSegments.begin(); it!=m_lSegments.end(); it++) {
                CSegment *p=*it;
                if (p->getName()==pCon->getSegment1Name()) pCon->setSegment1NoInit(p);
                if (p->getName()==pCon->getSegment2Name()) pCon->setSegment2NoInit(p);
              }

              m_lConnections.push_back(pCon);
              pAttr->drop();
            }

            //Load texture parameters of a segment
            if (iState==4) {
              if (pSeg!=NULL && iSegTex<6) {
                io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
                pAttr->read(pReader,true);
                pSeg->getTextureParameters(iSegTex)->load(pAttr);
                pSeg->update();
                pAttr->drop();
              }
            }

            //Load texture parameters of a connection
            if (iState==5) {
              if (pCon!=NULL && iConTex<4) {
                io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
                pAttr->read(pReader,true);
                pCon->getTextureParameters(iConTex)->load(pAttr);
                pCon->update();
                pAttr->drop();
              }
            }

            //Load surface data
            if (iState==6) {
              io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
              pAttr->read(pReader,true);
              m_pSurface->load(pAttr);
              m_pSurface->setVisible(true);
              pAttr->drop();
            }

            //Load texture parameters of the surface
            if (iState==7) {
              io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
              pAttr->read(pReader,true);
              m_pSurface->getTextureParameters(iGndTex)->load(pAttr);
              m_pSurface->recalcMeshBuffer();
              pAttr->drop();
            }
          }
        }

        if (pReader->getNodeType()==io::EXN_ELEMENT_END) {
          switch (iState) {
            case 1:
              if (core::stringw(pReader->getNodeName())==L"Segment") {
                iState=0;
                pSeg=NULL;
                iSegTex=0;
              }
              break;

            case 2:
              if (core::stringw(pReader->getNodeName())==L"Connection") {
                iState=0;
                pCon=NULL;
                iConTex=0;
              }
              break;

            case 3:
              if (core::stringw(pReader->getNodeName())==L"Materials") iState=0;
              break;

            case 4:
              if (core::stringw(pReader->getNodeName())==L"TextureParams") {
                iState=1;
                iSegTex++;
              }
              break;

            case 5:
              if (core::stringw(pReader->getNodeName())==L"TextureParams") {
                iState=2;
                iConTex++;
              }
              break;

            case 6:
              if (core::stringw(pReader->getNodeName())==L"Surface") {
                iState=0;
                m_pSurface->recalcMeshBuffer();
              }
              break;

            case 7:
              if (core::stringw(pReader->getNodeName())==L"TextureParams") {
                iState=6;
                iGndTex++;
              }
              break;
          }
        }
      }
      m_sCurrentRoad=sName;
      pReader->drop();
    }
    else {
      printf("ERROR: can't create XML reader for file \"%s\".\n",sName.c_str());
      bRet=false;
    }
    pFile->drop();
  }
  else {
    printf("ERROR: can't open file \"%s\" for reading.\n",sName.c_str());
    bRet=false;
  }
  return bRet;
}

/**
 * This method saves a road to a file
 * @param sName the file to save the road to
 * @see CSegment::save
 * @see CConnection::save
 * @see CTextureParameters::save
 */
void CRoadLoader::saveRoad() {
  io::IWriteFile *pFile=m_pDevice->getFileSystem()->createAndWriteFile(m_sCurrentRoad.c_str(),false);
  if (pFile) {
    io::IXMLWriter *pWriter=m_pDevice->getFileSystem()->createXMLWriter(pFile);
    if (pWriter) {
      pWriter->writeXMLHeader();
      pWriter->writeElement(L"BulletByteRoadCreator",false);
      pWriter->writeLineBreak();

      //Write the segments
      core::list<CSegment *>::Iterator sit;
      for (sit=m_lSegments.begin(); sit!=m_lSegments.end(); sit++) {
        CSegment *p=*sit;
        pWriter->writeElement(L"Segment",false);
        pWriter->writeLineBreak();
        io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
        p->save(pAttr);
        pAttr->write(pWriter);
        pAttr->drop();

        //Write the segment texture parameters
        for (u32 i=0; i<6; i++) {
          core::stringw s=L"TextureParams";

          pWriter->writeElement(s.c_str(),false);
          pWriter->writeLineBreak();

          CTextureParameters *t=p->getTextureParameters(i);
          io::IAttributes *pTexAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
          t->save(pTexAttr);
          pTexAttr->write(pWriter);
          pTexAttr->drop();

          pWriter->writeClosingTag(s.c_str());
          pWriter->writeLineBreak();
        }

        pWriter->writeClosingTag(L"Segment");
        pWriter->writeLineBreak();
      }

      //Write the connections
      core::list<CConnection *>::Iterator cit;
      for (cit=m_lConnections.begin(); cit!=m_lConnections.end(); cit++) {
        CConnection *p=*cit;
        pWriter->writeElement(L"Connection",false);
        pWriter->writeLineBreak();
        io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
        p->save(pAttr);
        pAttr->write(pWriter);
        pAttr->drop();

        //Write the connection's texture parameters
        for (u32 i=0; i<4; i++) {
          core::stringw s=L"TextureParams";

          pWriter->writeElement(s.c_str(),false);
          pWriter->writeLineBreak();

          CTextureParameters *t=p->getTextureParameters(i);
          io::IAttributes *pTexAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
          t->save(pTexAttr);
          pTexAttr->write(pWriter);
          pTexAttr->drop();

          pWriter->writeClosingTag(s.c_str());
          pWriter->writeLineBreak();
        }
        pWriter->writeClosingTag(L"Connection");
        pWriter->writeLineBreak();
      }

      if (m_pSurface->isVisible()) {
        pWriter->writeElement(L"Surface",false);
        pWriter->writeLineBreak();
        io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
        m_pSurface->save(pAttr);
        pAttr->write(pWriter);
        pAttr->drop();

        for (u32 i=0; i<2; i++) {
          CTextureParameters *p=m_pSurface->getTextureParameters(i);
          if (p!=NULL) {
            core::stringw s=L"TextureParams";

            pWriter->writeElement(s.c_str(),false);
            pWriter->writeLineBreak();

            io::IAttributes *pAttr=m_pDevice->getFileSystem()->createEmptyAttributes();
            p->save(pAttr);
            pAttr->write(pWriter);
            pAttr->drop();

            pWriter->writeClosingTag(s.c_str());
            pWriter->writeLineBreak();
          }
        }
      }

      pWriter->writeClosingTag(L"BulletByteRoadCreator");
      pWriter->drop();
    }
    else printf("ERROR: unable to create XML writer for file \"%s\".\n",m_sCurrentRoad.c_str());
    pFile->drop();
  }
  else printf("ERROR: unable to write file \"%s\".\n",m_sCurrentRoad.c_str());
}

/**
 * Remove a road from memory
 */
void CRoadLoader::destroyRoad() {
  m_sCurrentRoad="";

  //remove all segments
  #ifdef _IRREDIT_PLUGIN
    while (m_lSegments.getSize()>0) {
  #else
    while (m_lSegments.size()>0) {
  #endif
    core::list<CSegment *>::Iterator it=m_lSegments.begin();
    CSegment *p=*it;
    m_lSegments.erase(it);
    delete p;
  }

  //remove all connections
  #ifdef _IRREDIT_PLUGIN
    while (m_lConnections.getSize()>0) {
  #else
    while (m_lConnections.size()>0) {
  #endif
    core::list<CConnection *>::Iterator it=m_lConnections.begin();
    CConnection *p=*it;
    m_lConnections.erase(it);
    delete p;
  }
}

core::list<CSegment    *> &CRoadLoader::getSegments   () { return m_lSegments   ; }
core::list<CConnection *> &CRoadLoader::getConnections() { return m_lConnections; }

const core::stringc CRoadLoader::getCurrentRoadName() { return m_sCurrentRoad; }
void CRoadLoader::setCurrentRoadName(const core::stringc s) { m_sCurrentRoad=s; }

scene::IAnimatedMesh *CRoadLoader::createMesh() {
  core::array<core::aabbox3df> aBoxes;
  core::array<scene::IMeshBuffer *> aBuffers;

  core::list<CSegment *>::Iterator sit;
  for (sit=m_lSegments.begin(); sit!=m_lSegments.end(); sit++) {
    CSegment *pSeg=*sit;
    for (u32 i=0; i<6; i++) {
      scene::IMeshBuffer *p=pSeg->getMeshBuffer(i);
      aBoxes.push_back(p->getBoundingBox());
      addBufferToArray(p,aBuffers);
    }
  }

  core::list<CConnection *>::Iterator cit;
  for (cit=m_lConnections.begin(); cit!=m_lConnections.end(); cit++) {
    CConnection *pCon=*cit;
    for (u32 i=0; i<4; i++) {
      scene::IMeshBuffer *p=pCon->getMeshBuffer(i);
      aBoxes.push_back(p->getBoundingBox());
      addBufferToArray(p,aBuffers);
    }
  }

  if (m_pSurface->isVisible()) {
    for (u32 i=0; i<2; i++) {
      scene::IMeshBuffer *p=m_pSurface->getMeshBuffer(i);
      if (p!=NULL) {
        aBoxes.push_back(p->getBoundingBox());
        addBufferToArray(p,aBuffers);
      }
    }
  }

  for (u32 i=0; i<aBuffers.size(); i++) {
    scene::IMeshBuffer *p=aBuffers[i];
    for (u32 j=0; j<p->getVertexCount(); j++) {
      ((video::S3DVertex *)p->getVertices())[j].Color=video::SColor(0xFF,0xFF,0xFF,0xFF);
    }
  }

  scene::SMesh *pMesh=new scene::SMesh();
  for (u32 i=0; i<aBuffers.size(); i++) pMesh->addMeshBuffer(aBuffers[i]);
  printf("\t\t**** aBuffers.size: %i\n",aBuffers.size());

  core::aabbox3df cBox=pMesh->getBoundingBox();

  for (u32 i=0; i<aBoxes.size(); i++) cBox.addInternalBox(aBoxes[i]);
  pMesh->setBoundingBox(cBox);

	scene::SAnimatedMesh *pAnimatedMesh=new scene::SAnimatedMesh();
	pAnimatedMesh->addMesh(pMesh);

  return pAnimatedMesh;
}

bool CRoadLoader::deleteConnection(CConnection *pToDelete) {
  core::list<CConnection *>::Iterator it;
  for (it=m_lConnections.begin(); it!=m_lConnections.end(); it++) {
    CConnection *p=*it;
    if (p==pToDelete) {
      m_lConnections.erase(it);
      delete p;
      return true;
    }
  }
  return false;
}

bool CRoadLoader::deleteSegment(CSegment *pToDelete) {
  core::list<CSegment *>::Iterator it;
  for (it=m_lSegments.begin(); it!=m_lSegments.end(); it++) {
    CSegment *p=*it;
    if (p==pToDelete) {
      m_lSegments.erase(it);
      delete p;
      return true;
    }
  }
  return false;
}