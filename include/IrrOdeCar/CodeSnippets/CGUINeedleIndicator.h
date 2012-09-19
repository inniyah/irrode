#ifndef _C_GUI_NEEDLE_INDICATOR
  #define _C_GUI_NEEDLE_INDICATOR

namespace irr {
  namespace gui {
    /**
     * @class CGUINeedleIndicator
     * A GUI element for a needle indicator
     */
    class CGUINeedleIndicator : public irr::gui::IGUIElement {
      public:
        /**
         * @class CNeedle
         * The needle for the needle indicator
         */
        class CNeedle {
          protected:
            video::IVideoDriver *m_pDrv;  /**<! video driver to draw needle */
            video::SColor m_cColor;       /**<! color of the needle */
            core::dimension2df m_vDim;    /**<! dimension of the needle indicator */
            core::position2di m_vCenter;  /**<! center of the needle indicator */
            f32 m_fLength,                /**<! length factor of the needle */
                m_fWidth,                 /**<! width factor of the needle */
                m_fFactor,                /**<! factor for the needle's angle */
                m_fValue;                 /**<! angle of the needle */

          public:
            /**
             * The constructor
             * @param pDrv the video driver
             * @param cColor color of the needle
             * @param fLength length of the needle
             * @param fWidth width of the needle's base
             * @param fFactor factor of the needle's angle
             * @param vCenter center of the needle indicator
             * @param vDim size of the needle indicator
             */
            CNeedle(video::IVideoDriver *pDrv, video::SColor cColor, f32 fLength, f32 fWidth, f32 fFactor, core::position2di vCenter, core::dimension2di vDim);

            /**
             * Set the value (angle) of the needle
             * @param fValue the new value (angle)
             */
            void setValue(f32 fValue);

            /**
             * Draw the needle
             */
            void draw();
        };

        /**
         * @enum eOverflowMode
         * How to handle overflow values
         */
        enum eOverflowMode {
          eOverflowCut,     /**<! limit the value to the upper/lower limit */
          eOverflowFit,     /**<! normalize the value so that it fits into the range */
          eOverflowIgnore   /**<! ignore the limt */
        };

      protected:
        irr::gui::IGUIEnvironment *m_pGuiEnv;    /**<! the GUI environment */
        video::ITexture *m_pTexture;        /**<! the background texture */
        video::IVideoDriver *m_pDrv;        /**<! the video driver */
        core::list<CNeedle *> m_lNeedles;   /**<! list of the needles */
        core::array<CNeedle *> m_aNeedles;  /**<! array of the needles */
        f32 m_fMin,               /**<! the minimum value */
            m_fMax,               /**<! the maximum value */
            m_fSum,               /**<! summary of minimum and maximum value (range) */
            m_fValue,             /**<! the current value */
            m_fMinAngle,          /**<! the minimum angle */
            m_fMaxAngle,          /**<! the maximum angle */
            m_fAngleOffset;       /**<! the angle offset */
        eOverflowMode m_eMode;    /**<! overflow mode */
        bool m_bReverse,          /**<! reverse rotation of the needle */
             m_bVisible;          /**<! is the element visible? */

      public:
        /**
         * The constructor
         * @param pGuiEnv the Irrlicht GUI environment
         * @param pParent the parent GUI element
         * @param iId id of the needle indicator
         * @param cRect rect of the needle indicator
         */
        CGUINeedleIndicator(IGUIEnvironment *pGuiEnv, IGUIElement *pParent, s32 iId, core::rect<s32> cRect);

        /**
         * The destructor
         */
        virtual ~CGUINeedleIndicator();

        /**
         * Draw the indicator
         */
        virtual void draw();

        /**
         * Set the background texture
         * @param pBackground the new background texture
         */
        void setBackground(video::ITexture *pBackground);

        /**
         * Add a needle to the indicator
         * @param cCol color of the needle
         * @param fLength length of the needle
         * @param fWidth width of the needle
         * @param fFactor angle factor of the needle
         */
        void addNeedle(video::SColor cCol, f32 fLength, f32 fWidth, f32 fFactor);

        /**
         * set the range of the needle indicator
         * @param fMin minimum value
         * @param fMax maximum value
         */
        void setRange(f32 fMin, f32 fMax);

        /**
         * Get the number of needles in the needle indicator
         * @return the number of needles in the needle indicator
         */
        u32 getNeedleCount();

        /**
         * Get a needle
         * @param i index of the needle
         * @return the needle at index i or NULL if i is out of range
         */
        CNeedle *getNeedle(u32 i);

        /**
         * set the reverse rotation flag
         * @param b the new value
         */
        void setReverse(bool b);

        /**
         * Set the overflow mode
         * @param eMode the new mode
         */
        void setOverflowMode(eOverflowMode eMode);

        /**
         * set the angle range
         * @param fMin the minimum angle
         * @param fMax the maximum angle
         */
        void setAngleRange(f32 fMin, f32 fMax);

        /**
         * set the angle offset
         * @param fOffset the new angle offset
         */
        void setAngleOffset(f32 fOffset);

        /**
         * Set the value the indicator indicates
         * @param f the new value
         */
        void setValue(f32 f);

        /**
         * Set visibility of element
         * @param b new flag
         */
        virtual void setVisible(bool b) { m_bVisible=b; }
    };
  }
}

#endif
