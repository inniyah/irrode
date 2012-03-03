#ifndef _C_IRR_ODE_CYLINDER
  #define _C_IRR_ODE_CYLINDER

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_CYLINDER_ID=MAKE_IRR_ID('i','o','g','c');
const wchar_t IRR_ODE_GEOM_CYLINDER_NAME[0xFF]=L"CIrrOdeGeomCylinder";

class CIrrOdeGeomCylinder : public CIrrOdeGeom {
  protected:
    f32 m_fRadius,m_fLength;

  public:
    CIrrOdeGeomCylinder(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                   const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		               const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		               const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeGeomCylinder();

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual s32 getID() const;
    virtual const wchar_t *getTypeName();

    virtual void setMassTotal(f32 fMass);

    void setRadius(f32 fRadius);
    void setLength(f32 fLength);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);
};

} //namespace ode
} //namespace irr

#endif
