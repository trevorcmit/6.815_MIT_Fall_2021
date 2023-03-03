// hdr.cpp
// Assignment 5

#include "hdr.h"
#include "filtering.h"
#include <algorithm>
#include <math.h>

using namespace std;

/**************************************************************
 //                       HDR MERGING                        //
 *************************************************************/

Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi) {
  // --------- HANDOUT  PS04 ------------------------------
  // Generate a weight image that indicates which pixels are good to use in
  // HDR, i.e. weight=1 when the pixel value is in [epsilonMini, epsilonMaxi].
  // The weight is per pixel, per channel.
  Image output(im.width(), im.height(), im.channels()); // Initialize output image
  for (int h = 0; h < output.height(); h++) {
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {
        if ((im(w, h, c) <= epsilonMaxi) && (im(w, h, c) >= epsilonMini)) {
          output(w, h, c) = 1.0f; // 1 if inside range
        }
        else {
          output(w, h, c) = 0.0f; // 0 otherwise
        }
      }
    }
  }
  return output; // Return weight image
}

float computeFactor(const Image &im1, const Image &w1, const Image &im2,
                    const Image &w2) {
  // --------- HANDOUT  PS04 ------------------------------
  // Compute the multiplication factor between a pair of images. This
  // gives us the relative exposure between im1 and im2. It is computed as
  // the median of im2/(im1+eps) for some small eps, taking into account
  // pixels that are valid in both images.
  vector<float> f_vec;
  Image im1_adj = im1 + 0.0000000001; // Add correction value for divide by zero errors
  for (int h = 0; h < im1.height(); h++) {
    for (int w = 0; w < im1.width(); w++) {
      for (int c = 0; c < im1.channels(); c++) {
        if ((w1(w, h, c) > 0.9) && (w2(w, h, c) > 0.9)) {   // If weights for both are 1 at this pixel
          f_vec.push_back(im2(w, h, c) / im1_adj(w, h, c)); // Add quotient to factor
        }
      }
    }
  }
  sort(f_vec.begin(), f_vec.end()); // Sort in ascending order
  if (f_vec.size() % 2 == 0) {      // Take median of even number of elements vector
    return (f_vec[floor(f_vec.size() / 2)] + f_vec[floor(f_vec.size() / 2 - 1)]) / 2;
  }
  else { // Take median of odd number of elements vector
    return f_vec[floor(f_vec.size() / 2)];
  }
}

Image makeHDR(vector<Image> &imSeq, float epsilonMini, float epsilonMaxi) {
  // --------- HANDOUT  PS04 ------------------------------
  Image output(imSeq.at(0).width(), imSeq.at(0).height(), imSeq.at(0).channels()); // Initialize output

  vector<float> k_i;            // Initialize vector to hold k_i values
  k_i.push_back(1.0f);          // Add 1.0 as first factor
  vector<Image> weights;        // Initialize vector to store weights so we don't compute multiple times
  Image weight_0 = computeWeight(imSeq.at(0), epsilonMini, 1.0f); // First image weight
  weights.push_back(weight_0);  // Push first weight onto vector

  for (int n = 1; n < imSeq.size(); n++) { // Iterate over all and make factor compared to first image
    if (n == imSeq.size() - 1) {
      Image weight = computeWeight(imSeq.at(n), 0.0f, epsilonMaxi); // In brightest case, don't chop darkest
      k_i.push_back(computeFactor(imSeq.at(n - 1), weights.at(weights.size() - 1), imSeq.at(n), weight)); 
      weights.push_back(weight); // For line above, use weight from previous image and not against im_0
    }
    else {
      Image weight = computeWeight(imSeq.at(n), epsilonMini, epsilonMaxi); // Use epsilons in normal case
      k_i.push_back(computeFactor(imSeq.at(n - 1), weights.at(weights.size() - 1), imSeq.at(n), weight));
      weights.push_back(weight); // For line above, use weight from previous image and not against im_0
    }
  }

  for (int h = 0; h < output.height(); h++) { // Iterate to create output image
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) { // Row-major order, over all channels

        float denom = 0.0f, sum = 0.0f, factor = 1.0f; // Initialize variables for pixel calculation
        for (int n = 0; n < imSeq.size(); n++) {
          for (int m = 0; m < n; m++) {
            factor *= k_i.at(m + 1) / k_i.at(m); // Chain previous factors together
          }
          denom += weights.at(n)(w, h, c);       // Divide by weight summation
          sum += weights.at(n)(w, h, c) * (1 / factor) * imSeq.at(n)(w, h, c); // Perform formula
        }
        if (denom < 1.0) { // If no pixels have weight, take the pixel from the first image
          output(w, h, c) = imSeq.at(0)(w, h, c);
        }
        else {
        output(w, h, c) = sum / denom; // Set pixel to summation divided by weight summation
        }
      }
    }
  }
  return output; // Return output HDR image
}

