#ifndef _C_IRR_ODE_TRIMESH
  #define _C_IRR_ODE_TRIMESH

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_TRIMESH_ID=MAKE_IRR_ID('i','o','g','t');
const wchar_t IRR_ODE_GEOM_TRIMESH_NAME[0xFF]=L"CIrrOdeGeomTrimesh";

class CIrrOdeGeomTrimesh : public CIrrOdeGeom {
  protected:
    irr::core::array<u32> m_aDataIds;
    irr::core::array<u32> m_aGeomIds;

  public:
    CIrrOdeGeomTrimesh(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                       const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                   const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                   const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeGeomTrimesh();

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual s32 getID() const;
    virtual irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();

    /**
     * Get the geom's ODE dGeomID
     * @param iNum the index of the geom id to return
     * @return the geom's ODE dGeomID
     */
    virtual u32 getGeomId(u32 iNum=0);

    s32 getIndexOfGeom(u32 iGeom);

    virtual void setMassTotal(f32 fMass);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual void setParent(irr::scene::ISceneNode *pParent);
    virtual CIrrOdeSurfaceParameters *getSurfaceParameters(u32 iIdx);
    virtual void removeFromPhysics();
};

} //namespace ode
} //namespace irr

#endif
