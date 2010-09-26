  #include <motors/CIrrOdeTorqueMotor.h>

namespace irr {
namespace ode {

CIrrOdeTorqueMotor::CIrrOdeTorqueMotor(ISceneNode *parent,ISceneManager *mgr,s32 id,
                    const vector3df &position, const vector3df &rotation, const vector3df &scale)
                    :IIrrOdeStepMotor(parent,mgr,id,position,rotation,scale) {

  m_vRollAxis =vector3df(1.0f,0.0f,0.0f);
  m_vPitchAxis=vector3df(0.0f,0.0f,1.0f);
  m_vYawAxis  =vector3df(0.0f,1.0f,0.0f);

  m_bAeroRudder=false;

  m_fMinVel=0.0f;
  m_fMaxVel=0.0f;
  m_fPitch =0.0f;
  m_fRoll  =0.0f;
  m_fYaw   =0.0f;

  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh) {
      c8 sFileName[1024];
      sprintf(sFileName,"%sIrrOdeTorqueMotor.png",m_sResources);
      m_cMat.setTexture(0,m_pSceneManager->getVideoDriver()->getTexture(sFileName));
    }
  #endif
}

void CIrrOdeTorqueMotor::step() {
  if (m_pBody!=NULL && m_pBody->physicsInitialized() && m_bIsActive) {
    vector3df vRot=m_pBody->getRotation(),
              vVel=m_pBody->getLinearVelocity(),
              vNormVel=vVel;

    vNormVel.normalize();
    vector3df vForeward=vRot.rotationToDirection(m_vRollAxis);
    f32 fForeward=vForeward.dotProduct(vNormVel.normalize());

    m_fForewardVel=fForeward*vVel.getLength();

    if (m_bAeroRudder) {
      vector3df vTorque=vector3df(0.0f,0.0f,0.0f);

      if (m_fForewardVel>=m_fMinVel || m_fForewardVel<=-m_fMinVel) {
        vector3df vRudder=m_vPitchAxis*m_fPitch+m_vYawAxis*m_fYaw+m_vRollAxis*m_fRoll;
        vTorque=vRot.rotationToDirection(vRudder);
        if (m_fForewardVel>=m_fMaxVel || m_fForewardVel<=-m_fMaxVel)
          vTorque*=(m_fMaxVel-m_fMinVel)*m_fMaxPower;
        else
          vTorque*=(m_fForewardVel-m_fMinVel)*m_fMaxPower;
      }


      m_pBody->addTorque(m_fPower*vTorque);
    }
    else {
      vector3df vRudder=m_vPitchAxis*m_fPitch+m_vYawAxis*m_fYaw+m_vRollAxis*m_fRoll,
                vTorque=vRot.rotationToDirection(vRudder)*m_fMaxPower;

      m_pBody->addTorque(m_fPower*vTorque);
    }
  }
}

const wchar_t *CIrrOdeTorqueMotor::getTypeName() {
  return IRR_ODE_TORQUE_MOTOR_NAME;
}

void CIrrOdeTorqueMotor::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const {
  IIrrOdeStepMotor::serializeAttributes(out,options);

  out->addVector3d("roll_axis" ,m_vRollAxis );
  out->addVector3d("pitch_axis",m_vPitchAxis);
  out->addVector3d("yaw_axis"  ,m_vYawAxis  );

  out->addBool("aero_rudder",m_bAeroRudder);

  out->addFloat("min_vel",m_fMinVel);
  out->addFloat("max_vel",m_fMaxVel);
}

void CIrrOdeTorqueMotor::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) {
  IIrrOdeStepMotor::deserializeAttributes(in,options);

  m_vRollAxis =in->getAttributeAsVector3d("roll_axis" );
  m_vPitchAxis=in->getAttributeAsVector3d("pitch_axis");
  m_vYawAxis  =in->getAttributeAsVector3d("yaw_axis"  );

  m_bAeroRudder=in->getAttributeAsBool("aero_rudder");

  m_fMinVel=in->getAttributeAsFloat("min_vel");
  m_fMaxVel=in->getAttributeAsFloat("max_vel");
}

ISceneNode *CIrrOdeTorqueMotor::clone(ISceneNode* newParent, ISceneManager* newManager) {
  CIrrOdeTorqueMotor *pRet=new CIrrOdeTorqueMotor(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  pRet->setBody(reinterpret_cast<irr::ode::CIrrOdeBody *>(newParent));
  pRet->setRollAxis(m_vRollAxis);
  pRet->setYawAxis(m_vYawAxis);
  pRet->setPitchAxis(m_vPitchAxis);

  pRet->setRoll(m_fRoll);
  pRet->setYaw(m_fYaw);
  pRet->setPitch(m_fPitch);

  pRet->setAeroRudder(m_bAeroRudder);
  pRet->setMinVel(m_fMinVel);
  pRet->setMaxVel(m_fMaxVel);

  pRet->setPower(m_fPower);
  pRet->setMaxPower(m_fMaxPower);
  pRet->setIsActive(m_bIsActive);
  return pRet;
}

}
}
