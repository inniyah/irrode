#ifndef _I_ROAD_PART
  #define _I_ROAD_PART
  
  #include <irrlicht.h>

using namespace irr;

class CTextureParameters;

class IRoadPart {
  public:
    virtual void recalcMeshBuffer()=0;
    
    virtual u32 getTextureCount()=0;
    virtual CTextureParameters *getTextureParameters(u32 i)=0;
    virtual scene::IMeshBuffer *getMeshBuffer(u32 i)=0;
    virtual void render()=0;
    virtual void save(io::IAttributes *out)=0;
    virtual void load(io::IAttributes *in )=0;
};

#endif
