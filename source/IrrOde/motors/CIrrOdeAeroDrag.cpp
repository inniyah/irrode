  #include <motors/CIrrOdeAeroDrag.h>

namespace irr {
  namespace ode {

CIrrOdeAeroDrag::CIrrOdeAeroDrag(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                 const irr::core::vector3df &position, const irr::core::vector3df &rotation, const irr::core::vector3df &scale)
                                 :IIrrOdeStepMotor(parent,mgr,id,position,rotation,scale) {

  m_vForeward=irr::core::vector3df(1.0f,0.0f,0.0f);
  m_vSideward=irr::core::vector3df(0.0f,0.0f,1.0f);
  m_vUpward  =irr::core::vector3df(0.0f,1.0f,0.0f);

  m_fStallSpeed      =0.0f;
  m_fMaxUpSpeed      =0.0f;
  m_fForewardDamp    =0.0f;
  m_fSidewardDamp    =0.0f;
  m_fUpwardDamp      =0.0f;
  m_fForewardVel     =0.0f;
  m_fUpFactor        =1.0f;
  m_fPitchTrim       =0.1f;
  m_fPitchTrimPwr    =0.15f;
  m_fUpToForeward    =0.0f;
  m_fUpDampToForeward=0.0f;
  m_fSideToForeward  =0.0f;
  m_fDampMaxVel      =0.0f;
  m_fDampMinVel      =0.0f;
}

void CIrrOdeAeroDrag::step() {
  if (m_pBody && m_pBody->physicsInitialized() && m_bIsActive) {
    irr::core::vector3df vRot=m_pBody->getRotation(),
              vVel=m_pBody->getLinearVelocity(),
              vNormVel=vVel,
              vUp=vRot.rotationToDirection(m_vUpward),
              vPushForeward=irr::core::vector3df(0.0f,0.0f,0.0f);

    vNormVel.normalize();

    irr::core::vector3df vForeward=vRot.rotationToDirection(m_vForeward);
    f32 fForeward=vForeward.dotProduct(vNormVel.normalize()),
        fSideUpDampFactor=1.0f;

    if (m_fPitchTrimPwr!=0.0f && m_fPitchTrim!=0.0f) {
      f32 v=vUp.dotProduct(vNormVel);
      v-=m_fPitchTrim;
      if (v>=0.001f || v<=-0.001f) {
        v*=vVel.getLength();
        if (v>= 10.0f) v= 10.0f;
        if (v<=-10.0f) v=-10.0f;
        v*=m_fPitchTrimPwr;
        m_pBody->addForceAtPosition(m_pBody->getPosition()-vForeward,-vUp*v);
      }
    }

    m_fForewardVel=fForeward*vVel.getLength();

    if (m_fForewardVel<0.0f) return;

    if (m_fDampMaxVel>m_fDampMinVel) {
      if (m_fForewardVel<m_fDampMinVel)
        fSideUpDampFactor=0.0f;
      else
        if (m_fForewardVel<m_fDampMaxVel)
          fSideUpDampFactor=(m_fForewardVel-m_fDampMinVel)/(m_fDampMaxVel-m_fDampMinVel);
    }

    //OK, if this aero drag provides a lifting force...
    if (m_fUpFactor!=0.0f) {
      //...it is created by the foreward velocity (e.g. as with the wings of a plane)...
      f32 fUp=fForeward;
      //...but only if it's higher than the stall speed...
      if (m_fForewardVel>=m_fStallSpeed)
        //with a limit of max up speed (in this case ... not reairr::core::listic, but OK)...
        if (m_fForewardVel>=m_fMaxUpSpeed)
          vUp*=m_fUpFactor*m_fMaxUpSpeed;
        else
          vUp*=m_fUpFactor*(m_fForewardVel-m_fStallSpeed);
      else
        //...but if the stall speed isn't exceeded we don't create a lifting force.
        vUp=irr::core::vector3df(0.0f,0.0f,0.0f);

      //now calculate the upwards force vector...
      vUp=m_fPower*m_fMaxPower*fUp*vUp;

      vPushForeward=vForeward*(m_fUpToForeward*vUp.getLength());
    }
    else vUp=irr::core::vector3df(0.0f,0.0f,0.0f);  //...or set it to a "null" vector if no up force is done

    //the calculation of the lifting force is one point of this method, now we calculate forces that damp velocities
    //into any direction. First comes the foreward damping. This is the simplest form: we just multiply the
    //square of the foreward part of the velocity vector with the given value and scale a vector with the result.
    irr::core::vector3df vForewardDamp=irr::core::vector3df(0.0f,0.0f,0.0f);

    if (m_fForewardDamp!=0.0f) {
      f32 fForewardDamp=m_fForewardVel*m_fForewardVel*m_fForewardDamp;
      vForewardDamp=vForeward*fForewardDamp;
    }

    //Sidewards and upwards damping are a bit different. They both add a static limit at which the effect begins.
    //They use the part of the vector that points into their direction (cross product) and calculate the
    //damping vectors using these values.
    irr::core::vector3df vSideDamp=irr::core::vector3df(0.0f,0.0f,0.0f);

    if (m_fSidewardDamp!=0.0f) {
      irr::core::vector3df vSideward=vRot.rotationToDirection(m_vSideward);
      f32 fSideward=vSideward.dotProduct(vNormVel.normalize());

      if (fSideward>=0.01f || fSideward<=-0.01f) {
        vSideDamp=fSideUpDampFactor*vSideward*m_fForewardVel*fSideward*m_fSidewardDamp;
        if (m_fForewardVel>2.0f || m_fForewardVel<-2.0f)
          vPushForeward+=vForeward+(m_fSideToForeward*vSideDamp.getLength());
      }
    }

    irr::core::vector3df vUpDamp=irr::core::vector3df(0.0f,0.0f,0.0f);

    if (m_fUpwardDamp!=0.0f) {
      irr::core::vector3df vUpward=vRot.rotationToDirection(m_vUpward);
      f32 fUpward=vUpward.dotProduct(vNormVel.normalize());

      if (fUpward>=0.1f || fUpward<=-0.1f) {
        vUpDamp=fSideUpDampFactor*vUpward*m_fForewardVel*fUpward*m_fUpwardDamp;
        if (m_fForewardVel>2.0f || m_fForewardVel<-2.0f)
          vPushForeward+=vForeward*(m_fUpDampToForeward*vUpDamp.getLength());
      }
    }

    //ok ... in the previous section we have calculated four vectors:
    //vUp: the lifting force
    //vForewardDamp: the damping of the foreward velocity
    //fSideDamp: the damping of the sideward slide
    //fUpdamp: the damping of the upward slidet
    //
    //now we combine these vectors to a single one that is then...
    irr::core::vector3df vForce=vUp+vPushForeward-vForewardDamp-vSideDamp-vUpDamp;

    //...applied to the body
    m_pBody->addForce(vForce);
  }
}

const wchar_t *CIrrOdeAeroDrag::getTypeName() {
  return IRR_ODE_AERO_DRAG_NAME;
}

void CIrrOdeAeroDrag::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
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

