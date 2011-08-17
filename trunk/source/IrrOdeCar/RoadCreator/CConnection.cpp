  #include <CTextureParameters.h>
  #include <CConnection.h>
  #include <CSegment.h>

  #define _RED video::SColor(0xFF,0xFF,0xE0,0xE0)
  #define _GREEN video::SColor(0xFF,0xE0,0xFF,0xE0)
  #define _BLUE video::SColor(0xFF,0xE0,0xE0,0xFF)

using namespace irr;

/**
 * Using this method you can add a single vertex to a temporarily used array of vertices. This array
 * is searched to see if the position of any vertex inside mathes the new vertex, and if this is true
 * the normals of those vertices are interpolated
 * @param vtx the vertex to add
 * @param aTmp the array of vertices
 * @return the index of the newly added vertex
 * @see CConnection::addToBuffers
 */
u16 CConnection::addToTempVertexBuffer(video::S3DVertex vtx, core::array<video::S3DVertex> &aTmp) {
  u16 iRet=0;

  //iterate through the array of already added vertices ...
  for (u16 i=0; i<aTmp.size(); i++) {
    //... compare the position and if one matches ...
    if (vtx.Pos.equals(aTmp[i].Pos)) {
      //... calculate the new normal value and set it.
      aTmp[i].Normal=(aTmp[i].Normal+vtx.Normal)/2;
      vtx.Normal=aTmp[i].Normal;
    }
  }

  iRet=aTmp.size();
  aTmp.push_back(vtx);

  return iRet;
}

/**
 * This method creates vertices out of the four vectors in the first argument
 * and adds those to the array and their indices to the index array
 * @param v the input vectors
 * @param aVerts the output array. The input vectors will be added to this array
 * @param aIdx the index output. The indices of the vertex output array are stored here
 * @param b boolean for multi-colored appearance in the editor
 * @param iIdx index of the side that the vectors are for (road, bottom, left, right)
 * @see CConnection::recalcMeshBuffer
 */
