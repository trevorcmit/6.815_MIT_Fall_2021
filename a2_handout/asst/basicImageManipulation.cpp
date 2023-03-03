/* -----------------------------------------------------------------
 * File:    basicImageManiuplation.cpp
 * Created: 2015-09-15
 * Updated: 2019-08-10
 * -----------------------------------------------------------------
 *
 * Basic Image Manipulation Utilities
 *
 * ---------------------------------------------------------------*/

#include "basicImageManipulation.h"
#include <cmath>
using namespace std;

// Create a surprise image
Image create_special() {
  // // --------- HANDOUT  PS01 ------------------------------
  // create the image outlined in the handout
  // return Image(1,1,1); // change this

  // --------- SOLUTION PS01 ------------------------------
  Image im(290, 150, 3);
  im.set_color(1.0f, 1.0f, 1.0f);
  int xstart[] = {0, 52, 103, 155, 155, 207, 207};
  int ystart[] = {0, 0, 0, 0, 48, 0, 48};
  int xend[] = {31, 83, 134, 186, 186, 289, 238};
  int yend[] = {149, 102, 149, 30, 149, 30, 149};
  for (int i = 0; i < 7; ++i) {
    if (i != 4)
      im.create_rectangle(xstart[i], ystart[i], xend[i], yend[i], 0.64f, 0.12f,
                          0.2f);
    else
      im.create_rectangle(xstart[i], ystart[i], xend[i], yend[i], 0.55f, 0.55f,
                          0.55f);
  }
  return im;
}

// Change the brightness of the image
// const Image & means a reference to im will get passed to the function,
// but the compiler won't let you modify it within the function.
// So you will return a new image
Image brightness(const Image &im, float factor) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Modify image brightness
  // return output;
  // return Image(1,1,1); // Change this

  // --------- SOLUTION PS01 ------------------------------
  return im * factor;
}

Image contrast(const Image &im, float factor, float midpoint) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Modify image contrast
  // return output;
  // return Image(1,1,1); //Change this

  // --------- SOLUTION PS01 ------------------------------
  return (im - midpoint) * factor + midpoint;
}

Image color2gray(const Image &im, const std::vector<float> &weights) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), 1);
  // Convert to grayscale
  // return Image(1,1,1); //Change this

  // --------- SOLUTION PS01 ------------------------------
  Image output(im.width(), im.height(), 1);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      float sum = 0;
      float weighted_sum = 0;
      for (int k = 0; k < im.channels(); k++) {
        weighted_sum += im(i, j, k) * weights[k];
        sum += weights[k];
      }
      output(i, j, 0) = weighted_sum / sum;
    }
  }
  return output;
}

// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with
// luminance first
std::vector<Image> lumiChromi(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create the luminance image
  // Create the chrominance image
  // Create the output vector as (luminance, chrominance)
  // return std::vector<Image>(); //Change this

  // --------- SOLUTION PS01 ------------------------------

  // Create the luminance
  Image im_luminance = color2gray(im);

  // Create chrominance images
  // We copy the input as starting point for the chrominance
  Image im_chrominance = im;
  for (int c = 0; c < im.channels(); c++)
    for (int y = 0; y < im.height(); y++)
      for (int x = 0; x < im.width(); x++) {
        im_chrominance(x, y, c) = im_chrominance(x, y, c) / im_luminance(x, y);
      }

  // Stack luminance and chrominance in the output vector, luminance first
  std::vector<Image> output;
  output.push_back(im_luminance);
  output.push_back(im_chrominance);
  return output;
}

// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, float brightF, float contrastF,
                             float midpoint) {
  // --------- HANDOUT  PS01 ------------------------------
  // Modify brightness, then contrast of luminance image
  // return Image(1,1,1);

  // --------- SOLUTION PS01 ------------------------------
  // Separate luminance and chrominance
  std::vector<Image> lumi_chromi = lumiChromi(im);
  Image im_luminance = lumi_chromi[0];
  Image im_chrominance = lumi_chromi[1];

  // Process the luminance channel
  im_luminance = brightness(im_luminance, brightF);
  im_luminance = contrast(im_luminance, contrastF, midpoint);

  // Multiply the chrominance with the new luminance to get the final image
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      for (int c = 0; c < im.channels(); c++) {
        im_chrominance(i, j, c) = im_chrominance(i, j, c) * im_luminance(i, j);
      }
    }
  }
  // At this point, im_chrominance holds the complete processed image
  return im_chrominance;
}

Image rgb2yuv(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate size
  // Change colorspace
  // return Image(1,1,1);

  // --------- SOLUTION PS01 ------------------------------
  Image output(im.width(), im.height(), im.channels());
  for (int j = 0; j < im.height(); j++)
    for (int i = 0; i < im.width(); i++) {
      output(i, j, 0) =
          0.299 * im(i, j, 0) + 0.587 * im(i, j, 1) + 0.114 * im(i, j, 2);
      output(i, j, 1) =
          -0.147 * im(i, j, 0) - 0.289 * im(i, j, 1) + 0.436 * im(i, j, 2);
      output(i, j, 2) =
          0.615 * im(i, j, 0) - 0.515 * im(i, j, 1) - 0.100 * im(i, j, 2);
    }
  return output;
}

Image yuv2rgb(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate size
  // Change colorspace
  // return Image(1,1,1);

  // --------- SOLUTION PS01 ------------------------------
  Image output(im.width(), im.height(), im.channels());
  for (int j = 0; j < im.height(); j++)
    for (int i = 0; i < im.width(); i++) {
      output(i, j, 0) = im(i, j, 0) + 0 * im(i, j, 1) + 1.14 * im(i, j, 2);
      output(i, j, 1) = im(i, j, 0) - 0.395 * im(i, j, 1) - 0.581 * im(i, j, 2);
      output(i, j, 2) = im(i, j, 0) + 2.032 * im(i, j, 1) + 0 * im(i, j, 2);
    }
  return output;
}

