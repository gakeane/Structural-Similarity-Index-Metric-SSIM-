#include "Algorithm.h"
#include "eeImage.h"
#include "5c1UnitTest.h"

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

namespace UnitTest {

	bool testGaussianSize();
	bool testGaussianData();
	bool testGuassianPoorInput();

	bool testGrayScaleData();
	bool testGrayScaleWithGrayImage();
	bool testGrayScaleNumberChannels();

	bool testSSIMvalue();
	bool testSSIMWindowPlace();
	bool testSSIMMean();
	bool testSSIMStandardDeviation();

	bool testMeanSSIM();
	bool testSSIMImage();
};