void CConnection::addToBuffers(core::vector3df v[], core::array<video::S3DVertex> &aVerts, core::array<u16> &aIdx, bool b, u32 iIdx) {
  video::S3DVertex vt[4];                                   //the vertex array
  core::vector3df norm=(v[0]-v[1]).crossProduct(v[0]-v[2]); //the normal
  u16 idx[6];                                               //the index array

  //Normalize the normal and invert it if the vertices should be flipped
  norm.normalize();
  if (m_bFlipVertices) norm=-norm;

  //Calculate the length and the width defined by the vectors to add
  f32 fLen=(((v[0]-v[2]).getLength())+((v[1]-v[3])).getLength())/2.0f,
      fWid=(((v[0]-v[1]).getLength())+((v[2]-v[3])).getLength())/2.0f;

  //Calculate the helper variables for texture calculations
  f32 diff=fWid!=0.0f?m_pTexParams[iIdx]->getScale()*fLen/fWid:1.0f,
      f1=m_bFlipVertices?1.0f:0.0f,f2=m_bFlipVertices?0.0f:1.0f;

  if (iIdx==0 || iIdx==1) {
    //we need to find out the biggest width of the
    //defined road. To do this we check the width
    //of the road (iIdx==0) and of the bottom (iIdx==1)
    if (m_fRoadWidth<fWid) m_fRoadWidth=fWid;
  }
  else
    //Recalculate the helper variables for texture mapping if we add the side of the road
    if (iIdx==2) {
      diff=m_fRoadWidth!=0.0f?m_pTexParams[iIdx]->getScale()*fWid/m_fRoadWidth:1.0f;
      f1=m_fRoadWidth!=0.0f?m_bFlipVertices?fWid/m_fRoadWidth:0.0f:0.0f;
      f2=m_fRoadWidth!=0.0f?m_bFlipVertices?0.0f:fWid/m_fRoadWidth:1.0f;
    }
    else {
      diff=m_fRoadWidth!=0.0f?m_pTexParams[iIdx]->getScale()*fLen/m_fRoadWidth:1.0f;
      f1=m_fRoadWidth!=0.0f?m_bFlipVertices?fLen/m_fRoadWidth:0.0f:0.0f;
      f2=m_fRoadWidth!=0.0f?m_bFlipVertices?0.0f:fLen/m_fRoadWidth:1.0f;
    }

  //Texture stretched? Reset the Diff value
  if (m_pTexParams[iIdx]->getStretch()) {
    diff=1.0f/((f32)(m_iSteps));
  }

  core::vector2df cCoord[4];

  //Now we fill a vector array with the texture coordinates
  //depending on the rotation
  switch (m_pTexParams[iIdx]->getRotate()) {
    case 0:   //Rotation 0
      cCoord[0]=core::vector2df(f1,m_fTex);
      cCoord[1]=core::vector2df(f2,m_fTex);
      cCoord[2]=core::vector2df(f1,m_fTex+diff);
      cCoord[3]=core::vector2df(f2,m_fTex+diff);
      m_fTex+=diff;
      break;

    case 1:   //Rotation 90
      cCoord[0]=core::vector2df(m_fTex     ,f2);
      cCoord[1]=core::vector2df(m_fTex     ,f1);
      cCoord[2]=core::vector2df(m_fTex+diff,f2);
      cCoord[3]=core::vector2df(m_fTex+diff,f1);
      m_fTex+=diff;
      break;

    case 2:   //Rotation 180
      cCoord[0]=core::vector2df(f2,m_fTex);
      cCoord[1]=core::vector2df(f1,m_fTex);
      cCoord[2]=core::vector2df(f2,m_fTex-diff);
      cCoord[3]=core::vector2df(f1,m_fTex-diff);
      m_fTex-=diff;
      break;

    case 3:   //Rotation 270
      cCoord[0]=core::vector2df(m_fTex     ,f1);
      cCoord[1]=core::vector2df(m_fTex     ,f2);
      cCoord[2]=core::vector2df(m_fTex-diff,f1);
      cCoord[3]=core::vector2df(m_fTex-diff,f2);
      m_fTex-=diff;
      break;
  }

  //Next we fill the temp array of vertices
  vt[0]=video::S3DVertex(v[0],norm,b?_RED:_BLUE,cCoord[0]);
  vt[1]=video::S3DVertex(v[1],norm,b?_RED:_BLUE,cCoord[1]);
  vt[2]=video::S3DVertex(v[2],norm,b?_RED:_BLUE,cCoord[2]);
  vt[3]=video::S3DVertex(v[3],norm,b?_RED:_BLUE,cCoord[3]);

  //make sure the texture coordinate starts between 0 and 1
  while (m_fTex>= 1.0f) m_fTex-=1.0f;
  while (m_fTex<=-1.0f) m_fTex+=1.0f;

  switch (iIdx) {
    //the ground vertices need some extra treatment
    case 1:
      vt[0].TCoords.X=-vt[0].TCoords.X; vt[0].Normal=-vt[0].Normal;
      vt[1].TCoords.X=-vt[1].TCoords.X; vt[1].Normal=-vt[1].Normal;
      vt[2].TCoords.X=-vt[2].TCoords.X; vt[2].Normal=-vt[2].Normal;
      vt[3].TCoords.X=-vt[3].TCoords.X; vt[3].Normal=-vt[3].Normal;

      if (m_bFlipVertices) {
        u16 iTmp;

        iTmp=addToTempVertexBuffer(vt[0],aVerts); idx[1]=iTmp;
        iTmp=addToTempVertexBuffer(vt[1],aVerts); idx[2]=iTmp; idx[3]=iTmp;
        iTmp=addToTempVertexBuffer(vt[2],aVerts); idx[0]=iTmp; idx[5]=iTmp;
        iTmp=addToTempVertexBuffer(vt[3],aVerts);              idx[4]=iTmp;
      }
      else {
        u16 iTmp;

        iTmp=addToTempVertexBuffer(vt[0],aVerts); idx[1]=iTmp;
        iTmp=addToTempVertexBuffer(vt[1],aVerts); idx[0]=iTmp; idx[3]=iTmp;
        iTmp=addToTempVertexBuffer(vt[2],aVerts); idx[2]=iTmp; idx[4]=iTmp;
        iTmp=addToTempVertexBuffer(vt[3],aVerts);              idx[5]=iTmp;
      }
      break;

    default:
      if (m_bFlipVertices) {
        u16 iTmp;

        iTmp=addToTempVertexBuffer(vt[0],aVerts); idx[2]=iTmp;
        iTmp=addToTempVertexBuffer(vt[1],aVerts); idx[1]=iTmp; idx[5]=iTmp;
        iTmp=addToTempVertexBuffer(vt[2],aVerts); idx[0]=iTmp; idx[3]=iTmp;
        iTmp=addToTempVertexBuffer(vt[3],aVerts);              idx[4]=iTmp;
      }
      else {
        u16 iTmp;

        iTmp=addToTempVertexBuffer(vt[0],aVerts); idx[0]=iTmp;
        iTmp=addToTempVertexBuffer(vt[1],aVerts); idx[1]=iTmp; idx[4]=iTmp;
        iTmp=addToTempVertexBuffer(vt[2],aVerts); idx[2]=iTmp; idx[3]=iTmp;
        iTmp=addToTempVertexBuffer(vt[3],aVerts);              idx[5]=iTmp;
      }
      break;
  }

  //now we add the indices to the output array
  for (u32 i=0; i<6; i++) aIdx.push_back(idx[i]);
}

