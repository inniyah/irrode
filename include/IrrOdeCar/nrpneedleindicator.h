/*
Original file by Dalerank.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

/*
* Original code copied from QTNeedleIndicator4
* THIS FILE WAS CREATED BY Dalerank 22 may 2010.
*/

#ifndef NRPNEEDLEINDICATOR_H
#define NRPNEEDLEINDICATOR_H
#include <irrlicht.h>

namespace irr
{

namespace gui
{

class CNrpRotatableImage;

class CNrpNeedleIndicator : public IGUIElement
{
public:
    /**
      * Creates CNrpNeedleIndicator with default settings.
      */
	CNrpNeedleIndicator( const wchar_t* text, IGUIEnvironment* environment,
						 IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
						 bool background = false  );
    /**
     * Set scale major ticks number. Default is 11 ticks. Don't set too many
     * ticks, because scale numbers will overlap.
     * @param ticks Major ticks number
     */
    void SetMajorTicks(int ticks);

    /**
     * Set scale major ticks number.
     * @returns Major ticks number
     */
    int  GetMajorTicks(void) { return (int)majorTicks_; }
     /**
     * Set scale minor ticks number. Default is 4 ticks.
     * @param ticks Minor ticks number
     */
    void SetMinorTicks(int ticks);
    /**
     * Get scale minor ticks number.
     * @returns Minor ticks number
     */
    int  GetMinorTicks(void) { return (int)minorTicks_; }
    /**
      * Set label font. Label is displayed below needle axis.
      * @param font Label font.
      */
    void SetLabelFont( IGUIFont* font);
     /**
      * Get scale digit font.
      * @returns Scale digit font.
      */
    IGUIFont* GetLabelFont(void) { return labelFont_; }
    /**
      * Set scale digit font.
      * @param font Scale digit font.
      */
    void  SetDigitFont(IGUIFont* font);

     /**
      * Get scale digit font.
      * @returns font Scale digit font.
      */
    IGUIFont* GetDigitFont(void) { return digitFont_; }
    /**
      * Set indicator range. All values passed to the indicator
      * will be coerced to this range. Scale will be drawn
      * to reflect this range too, so pay attention to set
      * sane values divisible by number of scale ticks.
      * @see setMinValue
      * @see setMaxValue
      * @param min Lowest possible value
      * @param max Maximum possible value.
      */
    void SetRange( f32 min_, f32 max_);
    /**
      * Set indicator minumum level
      * @see setRange
      * @param max Minimum number shown by the indicator.
      */
    void  SetMinValue( f32 min_);
    /**
      * Get lower range boundary.
      * @returns Lower range boundary
      */
    f32 GetMinValue(void) { return min_; }
    /**
     * Set indicator maximum level
     * @see setRange
     * @param max Maximum number shown by the indicator.
     */
    void  SetMaxValue(f32 max_);
     /**
      * Get upper range boundary.
      * @returns Upper range boundary
      */
    f32 GetMaxValue(void) { return max_; }

	/**
      * Set animated property. If true, the needle will be animated
      * and will reach set value smoothly. If false, needle will be
      * drawn immediately.
      * @param anim Turn animation on when true and off when false.
      */
    void SetAnimated(bool amount);
    /**
      * Returns animated property.
      * @returns True if needle is animated. False if not.
      */
    bool IsAnimated(void);
    /**
      * Set label text. If label is empty, no label will be drawn.
      * Keep label short and sweet and to the point. It will not
      * autoscale to fit the meter.
      * @see setLabelFont
      * @param label Label text.
      */
	void SetLabel( core::stringw label );
    /**
      * Get label text.
      * returns Label text
      */
	core::stringw GetLabel(void) { return label_; }
    /**
      * Set digit format. fmt is printf-like fmt string. Default is %.2f.
      * @parameter fmt Digit format.
      */
	void    SetDigitFormat( core::stringc fmt );
    /**
      * Get scale digit format string.
      * @returns Format string.
      */
	core::stringc GetDigitFormat(void) { return scaleFormat_; }

