#include "DDImage/Iop.h"		// class that defines image related pluging in nuke
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/Tile.h"
#include "eeImage.h"
#include "Algorithm.h"

#include <cmath>

using namespace DD::Image;

class SSIM : public Iop
{

public:

    //! Constructor. Initialize user controls to their default values.
    SSIM(Node* node);

    ~SSIM () {}

    void _validate(bool);
    void _request(int x, int y, int r, int t, ChannelMask channels, int count);

    //! This function does all the work.
    void engine ( int y, int x, int r, ChannelMask channels, Row& outRow );

    //! Return the name of the class.
    const char* Class() const { return CLASS; }
    const char* node_help() const { return HELP; }

    virtual int maximum_inputs() const ;		// number of inputs plugin can have
    virtual int minimum_inputs() const ;

    virtual const char* input_label(int n, char*) const;		// names of inputs
    virtual void knobs(Knob_Callback f);

private:
    bool _isFirstTime;
    Lock _lock;

    eeImage::Image<double> _src1, _src2, _dst;			// instance of image class (with floating point pixel values)
    Algorithm _algorithm;								// instance of algorithm class used to run SSIM processing
	Knob* _returnValueKnob;								// this is used to set a value for the knob

    //! Information to the plug-in manager of DDNewImage/NUKE.
    static const Iop::Description description;
    static const char* const CLASS;
    static const char* const HELP;

    //private functions
    ChannelSetInit fetchImage( eeImage::Image<double>& img, int n, int offset, ChannelSet channels );			// reads one of the inages from one of the inputs

	void _close();
}; 