/**
 * Recalculates the meshbuffer after some attribute was changed
 * @see CConnection::update
 */
void CConnection::recalcMeshBuffer() {
  //Delete all meshbuffers allocated previously
  for (u32 i=0; i<4; i++)
    if (m_pMeshBuffer[i]!=NULL) {
      m_pMeshBuffer[i]->drop();
      m_pMeshBuffer[i]=NULL;
    }

  //Return without doing any further things if the connection is not connected
  if (m_pSegment1==NULL || m_pSegment2==NULL) return;

  core::vector3df p1[4],p2[4],vOld[2];

  //initialize the texture offset for the road
  m_fTex=m_pTexParams[0]->getOffset();

  //initialize the beginning and the ending point of the first line that is calculated
  switch (m_iSegment1Border) {
    case 0: p1[0]=m_pSegment1->getPoint(0); p2[0]=m_pSegment1->getPoint(1); break;
    case 1: p1[0]=m_pSegment1->getPoint(0); p2[0]=m_pSegment1->getPoint(3); break;
    case 2: p1[0]=m_pSegment1->getPoint(3); p2[0]=m_pSegment1->getPoint(2); break;
    case 3: p1[0]=m_pSegment1->getPoint(2); p2[0]=m_pSegment1->getPoint(1); break;
  }

  //initialize the beginning and the ending point of the second line that is calculated
  switch (m_iSegment2Border) {
    case 0: p1[1]=m_pSegment2->getPoint(0); p2[1]=m_pSegment2->getPoint(1); break;
    case 1: p1[1]=m_pSegment2->getPoint(0); p2[1]=m_pSegment2->getPoint(3); break;
    case 2: p1[1]=m_pSegment2->getPoint(3); p2[1]=m_pSegment2->getPoint(2); break;
    case 3: p1[1]=m_pSegment2->getPoint(2); p2[1]=m_pSegment2->getPoint(1); break;
  }

  //create new meshbuffers
  m_pMeshBuffer[0]=new scene::SMeshBuffer();
  m_pMeshBuffer[1]=new scene::SMeshBuffer();
  m_pMeshBuffer[2]=new scene::SMeshBuffer();
  m_pMeshBuffer[3]=new scene::SMeshBuffer();

  //Temp arrays for the vertices of the top and bottom of the road and for the indices
  core::array<video::S3DVertex> aVerts,aBotVerts;
  core::array<u16> aIdx,aBotIdx;

  //if the connection wants to be flipped we flip it
  if (m_bFlipConnection) {
    core::vector3df v=p1[1];
    p1[1]=p2[1];
    p2[1]=v;
  }

  bool b=false;

  //now we calculate the actual positions depending on the selected type.
  switch (m_eType) {
    case eBezier1: {
        //initialize the variables for the previous points
        vOld[0]=p1[0]; vOld[1]=p2[0];

        //We start the calculation one step off so that no zero-sized
        //polygon is created at the start of the connection
        for (f32 f=1.0f/((f32)m_iSteps); f<1.0f; f+=1.0f/((f32)m_iSteps)) {
          //calculate the positions of the vertices,...
          core::vector3df v1=getBezier1(p1,f),
                          v2=getBezier1(p2,f),
                          vToAdd[]={ vOld[0],vOld[1],v1,v2 };

          //...add it to the vertex (and index) buffers...
          addToBuffers(vToAdd,aVerts,aIdx,b,0);

          //...and set the old vectors to the calculated positions for the next polygon
          vOld[0]=v1;
          vOld[1]=v2;

          b=!b;
        }

        //now we need to add a polygon with the end of our connecting lines
        core::vector3df v[4]={ vOld[0], vOld[1], p1[1], p2[1] };
        addToBuffers(v,aVerts,aIdx,b,0);
      }
      break;

    case eBezier2: {
        //initialize the variables for the previous points
        vOld[0]=p1[0]; vOld[1]=p2[0];

        //now we shift the point arrays because we need
        //to insert the helppoint for the Bezier2 calculation
        p1[2]=p1[1]; p1[1]=m_vHelpPoints[0];
        p2[2]=p2[1]; p2[1]=m_vHelpPoints[2];

        //We start the calculation one step off so that no zero-sized
        //polygon is created at the start of the connection
        for (f32 f=1.0f/((f32)m_iSteps); f<1.0f; f+=1.0f/((f32)m_iSteps)) {
          //calculate the positions of the vertices,...
          core::vector3df v1=getBezier2(p1,f),
                          v2=getBezier2(p2,f),
                          vToAdd[]={ vOld[0],vOld[1],v1,v2 };

          //...add it to the vertex (and index) buffers...
          addToBuffers(vToAdd,aVerts,aIdx,b,0);

          //...and set the old vectors to the calculated positions for the next polygon
          vOld[0]=v1;
          vOld[1]=v2;

          b=!b;
        }

        //now we need to add a polygon with the end of our connecting lines
        core::vector3df v[4]={ vOld[0], vOld[1], p1[2], p2[2] };
        addToBuffers(v,aVerts,aIdx,b,0);
      }
      break;

    case eBezier3: {
        //initialize the variables for the previous points
        vOld[0]=p1[0]; vOld[1]=p2[0];

        //now we shift the point arrays because we need
        //to insert the 2 helppoints for the Bezier3 calculation
        p1[3]=p1[1]; p1[1]=m_vHelpPoints[0]; p1[2]=m_vHelpPoints[1];
        p2[3]=p2[1]; p2[1]=m_vHelpPoints[2]; p2[2]=m_vHelpPoints[3];

        //We start the calculation one step off so that no zero-sized
        //polygon is created at the start of the connection
        for (f32 f=1.0f/((f32)m_iSteps); f<1.0f; f+=1.0f/((f32)m_iSteps)) {
          //calculate the positions of the vertices,...
          core::vector3df v1=getBezier3(p1,f),
                          v2=getBezier3(p2,f),
                          vToAdd[]={ vOld[0],vOld[1],v1,v2 };

          //...add it to the vertex (and index) buffers...
          addToBuffers(vToAdd,aVerts,aIdx,b,0);

          //...and set the old vectors to the calculated positions for the next polygon
          vOld[0]=v1;
          vOld[1]=v2;

          b=!b;
        }

        //now we need to add a polygon with the end of our connecting lines
        core::vector3df v[4]={ vOld[0], vOld[1], p1[3], p2[3] };
        addToBuffers(v,aVerts,aIdx,b,0);
      }
      break;
  }

  //Now create the first meshbuffer for the road with the previously calculated data and...
  m_pMeshBuffer[0]->append(aVerts.const_pointer(),aVerts.size(),aIdx.const_pointer(),aIdx.size());
  m_pMeshBuffer[0]->recalculateBoundingBox();
  //...reset the bounding box
  m_cBox.reset(m_pMeshBuffer[0]->getBoundingBox());

  //If we have a texture we apply it
  if (m_pTexParams[0]->getTexture()!="")
    m_pMeshBuffer[0]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[0]->getTexture().c_str()));

  //initialize the texture offset for the bottom
  m_fTex=m_pTexParams[1]->getOffset();

  b=false;
  //Calculation for the bottom of the road is easier.
  //we just take the vertices of the top, subtract
  //the offset and add it.
  for (u32 i=0; i<aVerts.size(); i+=4) {
    core::vector3df vToAdd[4];

    vToAdd[0]=aVerts[i  ].Pos; vToAdd[0].Y-=m_fOffset;
    vToAdd[1]=aVerts[i+1].Pos; vToAdd[1].Y-=m_fOffset;
    vToAdd[2]=aVerts[i+2].Pos; vToAdd[2].Y-=m_fOffset;
    vToAdd[3]=aVerts[i+3].Pos; vToAdd[3].Y-=m_fOffset;

    addToBuffers(vToAdd,aBotVerts,aBotIdx,b,1);
    b=!b;
  }

  //Fill the second meshbuffer, apply the bounding box and add the texture (if one is selected)
  m_pMeshBuffer[1]->append(aBotVerts.const_pointer(),aBotVerts.size(),aBotIdx.const_pointer(),aBotIdx.size());
  m_pMeshBuffer[1]->recalculateBoundingBox();
  m_cBox.addInternalBox(m_pMeshBuffer[1]->getBoundingBox());
  m_fTex=m_pTexParams[2]->getOffset();

  if (m_pTexParams[1]->getTexture()!="")
    m_pMeshBuffer[1]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[1]->getTexture().c_str()));

  //Now for the last part: we need to define the side polygons of the road. We do this in a loop
  //that's executed twice and just take the positions of the top and bottom vertices.
  for (u32 iBuffer=2; iBuffer<4; iBuffer++) {
    b=false;
    //Some temp variables
    core::array<video::S3DVertex> aToAdd;
    core::array<u16> aIdxToAdd;

    //Iterate over all vertices
    for (u32 i=0; i<aVerts.size(); i+=4) {
      core::vector3df vToAdd[4];

      if (iBuffer==2) {
        vToAdd[0]=aVerts   [i  ].Pos;
        vToAdd[1]=aVerts   [i+2].Pos;
        vToAdd[2]=aBotVerts[i  ].Pos;
        vToAdd[3]=aBotVerts[i+2].Pos;
      }
      else {
        vToAdd[0]=aVerts   [i+1].Pos;
        vToAdd[1]=aBotVerts[i+1].Pos;
        vToAdd[2]=aVerts   [i+3].Pos;
        vToAdd[3]=aBotVerts[i+3].Pos;
      }

      addToBuffers(vToAdd,aToAdd,aIdxToAdd,b,iBuffer);
      b=!b;
    }

    //Same procedure as with the other meshbuffers...
    m_pMeshBuffer[iBuffer]->append(aToAdd.const_pointer(),aToAdd.size(),aIdxToAdd.const_pointer(),aIdxToAdd.size());
    m_pMeshBuffer[iBuffer]->recalculateBoundingBox();
    m_cBox.addInternalBox(m_pMeshBuffer[iBuffer]->getBoundingBox());
    m_fTex=m_pTexParams[3]->getOffset();

    if (m_pTexParams[iBuffer]->getTexture()!="")
      m_pMeshBuffer[iBuffer]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[iBuffer]->getTexture().c_str()));
  }
}

