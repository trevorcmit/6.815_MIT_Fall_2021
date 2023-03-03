/* -----------------------------------------------------------------
 * File:    filtering.cpp
 * Created: 2015-09-22
 * -----------------------------------------------------------------
 *
 * Image convolution and filtering
 *
 * ---------------------------------------------------------------*/

#include "filtering.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include "basicImageManipulation.h"
using namespace std;

Image boxBlur(const Image &im, int k, bool clamp) { // Safe to assume k is odd
  // --------- HANDOUT  PS02 ------------------------------
  Image output(im.width(), im.height(), im.channels());
  for (int h = 0; h < im.height(); h++) { // Iterate all possible pixels given height and width
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
  
      float sum = 0.0f; // Initialize sum for averaging
      for (int h0 = h - (k - 1)/2; h0 < h - (k - 1)/2 + k; h0++) {
        for (int w0 = w - (k - 1)/2; w0 < w - (k - 1)/2 + k; w0++) { // Iterate over kernel-contained coordinates
          sum += im.smartAccessor(w0, h0, c, clamp);
        }
      }
      output(w, h, c) = sum / (k * k); // Put averages in output pixels by dividing by k^2
      }
    }
  }
  return output; // Return output image
}

Image Filter::convolve(const Image &im, bool clamp) const {
  // --------- HANDOUT  PS02 ------------------------------
  Image output(im.width(), im.height(), im.channels()); // Initialize output image of same size
  for (int h = 0; h < im.height(); h++) { // Iterate pixels in row-major iteration order
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
  
      float sum = 0.0f; // Initialize sum for averaging
      int f_h = 0;      // Initialize index for kernel y value
      for (int h0 = h - (height() - 1)/2; h0 < h - (height() - 1)/2 + height(); h0++) { // Row-major local iteration
        int f_w = 0;    // Index for kernel x value
        for (int w0 = w - (width() - 1)/2; w0 < w - (width() - 1)/2 + width(); w0++) {
          sum += im.smartAccessor(w0, h0, c, clamp) * (*this)(f_w, f_h);
          f_w += 1; // Increment kernel x value
        }
        f_h += 1; // Increment kernel y value
      }

      output(w, h, c) = sum; // Put convolution sum into output image
      }
    }
  }
  return output; // Return output image
}

Image boxBlur_filterClass(const Image &im, int k, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  vector<float> kernel;
	for (int n = 0; n < k * k; n++) {
		kernel.push_back(1.0f / (float(k * k))); // Generate averaging kernel of k * k size
	}

	Filter blur(kernel, k, k);       // Initialize filter then return convolution
	return blur.convolve(im, clamp); 
}

Image gradientMagnitude(const Image &im, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Uses a Sobel kernel to compute the horizontal and vertical components
  // of the gradient of an image and returns the gradient magnitude.
  vector<float> horizontal{-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0}; // Make kernels
  vector<float> vertical{-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0};
  Filter grad_h(horizontal, 3, 3); // Make filters
  Filter grad_v(vertical, 3, 3);
  Image im_h = grad_h.convolve(im, clamp); // Convolve components
  Image im_v = grad_v.convolve(im, clamp);

  Image output(im.width(), im.height(), im.channels()); // Initialize output image of same size
  for (int h = 0; h < im.height(); h++) { // Iterate pixels in row-major iteration order
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        output(w, h, c) = sqrt(
          pow(im_h(w, h, c), 2) + pow(im_v(w, h, c), 2) // Gradient formula: sqrt(hor^2 + ver^2)
        );
      }
    }
  }
  return output; // Return output image
}

vector<float> gauss1DFilterValues(float sigma, float truncate) {
  // --------- HANDOUT  PS02 ------------------------------
  int ext = abs(ceil(truncate * sigma)); // Extension of Filter in each direction away from center
	vector<float> filt;
	float sum = 0.0f; // Keep track of sum of values in filter to later normalize to sum to 1
	for (int n = -ext; n <= ext; n++) {
    float value = exp(-1.0f * pow(n, 2) / (2 * pow(sigma, 2))); // Calculate value from Gauss function
		filt.push_back(value);
		sum += value;
	}
  for (int i = 0; i < 1 + 2 * ext; i++) { // Loop over vector again to normalize to sum to 1
    filt.at(i) /= sum;
  }
  return filt;
}

Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  vector<float> kernel = gauss1DFilterValues(sigma, truncate);
  int ext = abs(ceil(truncate * sigma));
  Filter gauss_h(kernel, 1 + 2 * ext, 1);
  return gauss_h.convolve(im, clamp);
}