    /**
      * Set gap angle between first and last tick. Resulting scale will
      * have (360 - gap) degrees. Vale in degrees. Default is 90deg.
      * @param gap Scale gap angle in degrees
      */
    void  SetGapAngle( f32 gap );
    /**
      * Get scale gap angle.
      * @returns Gap angle in degrees.
      */
    f32 GetGapAngle(void) { return gapAngle_; }

	void SetStartGapAngle( f32 angle );
	f32 GetStartGapAngle() { return startGapAngle_; }
    /**
      * Set label offset. Label will be positioned between needle axis
      * and shield edge. 0 is needle level, 1 is edge level, 0.5 is between needle and edge,
      * 0.75 is closer to the edge, etc. Value must be between 0 and 1.
      * @param offset Label offset (0.0-1.0).
      */
    void  SetLabelOffset( f32 offset);
    /**
      * Get label offset.
      * @returns Label offset (0.0 - 1.0)
      */
    f32 getLabelOffset(void) { return labelOffset_/115.0f; }
    /**
      * Get currently set value. This is NOT actual needle position during animation, but
      * final indicator value.
      * @returns Indicator value.
      */
    f32 GetValue(void);

	/**
	* Set indicator value. If animated, this will also start needle animation. If
	* not animated, this will redraw the meter.
	* @param value New indicator value. Value will be coerced to min/max.
	*/
	void SetValue(f32 value);

	void SetBackgroundTexture( video::ITexture* ptx );

	void SetNeedleTexture( video::ITexture* ptx );

	void draw();

	void SetFrameRate( s32 amount ) { frameRate_ = amount; }

	void SetPieNumber( s32 amount ) { numPie_ = amount; }

	void setDrawLastTick(bool b) { drawLastTick=b; }
	
	//added by Brainsaw
	void setNeedleColor(video::SColor col) { needleColor_=col; }

private:
    /* addition by Brainsaw */
    bool drawLastTick;

    f32 max_;              // max indicator value (final on scale)
    f32 min_;              // min indicator value (first on scale)
    f32 step_;             // scale tick step
    f32 value_;            // target indicator value
    f32 currValue_;        // current value (needle position during animation)
    f32 majorTicks_;       // Number of major scale ticks
    f32 minorTicks_;       // Number of mainor scale ticks
    f32 rotDeg_, rotRad_; // Scale number rotation step (deg & radians)
    f32 startAngle_, stopAngle_, gapAngle_, startGapAngle_; // scale angles used for step/rotations/etc calculation
    IGUIFont* digitFont_;
    IGUIFont* labelFont_;
	core::stringw label_;
	core::stringc scaleFormat_; // scale format string
	scene::SMeshBuffer mesh_;
	core::matrix4 matrix_;
	
	video::SColor needleColor_; //added by Brainsaw

	core::recti backgroundRect_;

	video::ITexture* background_;
	gui::CNrpRotatableImage* needleImg_;
    //QTimer *timer;          // Animation timer
    int   animFrame_;        // Animation frame coutner
    f32 valueDelta_;       // Delta between current and target needle position value. Used in animation
    bool  animated_;         // Animation flag. Animate needle when true
    f32 labelOffset_;      // Label position between axis needle (when 0.0) and shield edge (when 1.0)
	s32 frameRate_;
	s32 numPie_;

    void  DrawBackground_(void);     /*!< Draw background shield        */
    void  DrawNeedle_(void);         /*!< Draw needle                   */
    f32   Value2Angle_(f32 value); /*!< Convert value to needle angle */
    void  StartAnimation_(void);     /*!< Start animation timer         */
	void  DrawLabel_( f32 valuel, core::position2di beginp, core::position2di endp );
	void  Animate_(void);     // needle animation; fired from QTimer
	void  UpdateMesh_();
	void  DrawMesh_();
};

}//end namespace gui

}//end namespace irr

#endif // QNEEDLEINDICATOR_H