/**
 * Init the Bezier helppoints. Nothing special here. We just
 * calculate the point in the middle between the chosed segment borders
 * @see CConnection::setSegment1
 * @see CConnection::setSegment2
 * @see CConnection::setSegment1Border
 * @see CConnection::setSegment2Border
 * @see CConnection::setType
 */
void CConnection::calculateHelpPoints() {
  if (m_pSegment1==NULL || m_pSegment2==NULL) return;

  core::vector3df v[4],v2[2];

  switch (m_iSegment1Border) {
    case 0: v[0]=m_pSegment1->getPoint(0); v[1]=m_pSegment1->getPoint(1); v2[0]=m_pSegment1->getPoint(3); break;
    case 1: v[0]=m_pSegment1->getPoint(0); v[1]=m_pSegment1->getPoint(3); v2[0]=m_pSegment1->getPoint(1); break;
    case 2: v[0]=m_pSegment1->getPoint(3); v[1]=m_pSegment1->getPoint(2); v2[0]=m_pSegment1->getPoint(0); break;
    case 3: v[0]=m_pSegment1->getPoint(2); v[1]=m_pSegment1->getPoint(1); v2[0]=m_pSegment1->getPoint(3); break;
    default: printf("Invalid value for \"segment 1 border\"!\n"); return;
  }

  switch (m_iSegment2Border) {
    case 0: v[2]=m_pSegment2->getPoint(0); v[3]=m_pSegment2->getPoint(1); v2[1]=m_pSegment2->getPoint(3); break;
    case 1: v[2]=m_pSegment2->getPoint(0); v[3]=m_pSegment2->getPoint(3); v2[1]=m_pSegment2->getPoint(1); break;
    case 2: v[2]=m_pSegment2->getPoint(3); v[3]=m_pSegment2->getPoint(2); v2[1]=m_pSegment2->getPoint(0); break;
    case 3: v[2]=m_pSegment2->getPoint(2); v[3]=m_pSegment2->getPoint(1); v2[1]=m_pSegment2->getPoint(3); break;
    default: printf("Invalid value for \"segment 2 border\"!\n"); return;
  }

  switch (m_eType) {
    case eBezier1: break;

    case eBezier2:
      if (m_bFlipConnection) {
        core::vector3df vDir=v2[0]-v[0],
                        vLen=v[0]-v[2];

        f32 fLen=vLen.getLength();

        vDir.normalize();

        core::line3df cLine[2]={
                        core::line3df(v[0],v[0]-fLen*vDir),
                        core::line3df(v[1],v[1]-fLen*vDir)
                      };

        vDir=v2[1]-v[2];
        vDir.normalize();

        core::vector3df vMinDist[2]={ core::vector3df(0.0f), core::vector3df(0.0f) };
        f32 fMinDist[2]={ 1000.0f*fLen, 1000.0f*fLen };

        for (u32 i=0; i<2; i++)
          for (f32 f=0.0f; f<fLen; f+=0.01f) {
            core::vector3df vDummy=v[i+2]-f*vDir;
            if (cLine[i==0?1:0].getClosestPoint(vDummy).getDistanceFrom(vDummy)<fMinDist[i]) {
              fMinDist[i]=cLine[i==0?1:0].getClosestPoint(vDummy).getDistanceFrom(vDummy);
              vMinDist[i]=vDummy;
            }
          }

        m_vHelpPoints[2]=vMinDist[0];
        m_vHelpPoints[0]=vMinDist[1];
      }
      else {
        core::vector3df vDir=v2[1]-v[2],
                        vLen=v[0]-v[2];

        f32 fLen=vLen.getLength();

        vDir.normalize();

        core::line3df cLine[2]={
                        core::line3df(v[2],v[2]-fLen*vDir),
                        core::line3df(v[3],v[3]-fLen*vDir)
                      };

        m_pDrv->draw3DLine(cLine[0].start,cLine[0].end,video::SColor(0xFF,0xFF,0,0));
        m_pDrv->draw3DLine(cLine[1].start,cLine[1].end,video::SColor(0xFF,0xFF,0,0));

        vDir=v2[0]-v[0];
        vDir.normalize();

        core::vector3df vMinDist[2]={ core::vector3df(0.0f), core::vector3df(0.0f) };
        f32 fMinDist[2]={ 1000.0f*fLen, 1000.0f*fLen };

        for (u32 i=0; i<2; i++)
          for (f32 f=0.0f; f<fLen; f+=0.01f) {
            core::vector3df vDummy=v[i]-f*vDir;
            if (cLine[i].getClosestPoint(vDummy).getDistanceFrom(vDummy)<fMinDist[i]) {
              fMinDist[i]=cLine[i].getClosestPoint(vDummy).getDistanceFrom(vDummy);
              vMinDist[i]=vDummy;
            }
          }

        m_pDrv->draw3DLine(v[0],vMinDist[0],video::SColor(0xFF,0,0xFF,0));
        m_pDrv->draw3DLine(v[1],vMinDist[1],video::SColor(0xFF,0,0xFF,0));

        m_vHelpPoints[0]=vMinDist[0];
        m_vHelpPoints[2]=vMinDist[1];

      }

      break;

    case eBezier3: {
        core::vector3df vDir=v2[0]-v[0],vLen=v[0]-v[2];
        vLen.Y=0;
        f32 fLen=vLen.getLength()/2.0f;

        vDir.normalize();

        m_vHelpPoints[0]=v[0]-fLen*vDir;
        m_vHelpPoints[2]=v[1]-fLen*vDir;

        vDir=v2[1]-v[2];
        vDir.normalize();

        if (m_bFlipConnection) {
          m_vHelpPoints[1]=v[3]-fLen*vDir;
          m_vHelpPoints[3]=v[2]-fLen*vDir;
        }
        else {
          m_vHelpPoints[1]=v[2]-fLen*vDir;
          m_vHelpPoints[3]=v[3]-fLen*vDir;
        }
      }
      break;
  }
}

