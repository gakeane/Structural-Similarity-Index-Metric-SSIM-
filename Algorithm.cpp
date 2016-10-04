#include "Algorithm.h"

	// constructors and destructors
Algorithm::Algorithm () {

	_windowSize = 3;			// default value for windowSize
	_standardDev = 1.5f;		// default value for standard deviation of Gaussian
	_alpha = 1.0f;
	_beta = 1.0f;
	_gamma = 1.0f;
	_MSSIM = 0;
}

Algorithm::~Algorithm () {
	
}

	// set methods
void Algorithm::setWindowSize(int w) {

	_windowSize = w;
}

void Algorithm::setMSSIM(double m) {

	_MSSIM = m;
}

void Algorithm::setStandardDev(double s) {

	_standardDev = s;
}

void Algorithm::setAlpha(double a) {

	_alpha = a;
}

void Algorithm::setBeta(double b) {

	_beta = b;
}

void Algorithm::setGamma(double g) {

	_gamma = g;
}

	// get methods
int Algorithm::getWindowSize() {

	return(_windowSize);
}

double Algorithm::getMSSIM() {

	return(_MSSIM);
}

double Algorithm::getStandardDev() {

	return(_standardDev);
}

double Algorithm::getAlpha() {

	return(_alpha);
}
double Algorithm::getBeta() {

	return(_beta);
}
double Algorithm::getGamma() {

	return(_gamma);
}

	// main Algorithm
void Algorithm::SSIM(const eeImage::Image<double> &ReferenceImage, const eeImage::Image<double> &DistortedImage, eeImage::Image<double> &SSIMImg) {

	std::vector<double>::iterator SSIMIt;							// itterator to move through the output image

		// This is essentailly what we will store in the alpha channel of the final output
	SSIMImg.set(ReferenceImage._width, ReferenceImage._height, 1, 0.0);			// creates an image of size width*height*1 (only one channel) where each element is zero

		// clear output image on each pass
	for (SSIMIt = SSIMImg._data.begin(); SSIMIt < SSIMImg._data.end(); SSIMIt++) {
		*SSIMIt = 0;
	}

		// check that images are the same size
	if (ReferenceImage._width != DistortedImage._width || ReferenceImage._height != DistortedImage._height) {
		printf("Reference Image and Distorted Image are different Sizes");
		return;
	}

	eeImage::Image<double> ReferenceIntensity;		// these will store the intensity values of the reference and distorted images
	eeImage::Image<double> DistortedIntensity;

	// int _windowSize = 3;							// size of each window
	int centerOffset = floor(_windowSize/2.0);		// used to get the center of each window
	int imageWidth = ReferenceImage._width;			// height of Image
	int imageHeight = ReferenceImage._height;		// width of Image
	// double numPoints = pow(_windowSize, 2.0f);	// number of points per window

	std::vector<double> gaussianWindow = makeGaussianWindow();		// create the Gaussian Kernal

		// calculate the intensity Image of the reference Image
	ConvertToGrayscale(ReferenceImage, ReferenceIntensity);

		// calculate the intensity Image of the Distorted Image
	ConvertToGrayscale(DistortedImage, DistortedIntensity);

	// will need two more for loops to cycle through x and y posistions of window
	// ----- START OF MAIN LOOP - CYCLES THROUGH EACH WINDOW ----- //

	// SSIMIt = SSIMImg._data.begin();				// set pointer to start of output image (will store SSIM of each window as a pixel in this image)
	SSIMIt = SSIMImg._data.begin() + centerOffset*(1 + imageWidth);				// This should mean the output pixel is the center of each window

	double SSIM_Sum = 0;					// this counts the SSIM values for each pixel and is used to get the MSSIM

		// each pass represents a pixel in the SSIM image so we need to itterate through this on each pass
		// these two for loops slide the window along the image (first horizontally then vertaiclly)
	for(int posY = 0; posY < (imageHeight - _windowSize); posY++, SSIMIt+=_windowSize) {
		for(int posX = 0; posX < (imageWidth - _windowSize); posX++, SSIMIt++) {

				// calculate the SSIM value for each window
			double valueSSIM = SSIMWindow(posX, posY, gaussianWindow, ReferenceIntensity, DistortedIntensity);

			*SSIMIt = valueSSIM;			// add SSIM value to SSIM image and increment iterator
			SSIM_Sum += valueSSIM;			// total SSIM value of all pixels
		}
	}
	// ----- END OF MAIN LOOP - CYCLES THROUGH EACH WINDOW ----- //

	_MSSIM = SSIM_Sum/((imageHeight - _windowSize)*(imageWidth - _windowSize));		// divide by number windows to get MSSIM value
	printf("Completed the Function \n");

}

	// function creates a gaussian filter kernal based on the window size