Image saturate(const Image &im, float factor) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate size
  // Saturate image
  // return output;
  // return Image(1,1,1);

  // --------- SOLUTION PS01 ------------------------------
  Image output = rgb2yuv(im); // Change colorspace
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      output(i, j, 1) = output(i, j, 1) * factor;
      output(i, j, 2) = output(i, j, 2) * factor;
    }
  }
  output = yuv2rgb(output); // Back to RGB
  return output;
}

// Gamma codes the image
Image gamma_code(const Image &im, float gamma) {
  // // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Gamma encodes the image
  // return output;

  // --------- SOLUTION PS01 ------------------------------
  Image output = Image(im.width(), im.height(), im.channels());
  for (int i = 0; i < im.number_of_elements(); ++i) {
    output(i) = pow(im(i), (1 / gamma));
  }
  return output;
}

// Quantizes the image to 2^bits levels and scales back to 0~1
Image quantize(const Image &im, int bits) {
  // // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Quantizes the image to 2^bits levels
  // return output;

  // --------- SOLUTION PS01 ------------------------------
  Image output(im.width(), im.height(), im.channels());
  // Quantizes the image to 2^bits levels
  int factor =  pow(2,bits); 
  for (int x=0; x<im.width(); x++){
   for (int y=0; y<im.height(); y++){
      for (int c=0; c<im.channels(); c++) {
        output(x,y,c) = floor(factor*im(x,y,c))/(factor-(1-floor(im(x,y,c))));
      }
    }
  }
  return output;
}

// Compare between first quantize then gamma_encode and first gamma_encode
// then quantize
std::vector<Image> gamma_test(const Image &im, int bits, float gamma) {
  // // --------- HANDOUT  PS01 ------------------------------
  // // im1 = quantize then gamma_encode the image
  // // im2 = gamma_encode then quantize the image
  // // Remember to create the output images and the output vector
  // // Push the images onto the vector
  // // Do all the required processing
  // // Return the vector, color image first
  // return std::vector<Image>(); //Change this

  // --------- SOLUTION PS01 ------------------------------
  Image im1 = quantize(im, bits);
  im1 = gamma_code(im1, gamma);

  Image im2 = gamma_code(im, gamma);
  im2 = quantize(im2, bits);

  std::vector<Image> imgs;
  imgs.push_back(im1);
  imgs.push_back(im2);
  return imgs;
}

// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Remember to create the output images and the output vector
  // Push the images onto the vector
  // Do all the required processing
  // Return the vector, color image first
  // return std::vector<Image>(); //Change this

  // --------- SOLUTION PS01 ------------------------------
  // Extract the luminance
  Image output_L = color2gray(im);

  // Convert to YUV for manipulation
  Image output_C = rgb2yuv(im);

  for (int j = 0; j < im.height(); j++)
    for (int i = 0; i < im.width(); i++) {
      output_C(i, j, 0) = 0.5;                // constant luminance
      output_C(i, j, 1) = -output_C(i, j, 1); // opposite chrominance
      output_C(i, j, 2) = -output_C(i, j, 2); // opposite chrominance
    }
  // Convert back to RGB
  output_C = yuv2rgb(output_C);

  // Location of the black dot
  int bdot_x = floor(im.width() / 2);
  int bdot_y = floor(im.height() / 2);

  // Add the black dot to Luminance, and Chrominance images
  output_L(bdot_x, bdot_y, 0) = 0.0f;
  output_C(bdot_x, bdot_y, 0) = 0.0f; // black is 0
  output_C(bdot_x, bdot_y, 1) = 0.0f;
  output_C(bdot_x, bdot_y, 2) = 0.0f;

  // Pack the images in a vector, chrominance first
  std::vector<Image> output;
  output.push_back(output_C);
  output.push_back(output_L);
  return output;
}

// White balances an image using the gray world assumption
Image grayworld(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Implement automatic white balance by multiplying each channel
  // of the input by a factor such that the three channel of the output
  // image have the same mean value. The mean value of the green channel
  // is taken as reference.
  // return Image(1,1,1);

  // --------- SOLUTION PS01 ------------------------------
  // Compute the mean per channel
  // find green mean
  float green_sum = 0;
  Image output(im.width(), im.height(), im.channels());
  for (int y = 0; y < im.height(); y++) {
    for (int x = 0; x < im.width(); x++) {
      green_sum += im(x, y, 1);
    }
  }
  float green_mean = green_sum / (im.width() * im.height());

  // red
  float red_sum = 0;
  for (int y = 0; y < im.height(); y++) {
    for (int x = 0; x < im.width(); x++) {
      red_sum += im(x, y, 0);
    }
  }
  float red_mean = red_sum / (im.width() * im.height());

  // blue
  float blue_sum = 0;
  for (int y = 0; y < im.height(); y++) {
    for (int x = 0; x < im.width(); x++) {
      blue_sum += im(x, y, 2);
    }
  }
  float blue_mean = blue_sum / (im.width() * im.height());

  float red_factor = green_mean / red_mean;
  float blue_factor = green_mean / blue_mean;

  // normalize 'white balance' red & blue
  for (int y = 0; y < im.height(); y++) {
    for (int x = 0; x < im.width(); x++) {
      output(x, y, 0) = im(x, y, 0) * (red_factor);
      output(x, y, 1) = im(x, y, 1);
      output(x, y, 2) = im(x, y, 2) * (blue_factor);
    }
  }

  return output;
}