/**
 * Get the next point using the Bezier1 algorithm
 * @param p array with the two points for the algorithm
 * @param fStep step number (0..1)
 * @return the next point
 * @see recalcMeshBuffer
 * @see render
 */
core::vector3df CConnection::getBezier1(core::vector3df p[], f32 fStep) {
  return (1-fStep)*p[0]+fStep*p[1];
}

/**
 * Get the next point using the Bezier2 algorithm
 * @param p array with the three points (start, help, end) for the algorithm
 * @param fStep step number (0..1)
 * @return the next point
 * @see recalcMeshBuffer
 * @see render
 */
core::vector3df CConnection::getBezier2(core::vector3df p[], f32 fStep) {
  return ((1-fStep)*(1-fStep))*p[0]+2*fStep*(1-fStep)*p[1]+fStep*fStep*p[2];
}

/**
 * Get the next point using the Bezier3 algorithm
 * @param p array with the four points (start, help1, help2, end) for the algorithm
 * @param fStep step number (0..1)
 * @return the next point
 * @see recalcMeshBuffer
 * @see render
 */
core::vector3df CConnection::getBezier3(core::vector3df p[], f32 fStep) {
  return (-p[0]+3*p[1]-3*p[2]+p[3])*fStep*fStep*fStep+(3*p[0]-6*p[1]+3*p[2])*fStep*fStep+(-3*p[0]+3*p[1])*fStep+p[0];
}