vector<float> gauss2DFilterValues(float sigma, float truncate) {
  // --------- HANDOUT  PS02 ------------------------------
  int ext = abs(ceil(truncate * sigma)); // Extension of Filter in each direction away from center
	vector<float> filt;
	float sum = 0.0f; // Keep track of sum of values in filter to later normalize to sum to 1
	for (int x = -ext; x <= ext; x++) {
    for (int y = -ext; y <= ext; y++) { // Form 2D kernel through double for-loop iteration
      float value = exp(
        -1.0f * (pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)) // Apply 2D gaussian formula
        );
      filt.push_back(value); // Add value to vector object
      sum += value;
    }
	}
  for (int i = 0; i < (1 + 2 * ext) * (1 + 2 * ext); i++) { // Loop over vector again to normalize to sum to 1
    filt.at(i) /= sum;
  }
  return filt;
}

Image gaussianBlur_2D(const Image &im, float sigma, float truncate,
                      bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  vector<float> kernel = gauss2DFilterValues(sigma, truncate); // Initialize kernel
  int ext = abs(ceil(truncate * sigma));
  Filter gauss_2d(kernel, 1 + 2 * ext, 1 + 2 * ext); // Make square filter
  return gauss_2d.convolve(im, clamp);
}

Image gaussianBlur_separable(const Image &im, float sigma, float truncate,
                             bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Use principles of seperabiltity to blur an image using 2 1D Gaussian
  // Filters
  vector<float> kernel = gauss1DFilterValues(sigma, truncate); // Get 1D gauss values
  int ext = abs(ceil(truncate * sigma));
  Filter gauss_h(kernel, 1 + 2 * ext, 1); // Horizontal filter
  Filter gauss_v(kernel, 1, 1 + 2 * ext); // Vertical filter
  return gauss_v.convolve(gauss_h.convolve(im, clamp), clamp); // Convolute in sequence
}

Image unsharpMask(const Image &im, float sigma, float truncate, float strength,
                  bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Sharpen an image
  Image lowpass = gaussianBlur_separable(im, sigma, truncate); // Initialize lowpass with Gaussian
  Image highpass = im - lowpass;                               // Subtract lowpass from original to get highpass

  Image str(im.width(), im.height(), im.channels());           // Initialize strength for multiplication
  str.set_color(strength, strength, strength);                                     // Set strength equal to input strength
  str = str * highpass;                                        // Multiply by highpass to get product

  return im + str;                                             // Return original + product
}

Image bilateral(const Image &im, float sigmaRange, float sigmaDomain,
                float truncateDomain, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  int ext = abs(ceil(sigmaDomain * truncateDomain));                         // Initialize ceiling value                
	vector<float> d_kernel = gauss2DFilterValues(sigmaDomain, truncateDomain); // Create 2D Gauss kernel values
	Filter domain_filter(d_kernel, 1 + 2 * ext, 1 + 2 * ext);                  // Create spatial (domain) filter
  Image output(im.width(), im.height(), im.channels());

  for (int h = 0; h < im.height(); h++) { // Iterate pixels in row-major iteration order
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        
        float sum = 0.0f, norm = 0.0f;                   // Initialize sum
        int f_h = 0;                                     // Index for kernel y value
        for (int h0 = h - ext; h0 <= h + ext; h0++) {    // Iterate in row-major order
          int f_w = 0;                                   // Index for kernel x value
          for (int w0 = w - ext; w0 <= w + ext; w0++) {

            // Do we loop over ALL colors for each pixel, or just each color for each pixel?
            float range_sum = 0.0f;
            for (int c0 = 0; c0 < im.channels(); c0++) {
              range_sum += pow(im.smartAccessor(w, h, c0, clamp) - im.smartAccessor(w0, h0, c0, clamp), 2);
            }
            // float range_sum = pow(im.smartAccessor(w, h, c, clamp) - im.smartAccessor(w0, h0, c, clamp), 2);


            float range = exp(-1.0f * range_sum / (2 * pow(sigmaRange, 2)));
            float domain = domain_filter(f_w, f_h); // Calculating Domain value from 2D filter
            norm += domain * range; // Save value for normalization later
            sum += domain * range * im.smartAccessor(w0, h0, c, clamp); // Calculating Range value
            f_w += 1; // Increment kernel x value
          }
          f_h += 1; // Increment kernel y value
        }
        output(w, h, c) = sum / norm; // Divide sum by the sum of normalizing values
      }
    }
  }
  return output; // Return output image
}

