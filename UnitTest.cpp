#include "UnitTest.h"

	// 1. check size of gaussian is correct
bool UnitTest::testGaussianSize() {
	
	Algorithm algorithm;
	algorithm.setWindowSize(3);

	std::vector<float> Gaussian = algorithm.makeGaussianWindow();

	if (Gaussian.size() == 9) {
		printf("Window of correct size generated\n");
		return(true);
	}

	printf("Incorrect Window Size Generated\n");
	return(false);
}

	// 2. here ground truth is taken from the matlab function fspecial
bool UnitTest::testGaussianData() {
	
	int precision = 6;

	Algorithm algorithm;				// creat algorithm object to run function
	algorithm.setWindowSize(3);			// set window size to three
	algorithm.setStandardDev(1.5f);		// set standard deviation to 1.5

		// ground truth taken from matlab
	float tmp[] = {0.094741658f, 0.118318013f, 0.094741658f, 0.118318013f, 0.147761316f, 0.118318013f, 0.094741658f, 0.118318013f, 0.094741658f};

	std::vector<float> Gaussian = algorithm.makeGaussianWindow();		// generate our guassian window
	std::vector<float> GroundTruth(tmp, tmp + 9);						// add ground truth data to a vector

	for (int i = 0; i < 9; i++) {														// check each value in window
		int iAnswer = int(Gaussian.at(i) * pow(10.0f, precision) + 0.5f);				// calculates value to precision (four) decimal places of accuracy
		int iGroundTruth = int(GroundTruth.at(i) * pow(10.0f, precision) + 0.5f);
		if (iAnswer != iGroundTruth) {													// compare function value with ground truth
			printf("Function Values don't correspond to ground truth\n");
			return(false);
		}
		// printf("Comparison %i: %i %i\n", i, iAnswer, iGroundTruth);
	}

	printf("All function values match the ground truth\n");
	return(true);
}

	// 3. test preformance of function when given an incorrect input
bool UnitTest::testGuassianPoorInput() {

	Algorithm algorithm;					// creat algorithm object to run function
	algorithm.setWindowSize(-3);			// set window size to three
	algorithm.setStandardDev(-1.5f);		// set standard deviation to 1.5

	std::vector<float> Gaussian = algorithm.makeGaussianWindow();		// generate our guassian window

	if (Gaussian.at(0) == -1.0f) {										// function should return -1 if an error occurs
		printf("Unallowed window size or standard deviation\n");
		return(true);
	}

	return (false);
}



	// 4. uses groundtruth Generated in Matlab