/**
 * Constructor
 * @param pDrv used Irrlicht videodriver
 */
CConnection::CConnection(video::IVideoDriver *pDrv) {
  //initialize the segments
  m_pSegment1=NULL;
  m_pSegment2=NULL;

  //initialize the segment names
  m_sSegment1="";
  m_sSegment2="";

  //initialize the border indices and the selected index
  m_iSegment1Border=0;
  m_iSegment2Border=0;
  m_iSelectedPoint=-1;

  //Initialize the flip and the selected flags
  m_bFlipConnection=false;
  m_bFlipVertices  =false;
  m_bSelected      =false;

  //Initialize the helppoints
  m_vHelpPoints[0]=core::vector3df(0.0f,0.0f,0.0f);
  m_vHelpPoints[1]=core::vector3df(0.0f,0.0f,0.0f);
  m_vHelpPoints[2]=core::vector3df(0.0f,0.0f,0.0f);
  m_vHelpPoints[3]=core::vector3df(0.0f,0.0f,0.0f);

  //Default: 10 steps, offset 3.0
  m_iSteps=10;
  m_fOffset=3.0f;

  //Default type= Bezier1
  m_eType=eBezier1;

  //Store the driver for later use
  m_pDrv=pDrv;

  //initialize the meshbuffer members and initialize the texture parameter objects
  for (u32 i=0; i<4; i++) {
    m_pMeshBuffer[i]=NULL;
    m_pTexParams[i]=new CTextureParameters();
  }

  m_fRoadWidth=0.0f;
}