/**************************************************************
 //                      TONE MAPPING                        //
 *************************************************************/

Image toneMap(const Image &im, float targetBase, float detailAmp, bool useBila, float sigmaRange) {
  // --------- HANDOUT  PS04 ------------------------------
  // tone map an hdr image
  // - Split the image into its luminance-chrominance components.
  // - Work in the log10 domain for the luminance
  Image output(im.width(), im.height(), im.channels()); // Initialize output image
  vector<Image> lumiChromi_vect = lumiChromi(im); // Create lumi/chromi and set to variables
  Image lumi = lumiChromi_vect.at(0), chromi = lumiChromi_vect.at(1);

  Image log10lumi = log10Image(lumi); // Turn luminance into log10 scale

  float spatial_SD = 0.0f;
  if (im.width() > im.height()) {    // Get sigma for spatial Gaussian
    spatial_SD = im.width() / 50.0f; // Width is larger
  }
  else {
    spatial_SD = im.height() / 50.0f; // Height is larger
  }

  Image base_lumi(im.width(), im.height(), im.channels()); // Make base (blurry luminance)
  if (useBila) {
    base_lumi = bilateral(log10lumi, sigmaRange, spatial_SD, 3.0f); // Bilateral version
  }
  else {
    base_lumi = gaussianBlur_separable(log10lumi, spatial_SD, 3.0f); // Gaussian version
  }

  Image difference = log10lumi - base_lumi; // Detail is difference of log luminance minus base

  // Reduce constrast on the base image
  base_lumi = base_lumi * (log10(targetBase)) / (base_lumi.max() - base_lumi.min());
  float curr_max = base_lumi.max();
  base_lumi = base_lumi - curr_max; // Add offset

  // Multiply details
  difference = difference * detailAmp;

  Image new_log_lumi = base_lumi + difference; // Recombine scaled details and base

  Image new_lumi = exp10Image(new_log_lumi); // Convert back to log domain

  vector<Image> new_lumichromi = {new_lumi, chromi}; // Make vector to convert to RGB
  output = lumiChromi2rgb(new_lumichromi);

  return output; // Return output tone-mapped image
}

/*********************************************************************
 *                       Tone mapping helpers                        *
 *********************************************************************/

// image --> log10Image
Image log10Image(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // Taking a linear image im, transform to log10 scale.
  // To avoid infinity issues, make any 0-valued pixel be equal the the
  // minimum non-zero value. See image_minnonzero(im).
  Image input = im + image_minnonzero(im); // Add minnonzero value

  Image output(im.width(), im.height(), im.channels());
  for (int h = 0; h < im.height(); h++) { // Iterate to find overall min
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        output(w, h, c) = log10(input(w, h, c)); // Put pixel in output after log10 scale
      }
    }
  }
  return output; // Return output image in log10 scale
}

// Image --> 10^Image
Image exp10Image(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // take an image in log10 domain and transform it back to linear domain.
  // see pow(a, b)
  Image output(im.width(), im.height(), im.channels());
  for (int h = 0; h < im.height(); h++) { // Iterate to find overall min
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        output(w, h, c) = pow(10, im(w, h, c)); // Put pixel in output after log10 scale
      }
    }
  }
  return output; // Return output image in log10 scale
}

// min non-zero pixel value of image
float image_minnonzero(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // return the smallest value in the image that is non-zeros (across all
  // channels too)
  float min = 1.0f;
  for (int h = 0; h < im.height(); h++) { // Iterate to find overall min
    for (int w = 0; w < im.width(); w++) {
      for (int c = 0; c < im.channels(); c++) {
        if (im(w, h, c) > 0.0f && im(w, h, c) < min) {
          min = im(w, h, c); // If greater than zero and less than current minimum, save
        }
      }
    }
  }
  return min;
  }