Image bilaYUV(const Image &im, float sigmaRange, float sigmaY, float sigmaUV,
              float truncateDomain, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // 6.865 only
  // Bilateral Filter an image seperatly for
  // the Y and UV components of an image
  Image im_yuv = rgb2yuv(im); // Convert image to YUV
  Image Y = bilateral(im, sigmaRange, sigmaY, truncateDomain, clamp);
  Image UV = bilateral(im, sigmaRange, sigmaUV, truncateDomain, clamp);

  Image output(im.width(), im.height(), im.channels());
  for (int h = 0; h < im.height(); h++) { // Iterate pixels in row-major iteration order
    for (int w = 0; w < im.width(); w++) {
      output(w, h, 0) = Y(w, h, 0);  // Take Y channel from sigmaY filtering
      output(w, h, 1) = UV(w, h, 1); // Take U channel from sigmaUV filtering
      output(w, h, 2) = UV(w, h, 2); // Take V channel from sigmaUV filtering
    }
  }
  return yuv2rgb(output); // Convert back to RGB space
}

Image nonlocal_means_bilateral(const Image &im, float sigmaRange, float sigmaDomain, bool clamp) {
  // --------- HANDOUT  PS02 EXTRA CREDIT-------------------------
  Image output(im.width(), im.height(), im.channels()); // Initialize output image

  for (int h = 0; h < im.height(); h++) { // Iterate pixels in row-major iteration order
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        
        float sum = 0.0f, norm = 0.0f;            // Initialize sum/norm to calculate pixel valu after iteration
        for (int h0 = 0; h0 < im.height(); h0++) {
          for (int w0 = 0; w0 < im.width(); w0++) {

            // Calculate Range value
            float range_sum = pow(im.smartAccessor(w, h, c, clamp) - im.smartAccessor(w0, h0, c, clamp), 2);
            float range = exp(-1.0f * range_sum / (2 * pow(sigmaRange, 2)));

            // Calculate Domain value
            float domain = exp(
              -1.0f * (pow(h - h0, 2) + pow(w - w0, 2)) / (2 * pow(sigmaDomain, 2))
            );

            norm += domain * range; // Save value for normalization later
            sum += domain * range * im.smartAccessor(w0, h0, c, clamp); // Calculating Range value
          }
        }
        output(w, h, c) = sum / norm; // Divide sum by the sum of normalizing values
      }
    }
  }
  return output; // Return output image
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
Image impulseImg(int k) {
  // initlize a kxkx1 image of all 0's
  Image impulse(k, k, 1);

  // set the center pixel to have intensity 1
  int center = floor(k / 2);
  impulse(center, center, 0) = 1.0f;

  return impulse;
}

// ------------- FILTER CLASS -----------------------
Filter::Filter() : width_(0), height_(0) {}
Filter::Filter(const vector<float> &fData, int fWidth, int fHeight)
    : kernel_(fData), width_(fWidth), height_(fHeight) {
  assert(fWidth * fHeight == (int)fData.size());
}

Filter::Filter(int fWidth, int fHeight)
    : kernel_(std::vector<float>(fWidth * fHeight, 0)), width_(fWidth),
      height_(fHeight) {}

const float &Filter::operator()(int x, int y) const {
  if (x < 0 || x >= width_)
    throw OutOfBoundsException();
  if (y < 0 || y >= height_)
    throw OutOfBoundsException();

  return kernel_[x + y * width_];
}

float &Filter::operator()(int x, int y) {
  if (x < 0 || x >= width_)
    throw OutOfBoundsException();
  if (y < 0 || y >= height_)
    throw OutOfBoundsException();

  return kernel_[x + y * width_];
}
// --------- END FILTER CLASS -----------------------