/**
 * The destructor
 */
CConnection::~CConnection() {
  //Notify the segments of the deletion
  if (m_pSegment1!=NULL) m_pSegment1->delNotify(this);
  if (m_pSegment2!=NULL) m_pSegment2->delNotify(this);

  //Delete the texture parameters and the meshbuffers
  for (u32 i=0; i<4; i++) {
    if (m_pMeshBuffer[i]!=NULL) m_pMeshBuffer[i]->drop();
    delete m_pTexParams[i];
  }
}

void CConnection::setSegment1(CSegment *p) { m_iSegment1Border=0; setSegment1NoInit(p); calculateHelpPoints(); }
void CConnection::setSegment2(CSegment *p) { m_iSegment2Border=0; setSegment2NoInit(p); calculateHelpPoints(); }

void CConnection::setSegment1NoInit(CSegment *p) {
  //If are already assigned to a segment we must tell this segment we're leaving it
  if (m_pSegment1!=NULL) m_pSegment1->delNotify(this);
  m_pSegment1=p;
  //Inform the new segment of the connection
  m_pSegment1->addNotify(this);
  //recreate the object
  attributesChanged(m_pSegment1);
}

void CConnection::setSegment2NoInit(CSegment *p) {
  //If are already assigned to a segment we must tell this segment we're leaving it
  if (m_pSegment2!=NULL) m_pSegment2->delNotify(this);
  m_pSegment2=p;
  //Inform the new segment of the connection
  m_pSegment2->addNotify(this);
  //recreate the object
  attributesChanged(m_pSegment2);
}

void CConnection::setSegment1Border(u32 i) { if (i<4) m_iSegment1Border=i; calculateHelpPoints(); update(); }
void CConnection::setSegment2Border(u32 i) { if (i<4) m_iSegment2Border=i; calculateHelpPoints(); update(); }

void CConnection::setType(eConnectionType eType) { m_eType=eType; calculateHelpPoints(); }

void CConnection::setNumberOfSteps(u32 i) { m_iSteps=i; update(); }

void CConnection::setOffset(f32 f) {
  m_fOffset=f;
  update();
}

f32 CConnection::getOffset() { return m_fOffset; }

void CConnection::setHelpPoint(u32 i, core::vector3df v) {
  if (i<4) {
    m_vHelpPoints[i]=v;
    update();
  }
}

void CConnection::setFlipConnection(bool b) { m_bFlipConnection=b; update(); }
void CConnection::setFlipVertices  (bool b) { m_bFlipVertices  =b; update(); }
void CConnection::setSelected      (bool b) { m_bSelected      =b; }

CSegment *CConnection::getSegment1() { return m_pSegment1; }
CSegment *CConnection::getSegment2() { return m_pSegment2; }

u32 CConnection::getSegment1Border() { return m_iSegment1Border; }
u32 CConnection::getSegment2Border() { return m_iSegment2Border; }

CConnection::eConnectionType CConnection::getConnectionType() { return m_eType; }

u32 CConnection::getNumberOfSteps() { return m_iSteps; }

core::vector3df CConnection::getHelpPoint(u32 i) {
  static core::vector3df vErr=core::vector3df(0.0f,0.0f,0.0f);
  return i<4?m_vHelpPoints[i]:vErr;
}

bool CConnection::getFlipConnection() { return m_bFlipConnection; }
bool CConnection::getFlipVertices  () { return m_bFlipVertices  ; }


