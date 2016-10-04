#include "SSIM.h"

SSIM::SSIM (Node* node) : Iop (node) {
}

/*! This is a function that creates an instance of the operator, and is
   needed for the Iop::Description to work.
 */

// this is required to run the plugin
static Iop* SSIMCreate(Node* node) {
    return new SSIM(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
   how to create one, and the menu item to show the user. The menu item may be
   0 if you do not want the operator to be visible.
 */
const Iop::Description SSIM::description ( CLASS, "Examples/SSIM", SSIMCreate );

const char* const SSIM::CLASS = "SSIM";
const char* const SSIM::HELP = "Example Plugin";

const char* SSIM::input_label(int n, char*) const {
	switch (n) {
	case 0: return "Src1";
	case 1: return "Src2";
	default: return "";
	}
}

int SSIM::maximum_inputs() const {return 2;}
int SSIM::minimum_inputs() const {return 2;}

void SSIM::_validate(bool for_real) {
    //the most important thing for this function is to call validate 

    //uncomment for more than 1 input
    for(int i=0; i<inputs(); i++) {
        Op* thisInput = input(i);
        if(thisInput) {
            thisInput->validate(for_real);
        }
    }

    copy_info(0);						// copy bbox channels etc from input0, which will validate it.
    info_.channels(Mask_RGBA);			// these are the channels we are interested in for the Plugin	(Tells channel to use rgb values)

    _isFirstTime = true;
}

void SSIM::_request(int x, int y, int r, int t, ChannelMask channels, int count) {

    // for this example, we're only interested in the RGB channels
    input(0)->request(ChannelMask(Mask_RGBA), count);			//this version requests the entire frame		(asks for the rgb value of desired channel)
    input(1)->request(ChannelMask(Mask_RGBA), count);			//this version requests the entire frame		(asks for the rgb value of desired channel)
    //input(0)->request( x, y, r, t, ChannelMask(Mask_RGB), count );//this version only requests the portion of the frame indicated by the box (x,y,r,t)
}

	// THIS WILL RETURN AN IMAGE (n is the input, offset is the x value in engine, same with channels)
ChannelSetInit SSIM::fetchImage(eeImage::Image<double>& img, int n, int offset, ChannelSet channels ) {

    ChannelSet imgChannels = Mask_None;
    bool fetch_src_ok = true;

    Iop* theInput = (Iop*)input(n, offset);		// creates a pointer to the same location as the image at input 1
    if(!theInput) {								// return an error if there is no input at location n
		error("Iop input not found.");
		fetch_src_ok = false;
		return Mask_None;
	}

    Format format = theInput->format();			// chose input format

    /* these useful format variables are used later */
    const int fx = format.x();
    const int fy = format.y();
    const int fr = format.r();
    const int ft = format.t();

    const int height = ft - fy ;		// height of input image
    const int width = fr - fx ;			// width of input image (we will use this to move correct distance along pointer)

    channels &= theInput->info().channels();		// get the information about the input channels

    int nchannels = channels.size();				// get the number of channels

		// if there are no channels return a blank mask
    if (nchannels == 0) {
		printf("No Channels \n");
		return Mask_None;
	}

    /* if everything's good, we're going to copy the data from our Tile */		
    if( fetch_src_ok ) {

		std::vector<double>::iterator imgIt;		// create an itterator (i.e. pointer that moves in steps of float)

		// Grab and lock the input
		if (aborted()) {
			printf("Aborted Attempt \n");
			return Mask_None;
		}

		Tile tile(*theInput, channels);											// create a Tile object starting at the beginning of input 
		// Tile tile(*theInput, left, bottom, right, top, channels);			// (note that we could specify a tile of desired size

		int tilefx = tile.x();						// get co-ordinates of tiles
		int tilefr = tilefx + tile.w();
		int tilefy = tile.y();
		int tileft = tilefy + tile.h();

		int chanInd;				// this is a channel counter
		Channel chan;				// this will represent the actual ID of the channels that are provided by the Tile

		for (chanInd = 0, chan = channels.first(); chanInd < nchannels; chanInd++, chan = channels.next(chan) ) {

			// this calculates which channels are actually in the tile
			if (intersect(tile.channels(), chan)) {
				imgChannels += (ChannelSetInit)(1 << (chan - 1)); 
			}
		} // END OF FOR LOOP

			// this seems to be restating what was done above
		nchannels = imgChannels.size();
		if (nchannels == 0) {
			printf("No Channels \n");
			return Mask_None;
		}

		const float** rowChan = new const float*[nchannels];	// declare a pointer to a pointer (first pointer points to a float array of size channel)
		img.set(width, height, nchannels);						// set the size of the image we're going to return (values initalised to zero)

			// fy is the bottom of the image and ft is the top
		for (int ry = fy; ry < ft; ry++) {
			if (ry < tilefy || ry >= tileft) {				// if we our outside the vertcal dimensions of the tile
				continue;									// returns to start of loop
			}

			for (chanInd = 0, chan = channels.first(); chanInd < nchannels; chanInd++, chan = channels.next(chan) ) {
				// direct each channel pointer to the appropriate memory location if that channel exists   

				if (intersect(tile.channels(),chan)) {					// check channel exists
					rowChan[chanInd] = &tile[chan][ry][tilefx];			// set channel to point to location in tile
				}
				else {
					rowChan[chanInd] = NULL;
				}
			}

			// read in the image row by row (we are inside the row loop)
			// tile fx is the 
			imgIt = img._data.begin() + (tilefx + ry*width)*nchannels;				// start itterator at the beginning of the image we're going to return

			// move along the row within the dimensions of the tile
			for (int rx = tilefx; rx < tilefr; ++rx) {
				for (chanInd = 0; chanInd < nchannels; chanInd++) {
					// only fills the array for channels that exist in the tile

					if(rowChan[chanInd]) {						// check if cahhnel exists (isn't NULL)
						*imgIt++ = *(rowChan[chanInd])++;		// set the value pointed to
					}
				}
			}

		}

		delete [] rowChan;		// clean up memory
    }

    return imgChannels.bitwiseAnd(Mask_All);		// returns channel mask of image
}

/*! For each line in the area passed to request(), this will be called. It must
   calculate the image data for a region at vertical position y, and between
   horizontal positions x and r, and write it to the passed row
   structure. Usually this works by asking the input for data, and modifying
   it.

 */

// y is the row we are on, x is the starting pixel in the row, r is the final pixel in the row, channels tell us which channels are being used (i.e. RGB, HSL)
// This is called for each row of the image, the outRow passes a value back to Nuke for each row
void SSIM::engine ( int y, int x, int r, ChannelMask channels, Row& outRow ) {

	/* engine calls are multi-threaded so any processing must be locked */
	// we dont want each thread to read in the image so we allow the first thread through and lock out the others until we are finished
	// this part of the function reads in the entire image
	if (_isFirstTime) {
		Guard guard(_lock);			// creates a lock (bars all treads except first from executing code in chain braces (forces single treaded processing))

			// only allows the first thread through to read the image
		if(_isFirstTime) {
			callCloseAfter(0);

			/* Do on-demand processing here to calculate data needed to update the plugin output */
			bool fetch_src_ok = true;

			if (aborted()) {
				error("Iop aborted.");
				fetch_src_ok = false;
				return;
			}

				// read in images (sets _src equal to values obtained through getImage (THIS ONLY GETS CALLED ONCE, GETS ENTIRE IMAGE)
			if(fetch_src_ok) {														// image does not have to contain an alpha channel but it must contain rgb channels
				fetch_src_ok = (fetchImage(_src1, 0, 0, Mask_RGB) == Mask_RGB);		// reads in image data from input number 0 in _src (Image data structure) (the == Mask_RGB is an error check)
				fetch_src_ok = (fetchImage(_src2, 1, 0, Mask_RGB) == Mask_RGB);		// reads in image data from input number 0 in _src (Image data structure) (the == Mask_RGB is an error check)
			}
      
			if(fetch_src_ok) {
				_algorithm.SSIM(_src1, _src2, _dst);		// if we have the data (stored in _src) then convert to grayscale
			}

			_isFirstTime = false;							// so we only enter with one thread
		}
	}

	// now write _dst to the red, green and blue channels. ie. each value of _dst gets assigned to the 3 channels separately
	std::vector<double>::iterator dstDataIt; 
  

	// this bit sends output back to NUKE
	foreach(chan, channels) {											// for each channel for which a new value is requested
		if (intersect(Mask_Alpha, chan)) {									// make sure that it is only a red, green or blue channel (This code will set the RGB values returned to nuke as the same value, if we set to alpha then only the alpha cahhenl will be returned)
			dstDataIt = _dst._data.begin() + (_dst._width * y + x);

			float * row_out = outRow.writable(chan) + x;					

			for(int xx=0; xx<r-x; xx++) {
				row_out[xx] = float(*dstDataIt++);
			}
		}
		else {							// this is for the case where channels are requested that we dont expect. It shouldn't happen if we did our job right
    
			ChannelMask chanMask = chan;
			input0().get(y, x, r, chanMask, outRow);		// just copy it from the input
		} // end of else statement
	} // end of foreach statement
} // end of engine fuction


void SSIM::knobs(Knob_Callback f) {

	int window = _algorithm.getWindowSize();				// get the current window size
	double deviation = _algorithm.getStandardDev();			// get the current standard deviation
	double alpha = _algorithm.getAlpha();					// Luminance weight
	double beta = _algorithm.getBeta();						// Contrast weight
	double gamma = _algorithm.getGamma();					// Structural weight
	double m = _algorithm.getMSSIM();						// get the current MSSIM

		// knobs for adjusting window size and weights
	Int_knob(f, &window, IRange(0, 1), "Window Size", "WindowSize");
	Double_knob(f, &deviation, IRange(0, 10), "Standard deviation", "StandardDev");
	Double_knob(f, &alpha, IRange(0, 5), "Luminance Weight", "Alpha");
	Double_knob(f, &beta, IRange(0, 5), "Contrast Weight", "Beta");
	Double_knob(f, &gamma, IRange(0, 5), "Structure Weight", "Gamma");

		// this is the knob setup for the MSSIM value
	_returnValueKnob = Double_knob(f, &m, IRange(0, 1), "MSSIM Value", "MSSIM");
	_returnValueKnob->set_flag(Knob::OUTPUT_ONLY);
	_returnValueKnob->clear_flag(Knob::SLIDER);
	_returnValueKnob->set_flag(Knob::NO_ANIMATION);

	_algorithm.setWindowSize(window);						// set the new window size
	_algorithm.setStandardDev(deviation);					// set the new standard deviation
	_algorithm.setAlpha(alpha);								// set new Luminance weight
	_algorithm.setBeta(beta);								// set new Contrast weight
	_algorithm.setGamma(gamma);								// set new Structural weight

	return;
}

void SSIM::_close() {

	double m = _algorithm.getMSSIM();
	_returnValueKnob->set_value(m);
}