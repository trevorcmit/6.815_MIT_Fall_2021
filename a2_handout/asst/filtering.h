/* -----------------------------------------------------------------
 * File:    filtering.h
 * Created: 2015-09-22
 * -----------------------------------------------------------------
 *
 * Image convolution and filtering
 *
 * ---------------------------------------------------------------*/

#ifndef __FILTERING__H
#define __FILTERING__H

#include <vector>
#include "Image.h"


// --------- HANDOUT  PS02 ------------------------------

class Filter {

public:
  // Constructor
  Filter();
  Filter(const std::vector<float> &fData, int fWidth, int fHeight);
  Filter(int fWidth, int fHeight); // kernel with all zero

  // function to convolve your filter with an image
  Image convolve(const Image &im, bool clamp = true) const;

  // Accessors of the filter values
  const float &operator()(int x, int y) const;
  float &operator()(int x, int y);

  int width() const { return width_; }
  int height() const { return height_; }

  // The following are functions and variables that are not accessible from
  // outside the class
private:
  std::vector<float> kernel_;
  int width_;
  int height_;
};

// Box Blurring
Image boxBlur(const Image &im, int k, bool clamp = true);
Image boxBlur_filterClass(const Image &im, int k, bool clamp = true);

// Gradient Filter
Image gradientMagnitude(const Image &im, bool clamp = true);

// Gaussian Blurring
std::vector<float> gauss1DFilterValues(float sigma, float truncate);
std::vector<float> gauss2DFilterValues(float sigma, float truncate);
Image gaussianBlur_horizontal(const Image &im, float sigma,
                              float truncate = 3.0, bool clamp = true);
Image gaussianBlur_separable(const Image &im, float sigma, float truncate = 3.0,
                             bool clamp = true);
Image gaussianBlur_2D(const Image &im, float sigma, float truncate = 3.0,
                      bool clamp = true);

// Sharpen an Image
Image unsharpMask(const Image &im, float sigma, float truncate = 3.0,
                  float strength = 1.0, bool clamp = true);

// Bilateral Filtering
Image bilateral(const Image &im, float sigmaRange = 0.1,
                float sigmaDomain = 1.0, float truncateDomain = 3.0,
                bool clamp = true);
Image bilaYUV(const Image &im, float sigmaRange = 0.1, float sigmaY = 1.0,
              float sigmaUV = 4.0, float truncateDomain = 3.0,
              bool clamp = true);

Image nonlocal_means_bilateral(const Image &im, float sigmaRange=0.1, float sigmaDomain=1.0, bool clamp=true);

// Return impulse image of size k x k x 1
// returned image is all zeros (except at the center where it is white)
Image impulseImg(int k);

// ------------------------------------------------------

#endif