std::vector<double> Algorithm::makeGaussianWindow() {

	int numPoints = pow(_windowSize,2.0);		// number of points in kernal
	std::vector<double> guassianWindow;			// vector to store gaussian kernal
	guassianWindow.resize(numPoints);			// set size of the vector

	// move through the array setting points based on the standard deviation of the Guassian

	double sigma = _standardDev;
	double mean = _windowSize/2;				// used to set center of Gaussian kernal at center of window
	double sum = 0.0;							// For accumulating the kernel values
	for (int x = 0; x < _windowSize; ++x) {
		for (int y = 0; y < _windowSize; ++y) {
			guassianWindow.at(y + x*_windowSize) = exp(-0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0))) / (2 * M_PI * sigma * sigma);		// calculate the Guassian at each location

			// Accumulate the kernel values
			sum += guassianWindow.at(y + x*_windowSize);
		}
	}

	printf("Gaussian Filter \n");

	// Normalize the kernel
	for (int x = 0; x < _windowSize; ++x) {
		for (int y = 0; y < _windowSize; ++y) {
			guassianWindow.at(y + x*_windowSize) /= sum;
			printf("%4.2f ", guassianWindow.at(y + x*_windowSize));
		}
		printf("\n");
	}

	return(guassianWindow);
}

	// function converts an image to grayscale
void Algorithm::ConvertToGrayscale(const eeImage::Image<double> &colourImage, eeImage::Image<double> &grayImage) {

	grayImage.set(colourImage._width, colourImage._height, 1, 0.0);			// set size of grayscale

		// iterators to move through colour image and gray image
	std::vector<double>::const_iterator colourImageIt;								
	std::vector<double>::iterator grayimageIt;

			// for when we have an RGB image
	if (colourImage._channels == 3) {
		for (colourImageIt = colourImage._data.begin(), grayimageIt = grayImage._data.begin(); colourImageIt < colourImage._data.end(); colourImageIt+=colourImage._channels, grayimageIt++) {
			*grayimageIt = 0.299f * (*colourImageIt) + 0.587f * (*(colourImageIt+1)) + 0.114f * (*(colourImageIt+2));		
		}
	}

		// for when we have a grayscale image, just copy image over
	else if (colourImage._channels == 1) {
		for (colourImageIt = colourImage._data.begin(), grayimageIt = grayImage._data.begin(); colourImageIt < colourImage._data.end(); colourImageIt+=colourImage._channels, grayimageIt++) {
			*grayimageIt = (*colourImageIt);		
		}
	}

		// when we don't have 1 or 3 channel input
	else {
		printf("Incorrcet number of channels. There are %df channels" ,colourImage._channels);
	}
}

	// function calculates the SSIM value for a single window at posX, posY
