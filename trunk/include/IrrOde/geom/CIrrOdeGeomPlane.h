#ifndef _IRR_ODE_GEOM_PLANE
  #define _IRR_ODE_GEOM_PLANE

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_PLANE_ID=MAKE_IRR_ID('i','o','g','p');
const wchar_t IRR_ODE_GEOM_PLANE_NAME[0xFF]=L"CIrrOdeGeomPlane";

class CIrrOdeGeomPlane : public CIrrOdeGeom {
  protected:
    f32 m_fA,m_fB,m_fC,m_fD;

  public:
    CIrrOdeGeomPlane(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                     const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                 const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                 const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    void setParams(f32 fA, f32 fB, f32 fC, f32 fD);
    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual s32 getID() const;
    virtual irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();

    virtual void setMassTotal(f32 fMass);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);
};

} //namespace ode
} //namespace irr

#endif
