/* --------------------------------------------------------------------------
 * File:    a2_main.cpp
 * Created: 2015-09-23
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "Image.h"
#include "filtering.h"
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of filter.cpp and Image.cpp
int main() {
  clock_t start = clock();
  // cout << "nothing done in a2_main.cpp, debug me !" << endl;

  // ------- Example tests, change them ! --------------
  // Image im = impulseImg(10);
  // cout << "smart accessor at (1,3,0): " << im.smartAccessor(1, 3, 0, true)
  //      << endl;

  // Image blurred = boxBlur(im, 3, true);
  // cout << "blurred impulse image" << endl;

  // cout << "keep testing..." << endl;

  Image im1("./Input/Cambridge1.png");

  Image sharp_sig1_str1 = unsharpMask(im1, 1.0f, 3.0f, 1.0f);
  Image sharp_sig10_str1 = unsharpMask(im1, 10.0f, 3.0f, 1.0f);
  Image sharp_sig1_str10 = unsharpMask(im1, 1.0f, 3.0f, 10.0f);
  Image sharp_sig10_str10 = unsharpMask(im1, 10.0f, 3.0f, 10.0f);
  sharp_sig1_str1.write("./Output/sharp_sig1_str1.png");
  sharp_sig10_str1.write("./Output/sharp_sig10_str1.png");
  sharp_sig1_str10.write("./Output/sharp_sig1_str10.png");
  sharp_sig10_str10.write("./Output/sharp_sig10_str10.png");
  
  // Image blurred1 = boxBlur(im2, 9, true);
  // blurred1.write("./Output/boxblur_cambridge2.png");

  // Image blurred2 = boxBlur_filterClass(im2, 9, true);
  // blurred2.write("./Output/boxblur_filterClass_cambridge2.png");

  // Image im("./Input/Cambridge2.png");
  // Image gauss2d_n100 = gaussianBlur_2D(im, -100);
  // Image gauss2d_n10  = gaussianBlur_2D(im, -10);
  // Image gauss2d_n1   = gaussianBlur_2D(im, -1);
  // Image gauss2d_1    = gaussianBlur_separable(im, 3);
  // Image gauss2d_10   = gaussianBlur_2D(im, 10);
  // Image gauss2d_100  = gaussianBlur_2D(im, 100);
  // gauss2d_100.write("./Output/gauss_2d_100.png");
  // gauss2d_10.write("./Output/gauss_2d_10.png");
  // gauss2d_1.write("./Output/gauss_2d_3.png");
  // gauss2d_n100.write("./Output/gauss_2d_n100.png");
  // gauss2d_n10.write("./Output/gauss_2d_n10.png");
  // gauss2d_n1.write("./Output/gauss_2d_n1.png");
  // ---------------------------------------------------

  // // ---------------------------------------------------
  // // Test the filter class on an impulse image
  // Image dirac = impulseImg(31);

  // // Test kernel
  // vector<float> kernel{0, 0, 1, 0, 1, 0, 1, 0, 0}; // C++11 syntax
  // Filter testFilter(kernel, 3, 3);
  // Image testOutput = testFilter.convolve(dirac);
  // // The output should be an exact copy of the kernel in the center of the
  // // image
  // testOutput.write("./Output/testKernel.png");
  // // ---------------------------------------------------

  // // ---------------------------------------------------
  // // E.g. test the sobelKernel
  // // create Sobel Filter that extracts horizontal gradients
  // // [ -1 0 1 ]
  // // [ -2 0 2 ]
  // // [ -1 0 1 ]
  // vector<float> fDataX{-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0};
  // Filter sobelX(fDataX, 3, 3);

  // // verify that your filter is correct by using it to filter an impulse image
  // Image impulse = impulseImg(11); // create an image containing an impulse
  // // convolve the impulse image with the Sobel kernel. We divide the output
  // // by 4 and add 0.5 to make the range of the image between 0 and 1
  // Image verifyKernel = sobelX.convolve(impulse) / 4 + 0.5;
  // verifyKernel.write("./Output/verifySobelKernel.png");

  // // filter an image using the sobel kernel
  // Image im2("./Input/lounge_view.png");
  // Image sobelFiltered = sobelX.convolve(im2);

  // // make the range of the output image from 0 to 1 for visualization
  // // since the Sobel filter changes the range of a (0,1) image to (-4,4)
  // Image sobelOut = sobelFiltered / 8 + 0.5;
  // sobelOut.write("./Output/sobelFiltered.png");

  Image im("./Input/lens.png"); // Import lens image
  Image bila = bilateral(im);
  bila.write("./Output/bilateral_lens_2.png");
  // // ---------------------------------------------------

  // // --- Timer example ---------------------------------

  // Image im2("./Input/Cambridge2.png");
  // float sigma = 3.0f;

  // clock_t start0 = clock();
  // Image blur_2d = gaussianBlur_2D(im2, sigma);
  // clock_t end0 = clock();

  // clock_t start1 = clock();
  // Image blur_sep = gaussianBlur_separable(im2, sigma);
  // clock_t end1 = clock();

  // blur_2d.write("gauss_2d_3_timer.png");
  // blur_sep.write("gauss_sep_3_timer.png");

  // double duration0 = (end0 - start0) * 1.0f / CLOCKS_PER_SEC;
  // double duration1 = (end1 - start1) * 1.0f / CLOCKS_PER_SEC;
  // cout << "2D gaussian took: " << duration0 << "s" << endl;
  // cout << "2D gaussian took: " << duration1 << "s" << endl;
  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "a2_main.cpp runtime: " << duration << "s" << endl;
  // // ---------------------------------------------------
}
