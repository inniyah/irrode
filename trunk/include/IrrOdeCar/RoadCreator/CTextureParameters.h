#ifndef _C_TEXTURE_PARAMETERS
  #define _C_TEXTURE_PARAMETERS
  
  #include <irrlicht.h>
  
using namespace irr;

/**
 * @class CTextureParameters
 * @author Christian Keimel / bulletbyte.de
 * This class is used to store all necessary texture parameters
 */
class CTextureParameters {
  protected:
    core::stringc m_sTexture;   /**<! the texture */
    u32 m_iRotate;              /**<! the texture rotation (0=0 degrees, 1=90 degrees, 2=180 degrees, 3=270 degrees) */
    f32 m_fOffset,              /**<! offset of the texture */
        m_fScale;               /**<! scale of the texture */
    bool m_bStretch;            /**<! flag to stretch the texture to fit */
    
  public:
    /**
     *  The constructor
     */
    CTextureParameters() {
      m_sTexture="";
      m_iRotate=0;
      m_fOffset=0.0f;
      m_fScale=1.0f;
      m_bStretch=false;
    }
    
    /**
     * Set the texture file
     * @param sTexture the texture
     */
    void setTexture(const core::stringc &sTexture) {
      m_sTexture=sTexture;
    }
    
    /**
     * Set the texture rotation
     * @param i the texture rotation (0=0 degrees, 1=90 degrees, 2=180 degrees, 3=270 degrees)
     */
    void setRotate(u32 i) {
      m_iRotate=i;
    }
    
    /**
     * Set the offset of the texture
     * @param f the offset of the texture
     */
    void setOffset(f32 f) {
      m_fOffset=f;
    }
    
    /**
     * Set the scale of the texture
     * @param f the new scale of the texture
     */
    void setScale(f32 f) {
      m_fScale=f;
    }
    
    /**
     * Set the stretch flag of the texture
     * @param b the new value for the stretch flag
     */
    void setStretch(bool b) {
      m_bStretch=b;
    }
    
    /**
     * Get the file path of the texture
     * @return the file path of the texture
     */
    const core::stringc &getTexture() {
      return m_sTexture;
    }
    
    /**
     * Get the rotation of the texture
     * @return the rotation of the texture (0=0 degrees, 1=90 degrees, 2=180 degrees, 3=270 degrees)
     */
    u32 getRotate() {
      return m_iRotate;
    }
    
    /**
     * Get the offset of the texture
     * @return the offset of the texture
     */
    f32 getOffset() {
      return m_fOffset;
    }
    
    /**
     * Get the scale of the texture
     * @return the scale of the texture
     */
    f32 getScale() {
      return m_fScale;
    }
    
    /**
     * Get the stretch flag of the texture
     * @return the stretch flag of the texture
     */
    bool getStretch() {
      return m_bStretch;
    }
    
    /**
     * Save the texture parameters to an attribute object
     * @param out the attribute object
     */
    void save(io::IAttributes *out) {
      out->addString("Texture",m_sTexture.c_str());
      
      out->addInt("Rotate",m_iRotate);
      
      out->addFloat("Offset",m_fOffset);
      out->addFloat("Scale" ,m_fScale );
      
      out->addBool("Stretch",m_bStretch);
    }
    
    /**
     * Load the texture parameters from an attribute object
     * @param out the attribute object
     */
    void load(io::IAttributes *in) {
      m_sTexture=in->getAttributeAsString("Texture");
      
      m_iRotate=in->getAttributeAsInt("Rotate");
      
      m_fOffset=in->getAttributeAsFloat("Offset");
      m_fScale =in->getAttributeAsFloat("Scale" );
      
      m_bStretch=in->getAttributeAsBool("Stretch");
    }
};

#endif