bool UnitTest::testGrayScaleData() {


	eeImage::Image<float> colorImage;		// create 2 images (first one is colour image to be converted)
	eeImage::Image<float> grayImage;		// second is a place holder for converted image
	eeImage::Image<float> groundTruth;		// image used to hold ground truth

	colorImage.set(256, 256, 3, 0.0);				// set the input image to be grayscale
	groundTruth.set(256, 256, 1, 0.0);				// initalise ground truth
	std::vector<float>::iterator Iter1;				// create interator for input 1 to set data in file
	std::vector<float>::iterator Iter2;				// create interator for input 2 to read data

	ifstream myfile;

		// read in colour image data
	Iter1 = colorImage._data.begin();
	myfile.open("LenaColourImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter1)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in grayscale ground truth data (grayscale ground truth from matlab only accuarte to interger values)
	Iter1 = groundTruth._data.begin();
	myfile.open("GrayScaleGroundTruth.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter1)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	Algorithm algorithm;									// creat algorithm object to run function
	algorithm.ConvertToGrayscale(colorImage, grayImage);			// convert grayscale image to grayscale

		// check returned image is the correct size
	if (grayImage._height != groundTruth._height || grayImage._width != groundTruth._width || grayImage._channels != groundTruth._channels) {
		printf("Returned Value is incorrect size \n");
		return(false);
	}

		// compare returned image with ground truth
	for (Iter1 = grayImage._data.begin(), Iter2 = groundTruth._data.begin(); Iter1 < grayImage._data.end(); Iter1++, Iter2++) {
		int val = int(*Iter1) != int(*Iter2);
		if (abs(val) > 1) {
			printf("Returned Image data doesn't match ground truth \n");
			return(false);
		}
	}

	printf("Returned Grayscale image matches the ground truth \n");
	return(true);
}

	// 5. checks GrayScale function when given a grayscale input
bool UnitTest::testGrayScaleWithGrayImage() {

	eeImage::Image<float> Input1;		// create 2 images (first one is colour image to be converted)
	eeImage::Image<float> Input2;		// second is a place holder for converted image

	Input1.set(512, 256, 1, 0.0);					// set the input image to be grayscale
	std::vector<float>::iterator Iter1;				// create interator for input 1 to set data in file
	std::vector<float>::iterator Iter2;				// create interator for input 2 to read data

		// place random values (between 0 and 1) in input Image
	for (Iter1 = Input1._data.begin(); Iter1 < Input1._data.end(); Iter1++) {
		*Iter1 = ((float)rand() / (RAND_MAX));
	}

	Algorithm algorithm;							// creat algorithm object to run function
	algorithm.ConvertToGrayscale(Input1, Input2);	// convert grayscale image to grayscale

		// check that image dimensions match
	if (Input1._height != Input2._height || Input1._width != Input2._width || Input1._channels != Input2._channels) {
		printf("Returned Value is different size to the input Value \n");
		return(false);
	}

		// check that image data matches
	for (Iter1 = Input1._data.begin(), Iter2 = Input2._data.begin(); Iter1 < Input1._data.end(); Iter1++, Iter2++) {
		if (*Iter1 != *Iter2) {
			printf("Returned Image data doesn't match input image data \n");
			return(false);
		}
	}

	return(true);			// return true if function returns same grayscale image values
}

	// 6. checks grayscale function with input containing number of channels that aren't 1 or 3
bool UnitTest::testGrayScaleNumberChannels() {

	eeImage::Image<float> Input1;		// create 2 images (first one is colour image to be converted)
	eeImage::Image<float> Input2;		// second is a place holder for converted image

	Input1.set(512, 256, 2, 0.0);					// set the input image to an unusable number of channels

	Algorithm algorithm;							// creat algorithm object to run function
	algorithm.ConvertToGrayscale(Input1, Input2);	// convert grayscale image to grayscale

	std::vector<float>::iterator Iter;				// create interator to move through returned image

		// check that image dimensions match
	if (Input1._height != Input2._height || Input1._width != Input2._width) {
		printf("Returned Value is different size to the input Value \n");
		return(false);
	}

		// should return empty image (with one channel) of same size if incorrect number of channels
	for (Iter = Input2._data.begin(); Iter < Input2._data.end(); Iter++) {
		if(*Iter != 0) {
			printf("Returned Image was not blank \n");
			return(false);
		}
	}

	return(true);
}

	// 7. this tests whether the SSIM function calculates the correct value for a window and to what precision
	// ground truth is taken from matlab
bool UnitTest::testSSIMvalue() {

	float GroundTruth = 0.999988115608216f;				// only accurate to three decimal places
	int precision = 3;

	int posX = 0;		// set bottom left corner of window
	int posY = 0;

	Algorithm algorithm;				// create an algorithm object to run the function
	algorithm.setWindowSize(11);		// set the window size to 11
	algorithm.setStandardDev(1.5);		// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorrted image

	std::vector<float>::iterator Iter;			// create interator to read data from files

	ifstream myfile;

		// read in Reference Image
	Iter = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Colour Image
	Iter = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	float SSIM_value = algorithm.SSIMWindow(posX, posY, gaussianWindow, ReferenceImage, DistortedImage);		// calculate the SSIM values

	int iAnswer = int(SSIM_value * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
	int iGroundTruth = int(GroundTruth * pow(10.0f, precision) + 0.5f);
	if (iAnswer != iGroundTruth) {													// compare function value with ground truth
		printf("SSIM value of window doesn't match ground truth\n");
		return(false);
	}

	return(true);
}

	// 8. this function checks the SSIM value for all windows
	// this has the benifit of determining if the window is being shifted correctly
bool UnitTest::testSSIMWindowPlace() {

	int precision = 3;

	int imageHeight = 256;
	int imageWidth = 256;
	int windowSize = 11;

	Algorithm algorithm;					// create an algorithm object to run the function
	algorithm.setWindowSize(windowSize);	// set the window size to 11
	algorithm.setStandardDev(1.5);			// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)
	eeImage::Image<float> GroundTruth;			// Ground truth Image (read from file)

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorted image
	GroundTruth.set(256, 256, 1, 0.0);			// set size of distorted image

	std::vector<float>::iterator Iter;			// create interator to read data from files

	ifstream myfile;

		// read in Reference Image
	Iter = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Distorted Image
	Iter = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in SSIM Ground truth
	Iter = GroundTruth._data.begin();
	myfile.open("SSIMGroundTruth.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	Iter = GroundTruth._data.begin();		// return iterator to start of ground truth

		// slide window through image, calculate SSIM value for each window, compare to ground truth
	for(int posY = 0; posY < (imageHeight - windowSize); posY++) {
		for(int posX = 0; posX < (imageWidth - windowSize); posX++, Iter++) {

			float SSIM_value = algorithm.SSIMWindow(posX, posY, gaussianWindow, ReferenceImage, DistortedImage);		// calculate the SSIM values

			int iAnswer = int(SSIM_value * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
			int iGroundTruth = int(*(Iter) * pow(10.0f, precision) + 0.5f);
			if (iAnswer != iGroundTruth) {													// compare function value with ground truth
				printf("SSIM value of window doesn't match ground truth at posistion %i %i\n", posX, posY);
				return(false);
			}
		}
	}

	return(true);
}

	// 9. This test checks that the average luminance is being correctly calculated
bool UnitTest::testSSIMMean() {

	float GroundTruth = 26.2812775771998f;				// only accurate to three decimal places
	int precision = 2;

	int posX = 0;		// set bottom left corner of window
	int posY = 0;

	Algorithm algorithm;				// create an algorithm object to run the function
	algorithm.setWindowSize(11);		// set the window size to 11
	algorithm.setStandardDev(1.5);		// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)
	eeImage::Image<float> LuminanceValues;		// Luminance values ground truth

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorrted image
	LuminanceValues.set(11, 11, 1, 0.0);


	std::vector<float>::iterator Iter;			// create interator to read data from files

	ifstream myfile;

		// read in Reference Image
	Iter = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Colour Image
	Iter = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in SSIM Ground truth
	Iter = LuminanceValues._data.begin();
	myfile.open("LuminanceGroundTruth.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	float luminanceValues[121]; 
	float meanLuminance;
	float SSIM_value = algorithm.SSIMWindowLuminance(posX, posY, gaussianWindow, ReferenceImage, DistortedImage, luminanceValues, meanLuminance);		// calculate the SSIM values

		// check mean luminance value is correct
	int iAnswer = int(meanLuminance * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
	int iGroundTruth = int(GroundTruth * pow(10.0f, precision) + 0.5f);
	if (iAnswer != iGroundTruth) {													// compare function value with ground truth
		printf("Mean Luminance value of window doesn't match ground truth\n");
		return(false);
	}

	Iter = LuminanceValues._data.begin();		// return iterator to start of ground truth

		// check each indivial luminance value is correct
	for (int i = 0; i < 121; i++) {
		int iAnswer = int(*(luminanceValues+i) * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
		int iGroundTruth = int(*(Iter)++ * pow(10.0f, precision) + 0.5f);
		if (iAnswer != iGroundTruth) {															// compare function value with ground truth
			printf("Luminance value at %i doesn't match ground truth\n", i);
			return(false);
		}
	}

	return(true);
}

	// 10. This test checks that the standard deviation of the luminance is being correctly calculated
bool UnitTest::testSSIMStandardDeviation() {
	float GroundTruth = 1.78168370340330f;				// only accurate to three decimal places
	int precision = 3;

	int posX = 0;		// set bottom left corner of window
	int posY = 0;

	Algorithm algorithm;				// create an algorithm object to run the function
	algorithm.setWindowSize(11);		// set the window size to 11
	algorithm.setStandardDev(1.5);		// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)
	eeImage::Image<float> Deviations;			// Luminance values ground truth

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorrted image
	Deviations.set(11, 11, 1, 0.0);


	std::vector<float>::iterator Iter;			// create interator to read data from files

	ifstream myfile;

		// read in Reference Image
	Iter = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Colour Image
	Iter = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in SSIM Ground truth
	Iter = Deviations._data.begin();
	myfile.open("DeviationGroundTruth.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	float deviationValues[121]; 
	float standardDeviation;
	float SSIM_value = algorithm.SSIMWindowDeviation(posX, posY, gaussianWindow, ReferenceImage, DistortedImage, deviationValues, standardDeviation);		// calculate the SSIM values

		// check mean luminance value is correct
	int iAnswer = int(standardDeviation * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
	int iGroundTruth = int(GroundTruth * pow(10.0f, precision) + 0.5f);
	if (iAnswer != iGroundTruth) {													// compare function value with ground truth
		printf("standard Deviation value of window doesn't match ground truth\n");
		return(false);
	}

	Iter = Deviations._data.begin();		// return iterator to start of ground truth

		// check each indivial luminance value is correct
	for (int i = 0; i < 121; i++) {
		int iAnswer = int(*(deviationValues+i) * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
		int iGroundTruth = int(*(Iter)++ * pow(10.0f, precision) + 0.5f);
		if (iAnswer != iGroundTruth) {															// compare function value with ground truth
			printf("Deviation value at %i doesn't match ground truth\n", i);
			return(false);
		}
	}

	return(true);
}

	// 11. This test checks that the standard deviation of the luminance is being correctly calculated
bool UnitTest::testMeanSSIM() {

	float GroundTruth = 0.839556343036474f;				
	int precision = 1;

	Algorithm algorithm;				// create an algorithm object to run the function
	algorithm.setWindowSize(11);		// set the window size to 11
	algorithm.setStandardDev(1.5);		// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)
	eeImage::Image<float> SSIMImg;				// Stores SSIM Image

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorrted image

	std::vector<float>::iterator Iter;			// create interator to read data from files

	ifstream myfile;

		// read in Reference Image
	Iter = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Colour Image
	Iter = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

	float SSIM_value = algorithm.SSIMTest(ReferenceImage, DistortedImage, SSIMImg);		// calculate the SSIM values

	int iAnswer = int(SSIM_value * pow(10.0f, precision) + 0.5f);					// calculates value to precision (four) decimal places of accuracy
	int iGroundTruth = int(GroundTruth * pow(10.0f, precision) + 0.5f);
	if (iAnswer != iGroundTruth) {													// compare function value with ground truth
		printf("MSSIM value doesn't match ground truth\n");
		return(false);
	}

	return(true);
}

	// 12. checks that SSIM image Matches Ground truth
	// this function won't work since matlab removes the black pixels at the border
bool UnitTest::testSSIMImage() {

	int precision = 3;

	int imageHeight = 256;
	int imageWidth = 256;
	int windowSize = 11;

	Algorithm algorithm;					// create an algorithm object to run the function
	algorithm.setWindowSize(windowSize);	// set the window size to 11
	algorithm.setStandardDev(1.5);			// set the standard deviation to 1.5

	std::vector<float> gaussianWindow = algorithm.makeGaussianWindow();		// generate the gaussian window

	eeImage::Image<float> ReferenceImage;		// Reference Image (read from file)
	eeImage::Image<float> DistortedImage;		// Distorted Image (read from file)
	eeImage::Image<float> SSIMImg;
	eeImage::Image<float> GroundTruth;			// Ground truth Image (read from file)

	ReferenceImage.set(256, 256, 1, 0.0);		// set size of reference image
	DistortedImage.set(256, 256, 1, 0.0);		// set size of distorted image
	GroundTruth.set(256, 256, 1, 0.0);			// set size of distorted image

	std::vector<float>::iterator Iter1;			// create interator to read data from files
	std::vector<float>::iterator Iter2;			// create interator to scan SSIM image

	ifstream myfile;

		// read in Reference Image
	Iter1 = ReferenceImage._data.begin();
	myfile.open("EinsteinImage.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter1)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in Distorted Image
	Iter1 = DistortedImage._data.begin();
	myfile.open("SaltPepperNoise.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter1)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();

		// read in SSIM Ground truth
	Iter1 = GroundTruth._data.begin();
	myfile.open("SSIMGroundTruth.txt");

	if (myfile) {        
		float value;

		// read the elements into the image
		while (myfile >> value) {
			*(Iter1)++ = value;				// store value then incrment pointer
		}
	}
	myfile.close();


	algorithm.SSIM(ReferenceImage, DistortedImage, SSIMImg);		// calculate the SSIM Image

	Iter1 = GroundTruth._data.begin();		// return iterator to start of ground truth
	Iter2 = SSIMImg._data.begin();			// set itterator to start of SSIM image

	for (Iter1 = GroundTruth._data.begin(), Iter2 = SSIMImg._data.begin(); Iter1 < GroundTruth._data.end(); Iter1++, Iter2++) {
		int iAnswer = int(*(Iter2) * pow(10.0f, precision) + 0.5f);						// calculates value to precision (four) decimal places of accuracy
		int iGroundTruth = int(*(Iter1) * pow(10.0f, precision) + 0.5f);
		if (iAnswer != iGroundTruth) {													// compare function value with ground truth
			printf("SSIM Image pixel value doesn't match ground truth");
			return(false);
		}
	}

	return(true);
}