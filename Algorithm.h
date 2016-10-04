#include "eeImage.h"

#include <cmath>
#include <vector>

class Algorithm {
public:

		// default constructors and destructors
	Algorithm ();
	~Algorithm ();

		// set methods
	void setWindowSize(int w);
	void setMSSIM(double m);
	void setStandardDev(double s);
	void setAlpha(double a);
	void setBeta(double b);
	void setGamma(double g);

		// get methods
	int getWindowSize();
	double getMSSIM();
	double getStandardDev();
	double getAlpha();
	double getBeta();
	double getGamma();

		// main SSIM Function
	void SSIM(const eeImage::Image<double> &ReferenceImage, const eeImage::Image<double> &DistortedImage, eeImage::Image<double> &SSIMImg);

private:

		// parameters
	int _windowSize;
	double _MSSIM;
	double _standardDev;
	double _alpha;
	double _beta;
	double _gamma;

		// helper functions-
	std::vector<double> makeGaussianWindow();
	void ConvertToGrayscale(const eeImage::Image<double> &colourImage, eeImage::Image<double> &grayImage);
	double SSIMWindow(int posX, int posY, std::vector<double>& gaussianWindow, eeImage::Image<double>& ReferenceIntensity, eeImage::Image<double>& DistortedIntensity);
};