  out->addFloat("pitch_trim",m_fPitchTrim);
  if (m_fPitchTrim!=0.0f) out->addFloat("pitch_trim_pwr",m_fPitchTrimPwr);

  out->addFloat("up_to_foreward"     ,m_fUpToForeward    );
  out->addFloat("up_damp_to_foreward",m_fUpDampToForeward);
  out->addFloat("side_to_foreward"   ,m_fSideToForeward  );
  out->addFloat("min_damp_velocity"  ,m_fDampMinVel      );
  out->addFloat("max_damp_velocity"  ,m_fDampMaxVel      );
}

void CIrrOdeAeroDrag::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
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

  m_fPitchTrim=in->getAttributeAsFloat("pitch_trim");
  if (m_fPitchTrim!=0.0f) m_fPitchTrimPwr=in->getAttributeAsFloat("pitch_trim_pwr");

  m_fUpToForeward    =in->getAttributeAsFloat("up_to_foreward"  );
  m_fUpDampToForeward=in->getAttributeAsFloat("up_damp_to_foreward");
  m_fSideToForeward  =in->getAttributeAsFloat("side_to_foreward");
  m_fDampMinVel      =in->getAttributeAsFloat("min_damp_velocity");
  m_fDampMaxVel      =in->getAttributeAsFloat("max_damp_velocity");
}

irr::scene::ISceneNode *CIrrOdeAeroDrag::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeAeroDrag *pRet=new CIrrOdeAeroDrag(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  pRet->setBody(reinterpret_cast<irr::ode::CIrrOdeBody *>(newParent));
  pRet->setName(getName());

  pRet->setForeward(m_vForeward);
  pRet->setSideward(m_vSideward);
  pRet->setUpward(m_vUpward);

  pRet->setStallSpeed(m_fStallSpeed);
  pRet->setMaxUpSpeed(m_fMaxUpSpeed);
  pRet->setUpFactor(m_fUpFactor);
  pRet->setForewardDamp(m_fForewardDamp);
  pRet->setUpwardDamp(m_fUpwardDamp);
  pRet->setSidewardDamp(m_fSidewardDamp);

  pRet->setPower(m_fPower);
  pRet->setMaxPower(m_fMaxPower);
  pRet->setIsActive(m_bIsActive);

  pRet->setPitchTrim(m_fPitchTrim);
  pRet->setPitchTrimPower(m_fPitchTrimPwr);

  pRet->setUpToForeward(m_fUpToForeward);
  pRet->setUpDampToForeward(m_fUpDampToForeward);
  pRet->setSideToForeward(m_fSideToForeward);

  CIrrOdeSceneNode::cloneChildren(pRet,newManager);

  return pRet;
}

  } //namespace ode
}   //namespace irr

