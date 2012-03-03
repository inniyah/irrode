#ifndef _IRR_ODE_SPACE
  #define _IRR_ODE_SPACE

  #include <CIrrOdeSceneNode.h>

namespace irr {
namespace ode {

class CIrrOdeWorld;

const int IRR_ODE_SPACE_ID=MAKE_IRR_ID('i','o','s','p');
const wchar_t IRR_ODE_SPACE_NAME[0xFF]=L"CIrrOdeSpace";

enum _IRR_ODE_SPACE_TYPE {
  eIrrOdeSpaceSimple,
  eIrrOdeSpaceHash,
  eIrrOdeSpaceQuadTree
};

class CIrrOdeSpace : public CIrrOdeSceneNode {
  protected:
    s32 m_iSpaceId;
    _IRR_ODE_SPACE_TYPE m_iType;
    CIrrOdeSpace *m_pParentSpace;
    CIrrOdeWorld *m_pWorld;
    irr::core::vector3df m_cCenter,m_cExtents;
    s32 m_iDepth;

  public:
    CIrrOdeSpace(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                 const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
	  	           const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));
    CIrrOdeSpace(CIrrOdeSceneNode *pSource, irr::scene::ISceneNode *pNewParent, irr::scene::ISceneManager *pNewManager);
    ~CIrrOdeSpace();

    virtual void initPhysics();

    void setSpaceType(_IRR_ODE_SPACE_TYPE iType);
    _IRR_ODE_SPACE_TYPE getSpaceType();

    void setQuadTreeParams(irr::core::vector3df cCenter, irr::core::vector3df cExtents, s32 iDepth);

    u32 getSpaceId();

    s32 getID();

    irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();
    virtual void render();
    void OnRegisterSceneNode();
    void setType(_IRR_ODE_SPACE_TYPE iType);
    _IRR_ODE_SPACE_TYPE getType();

    CIrrOdeSpace *getParentSpace();
    irr::core::vector3df &getCenter();
    irr::core::vector3df &getExtents();
    s32 getDepth();

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);
    virtual void removeFromPhysics();

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent) { return false; }
};

} //namespace ode
} //namespace irr

#endif
