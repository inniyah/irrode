  #include <motors/CIrrOdeAeroDrag.h>

namespace irr {
  namespace ode {

CIrrOdeAeroDrag::CIrrOdeAeroDrag(ISceneNode *parent,ISceneManager *mgr,s32 id,
                                 const vector3df &position, const vector3df &rotation, const vector3df &scale)
                                 :IIrrOdeStepMotor(parent,mgr,id,position,rotation,scale) {

  m_vForeward=vector3df(1.0f,0.0f,0.0f);
  m_vSideward=vector3df(0.0f,0.0f,1.0f);
  m_vUpward  =vector3df(0.0f,1.0f,0.0f);

  m_fStallSpeed  =0.0f;
  m_fMaxUpSpeed  =0.0f;
  m_fForewardDamp=0.0f;
  m_fSidewardDamp=0.0f;
  m_fUpwardDamp  =0.0f;
  m_fForewardVel =0.0f;
  m_fUpFactor    =1.0f;

  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh) {
      c8 sFileName[1024];
      sprintf(sFileName,"%sIrrOdeAeroDrag.png",m_sResources);
      m_cMat.setTexture(0,m_pSceneManager->getVideoDriver()->getTexture(sFileName));
    }
  #endif
}

void CIrrOdeAeroDrag::step() {
  if (m_pBody && m_bIsActive) {
    vector3df vRot=m_pBody->getRotation(),
              vVel=m_pBody->getLinearVelocity(),
              vNormVel=vVel,
              vUp=vRot.rotationToDirection(m_vUpward);

    vNormVel.normalize();

    vector3df vForeward=vRot.rotationToDirection(m_vForeward);
    f32 fForeward=vForeward.dotProduct(vNormVel.normalize());

    m_fForewardVel=fForeward*vVel.getLength();

    f32 fUp=fForeward;
    if (m_fForewardVel>=m_fStallSpeed)
      if (m_fForewardVel>=m_fMaxUpSpeed)
        vUp*=m_fUpFactor*m_fMaxUpSpeed;
      else
        vUp*=m_fUpFactor*(m_fForewardVel-m_fStallSpeed);
    else
      vUp=vector3df(0.0f,0.0f,0.0f);

    vUp=m_fPower*m_fMaxPower*fUp*vUp;

    f32 fForewardDamp=m_fForewardVel*m_fForewardVel*m_fForewardDamp;
    vector3df vForewardDamp=vForeward*fForewardDamp;

    vector3df vSideward=vRot.rotationToDirection(m_vSideward);
    f32 fSideward=vSideward.dotProduct(vNormVel.normalize());

    vector3df vSideDamp=vector3df(0.0f,0.0f,0.0f);

    if (fSideward>=0.01f || fSideward<=-0.01f) vSideDamp=vSideward*m_fForewardVel*fSideward*m_fSidewardDamp;

    vector3df vUpward=vRot.rotationToDirection(m_vUpward);
    f32 fUpward=vUpward.dotProduct(vNormVel.normalize());

    vector3df vUpDamp=vector3df(0.0f,0.0f,0.0f);

    if (fUpward>=0.1f || fUpward<=-0.1f) vUpDamp=vUpward*m_fForewardVel*fUpward*m_fUpwardDamp;

    vector3df vForce=vUp-vForewardDamp-vSideDamp-vUpDamp;

    m_pBody->addForce(vForce);
  }
}

const wchar_t *CIrrOdeAeroDrag::getTypeName() {
  return IRR_ODE_AERO_DRAG_NAME;
}

void CIrrOdeAeroDrag::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const {
  IIrrOdeStepMotor::serializeAttributes(out,options);

  out->addVector3d("foreward",m_vForeward);
  out->addVector3d("sideward",m_vSideward);
  out->addVector3d("upward"  ,m_vUpward  );

  out->addFloat("stall_speed"  ,m_fStallSpeed  );
  out->addFloat("max_up_speed" ,m_fMaxUpSpeed  );
  out->addFloat("up_factor"    ,m_fUpFactor    );
  out->addFloat("foreward_damp",m_fForewardDamp);
  out->addFloat("sideward_damp",m_fSidewardDamp);
  out->addFloat("upward_damp"  ,m_fUpwardDamp  );
}

void CIrrOdeAeroDrag::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) {
  IIrrOdeStepMotor::deserializeAttributes(in,options);
  m_vForeward=in->getAttributeAsVector3d("foreward");
  m_vSideward=in->getAttributeAsVector3d("sideward");
  m_vUpward  =in->getAttributeAsVector3d("upward"  );

  m_fStallSpeed  =in->getAttributeAsFloat("stall_speed"  );
  m_fMaxUpSpeed  =in->getAttributeAsFloat("max_up_speed" );
  m_fForewardDamp=in->getAttributeAsFloat("foreward_damp");
  m_fSidewardDamp=in->getAttributeAsFloat("sideward_damp");
  m_fUpwardDamp  =in->getAttributeAsFloat("upward_damp"  );
  m_fUpFactor    =in->getAttributeAsFloat("up_factor"    );
}

  } //namespace ode
}   //namespace irr