double Algorithm::SSIMWindow(int posX, int posY, std::vector<double>& gaussianWindow, eeImage::Image<double>& ReferenceIntensity, eeImage::Image<double>& DistortedIntensity) {

	const double C1 = 0.0001f;					// recommended value for Intensity Constant (0.01L)^2 where L = 1
	const double C2 = 0.0009f;					// recommended value for Variance Constant (0.03L)^2 where L = 1
	const double C3 = C2/2.0f;					// recommended value for covaraince constant

	double meanRefIntensity = 0;			// this will be used to sum the intensity values of all pixels in a window
	double meandisIntensity = 0;			// this need to reset on each pass (new means for each window)
	
	int imageWidth = ReferenceIntensity._width;

		// iterators for moving through reference an distorted image
	std::vector<double>::iterator R_IntensityIt;
	std::vector<double>::iterator D_IntensityIt;

	R_IntensityIt = ReferenceIntensity._data.begin() + posX + imageWidth*posY;		// starts iterator pointing at bottom left of desired window
	D_IntensityIt = DistortedIntensity._data.begin() + posX + imageWidth*posY;		// starts iterator pointing at bottom left of desired window


		// first through the rows (y-axis) --- add remaining length of row to pointers
		// second along the rows (x-axis)
	// move through window to calculate the mean luminance
	for (int i = 0; i < _windowSize; i++, R_IntensityIt+=(imageWidth - _windowSize), D_IntensityIt+=(imageWidth - _windowSize)) {
		for (int j = 0; j < _windowSize; j++, R_IntensityIt++, D_IntensityIt++) {
			meanRefIntensity += (*R_IntensityIt)*(gaussianWindow.at(j + i*_windowSize));		// multiple each pixel intensity by corresponding gaussian kernal
			meandisIntensity += (*D_IntensityIt)*(gaussianWindow.at(j + i*_windowSize));
		}
	}

		// ----- RETURN TO BEGINNING OF WINDOW AND CALCULATE ---- //
		// ------- THE VARAINCE SINCE WE NOW KNOW THE MEAN ------ //
	R_IntensityIt = ReferenceIntensity._data.begin() + posX + imageWidth*posY;		// starts iterator pointing at bottom left of desired window
	D_IntensityIt = DistortedIntensity._data.begin() + posX + imageWidth*posY;		// starts iterator pointing at bottom left of desired window

	double refVarianceSum = 0;				// sum of values used to calculate varaince
	double disVarianceSum = 0;
	double covaraince = 0;

		// cycle through the window again to get the values for the variances and covarainces
	for (int i = 0; i < _windowSize; i++, R_IntensityIt+=(imageWidth - _windowSize), D_IntensityIt+=(imageWidth - _windowSize)) {
		for (int j = 0; j < _windowSize; j++, R_IntensityIt++, D_IntensityIt++) {
			refVarianceSum += (pow((*R_IntensityIt - meanRefIntensity), 2.0))*(gaussianWindow.at(j + i*_windowSize));
			disVarianceSum += (pow((*D_IntensityIt - meandisIntensity), 2.0))*(gaussianWindow.at(j + i*_windowSize));
			covaraince += ((*R_IntensityIt - meanRefIntensity)*(*D_IntensityIt - meandisIntensity))*(gaussianWindow.at(j + i*_windowSize));
		}
	}

	double refVariance = sqrt(refVarianceSum);		// variance of reference window
	double disVariance = sqrt(disVarianceSum);		// veriance of distorted window

		// Calculate the SSIM Value for this window (using default weights)
	// double valueSSIM = ((2*meanRefIntensity*meandisIntensity + C1)*(2*covaraince + C2))/((pow(meanRefIntensity,2.0f) + pow(meandisIntensity,2.0f) + C1)*(pow(refVariance,2.0f) + pow(disVariance,2.0f) + C2));

		// these are the three properties that make up the SSIM algorithm
	double luminance = (2*meanRefIntensity*meandisIntensity + C1)/(pow(meanRefIntensity,2.0) + pow(meandisIntensity,2.0) + C1);
	double contrast = (2*refVariance*disVariance + C2)/(pow(refVariance,2.0) + pow(disVariance,2.0) + C2);
	double structure = (covaraince + C3)/(refVariance*disVariance + C3);

		// Calculate SSIM using non-default weights
	double valueSSIM = (pow(luminance, _alpha))*(pow(contrast, _beta))*(pow(abs(structure), _gamma));
	return(valueSSIM);
}