void CConnection::render() {
  //Render the meshbuffers
  for (u32 i=0; i<4; i++)
    if (m_pMeshBuffer[i]!=NULL) {
      video::SMaterial cMat;
      cMat.Lighting=false;

      //draw the normals
      for (u32 j=0; j<m_pMeshBuffer[i]->getVertexCount(); j++) {
        core::vector3df pos=m_pMeshBuffer[i]->getPosition(j),
                        normal=m_pMeshBuffer[i]->getNormal(j);

        m_pDrv->setMaterial(cMat);
        m_pDrv->setTransform(video::ETS_WORLD,core::CMatrix4<f32>());
        m_pDrv->draw3DLine(pos,pos+normal,video::SColor(0xFF,0xFF,0xFF,0xFF));
      }

      //draw the meshbuffers
      cMat.setTexture(0,m_pMeshBuffer[i]->getMaterial().getTexture(0));

      m_pDrv->setMaterial(cMat);
      m_pDrv->drawMeshBuffer(m_pMeshBuffer[i]);
    }

  //if we're not the currently selected connection we don't render anything else
  if (!m_bSelected) return;

  //draw the bounding box to indicate we're selected
  m_pDrv->draw3DBox(m_cBox,video::SColor(0xFF,0x60,0x60,0xFF));

  if (m_pSegment1==NULL || m_pSegment2==NULL) return;

  //If this is the selected connection we should also display the helppoints
  //and highlight the selected one
  switch (m_eType) {
    case eBezier1: break;

    case eBezier2: {
        core::vector3df size=core::vector3df(0.5f,0.5f,0.5f);
        s32 iSelect[]={ 1, 3 },iIdx[]={ 0, 2 };

        for (u32 i=0; i<2; i++) {
          core::aabbox3df box=core::aabbox3df(m_vHelpPoints[iIdx[i]]-size,m_vHelpPoints[iIdx[i]]+size);
          m_pDrv->draw3DBox(box,m_iSelectedPoint==iSelect[i]?video::SColor(0xFF,0xFF,0xFF,0):video::SColor(0xFF,0,0,0xFF));
        }
      }
      break;

    case eBezier3: {
        core::vector3df size=core::vector3df(0.5f,0.5f,0.5f);
        s32 iSelect[]={ 1, 2, 3, 0 };

        for (u32 i=0; i<4; i++) {
          core::aabbox3df box=core::aabbox3df(m_vHelpPoints[i]-size,m_vHelpPoints[i]+size);
          m_pDrv->draw3DBox(box,m_iSelectedPoint==iSelect[i]?video::SColor(0xFF,0xFF,0xFF,0):video::SColor(0xFF,0,0,0xFF));
        }
      }
      break;
  }
}

void CConnection::setSelectedPoint(s32 iPoint) {
  m_iSelectedPoint=iPoint;
}

/**
 * Save the segment
 * @param out the attribute object to fill
 */
void CConnection::save(io::IAttributes *out) {
  out->addString("Segment1",m_pSegment1->getName().c_str());
  out->addString("Segment2",m_pSegment2->getName().c_str());

  out->addInt("Type",m_eType);
  out->addInt("S1Border",m_iSegment1Border);
  out->addInt("S2Border",m_iSegment2Border);
  out->addInt("Steps",m_iSteps);

  out->addBool("FlipC",m_bFlipConnection);
  out->addBool("FlipV",m_bFlipVertices  );

  for (u32 i=0; i<4; i++) {
    core::stringc s="HelpPoint"; s+=i;
    out->addVector3d(s.c_str(),m_vHelpPoints[i]);
  }
}

/**
 * Load the segment
 * @param in the attribute object with the necessary data
 */
void CConnection::load(io::IAttributes *in) {
  m_sSegment1=in->getAttributeAsString("Segment1");
  m_sSegment2=in->getAttributeAsString("Segment2");

  m_eType          =(CConnection::eConnectionType)in->getAttributeAsInt("Type");
  m_iSegment1Border=in->getAttributeAsInt("S1Border");
  m_iSegment2Border=in->getAttributeAsInt("S2Border");
  m_iSteps         =in->getAttributeAsInt("Steps");

  m_bFlipConnection=in->getAttributeAsBool("FlipC");
  m_bFlipVertices  =in->getAttributeAsBool("FlipV");

  for (u32 i=0; i<4; i++) {
    core::stringc s="HelpPoint"; s+=i;
    m_vHelpPoints[i]=in->getAttributeAsVector3d(s.c_str());
  }
}

const core::stringc &CConnection::getSegment1Name() { return m_sSegment1; }
const core::stringc &CConnection::getSegment2Name() { return m_sSegment2; }

void CConnection::attributesChanged(CSegment *p) {
  update();
}

void CConnection::objectDeleted(CSegment *p) {
  //If one of the segments was deleted we update the member
  if (m_pSegment1==p) m_pSegment1=NULL;
  if (m_pSegment2==p) m_pSegment2=NULL;

  //and recalculate everything
  update();
}

void CConnection::update() {
  //If something has changed we recalculate the meshbuffers
  recalcMeshBuffer();
}

CTextureParameters *CConnection::getTextureParameters(u32 i) {
  return i<4?m_pTexParams[i]:NULL;
}

scene::IMeshBuffer *CConnection::getMeshBuffer(u32 i) {
  return i<4?m_pMeshBuffer[i]:NULL;
}
