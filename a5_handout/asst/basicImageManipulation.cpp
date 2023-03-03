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

// --------- HANDOUT PS05 ------------------------------
// -----------------------------------------------------
//
Image scaleNN(const Image &im, float factor) {
  // --------- HANDOUT  PS05 ------------------------------
  // create a new image that is factor times bigger than the input by using
  // nearest neighbor interpolation.
  Image output(floor(im.width() * factor), floor(im.height() * factor), im.channels()); // Scale by floor(factor)
  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {
        output(w, h, c) = im.smartAccessor(round(w / factor), round(h / factor), c, true); // Nearest neighbor
      }
    }
  }
  return output; // Return output image
}

float interpolateLin(const Image &im, float x, float y, int z, bool clamp) {
  // --------- HANDOUT  PS05 ------------------------------
  // bilinear interpolation samples the value of a non-integral
  // position (x,y) from its four "on-grid" neighboring pixels.
  //  |           |
  // -1-----------2-
  //  |           |  *: my coordinates (x,y) are not integral
  //  |  *        |     since I am not on the pixel grid :(
  //  |           |  1: top-left
  //  |           |  2: top-right
  //  |           |  3: bottom-right
  // -4-----------3- 4: bottom-left, what are our coordinates?
  //  |           |    We are willing to share some color
  //                   information with * ! Of course, the pixel
  //                   closest to * should influence it more.
  float E = (floor(x + 1) - x) * im.smartAccessor(floor(x), floor(y), z, clamp) \
          + (x - floor(x)) * im.smartAccessor(floor(x + 1), floor(y), z, clamp); // Interpolate X above

  float F = (floor(x + 1) - x) * im.smartAccessor(floor(x), floor(y + 1), z, clamp) \
          + (x - floor(x)) * im.smartAccessor(floor(x + 1), floor(y + 1), z, clamp); // Interpolate X below

  return (floor(y + 1) - y) * E + (y - floor(y)) * F; // Interpolate along Y
}

Image scaleLin(const Image &im, float factor) {
  // --------- HANDOUT  PS05 ------------------------------
  // create a new image that is factor times bigger than the input by using
  // bilinear interpolation
  Image output(floor(im.width() * factor), floor(im.height() * factor), im.channels()); // Scale by floor(factor)
  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {
        output(w, h, c) = interpolateLin(im, w/factor, h/factor, c, true);
      }
    }
  }
  return output;
}

Image scaleBicubic(const Image &im, float factor, float B, float C) {
  // --------- HANDOUT  PS05 ------------------------------
  // create a new image that is factor times bigger than the input by using
  // a bicubic filter kernel with Mitchell and Netravali's parametrization
  // see "Reconstruction filters in computer graphics", Mitchell and Netravali
  // 1988 or http://entropymine.com/imageworsener/bicubic/
  Image output(floor(im.width() * factor), floor(im.height() * factor), im.channels()); // Scale by floor(factor)
  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {

        float sum = 0.0f;
        int adj_x = floor(w/factor), adj_y = floor(h/factor); // Find integer center to iterate over

        for (int x = adj_x - 3; x <= adj_x + 3; x++) { // Iterate over extra cells to make sure all are accounted for
          for (int y = adj_y - 3; y <= adj_y + 3; y++) {

            float x_dist = abs(w/factor - x), y_dist = abs(h/factor - y); // Distance from cell to cell in kernel
            float k_x, k_y;

            // Apply formula in the X direction
            if (x_dist < 1) {
              k_x = (12 - 9*B - 6*C)*pow(x_dist,3) + (-18 + 12*B + 6*C)*pow(x_dist,2) + (6 - 2*B);
            }
            else if (x_dist < 2 && 1 <= x_dist) {
              k_x = (-1*B - 6*C)*pow(x_dist,3) + (6*B + 30*C)*pow(x_dist,2) + (-12*B-48*C)*x_dist + (8*B + 24*C);
            }
            else {
              k_x = 0.0f;
            }

            // Apply formula in the Y direction
            if (y_dist < 1) { 
              k_y = (12 - 9*B - 6*C)*pow(y_dist,3) + (-18 + 12*B + 6*C)*pow(y_dist,2) + (6 - 2*B);
            }
            else if (y_dist < 2 && 1 <= y_dist) {
              k_y = (-1*B - 6*C)*pow(y_dist,3) + (6*B + 30*C)*pow(y_dist,2) + (-12*B-48*C)*y_dist + (8*B + 24*C);
            }
            else {
              k_y = 0.0f;
            }
            sum += (1.0f/36.0f) * k_x * k_y * im.smartAccessor(x, y, c, true); // Multiply weights by pixel value
          }
        }
        output(w, h, c) = sum; // Add summation to pixel 
      }
    }
  }
  return output; // Return output image
}

Image scaleLanczos(const Image &im, float factor, float a) {
  // --------- HANDOUT  PS05 ------------------------------
  // create a new image that is factor times bigger than the input by using
  // a Lanczos filter kernel
  Image output(floor(im.width() * factor), floor(im.height() * factor), im.channels()); // Scale by floor(factor)

  float pi = 3.14159265358979323846;
  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {

        float sum = 0.0f;
        int adj_x = floor(w/factor), adj_y = floor(h/factor); // Find integer center to iterate over
        int adj_a = ceil(a); // Take ceiling of a so we can iterate, use ceiling to guarantee no cells are missed
        for (int x = adj_x - adj_a; x <= adj_x + adj_a; x++) { // Iterate over extra cells to make sure all are accounted for
          for (int y = adj_y - adj_a; y <= adj_y + adj_a; y++) {

            float x_dist = abs(w/factor - x), y_dist = abs(h/factor - y); // Distance from cell to cell in kernel
            float k_x, k_y;

            // Apply formula in the X direction
            if (adj_x - x == 0) {
              k_x = 1.0f;
            }
            else if (x_dist < 1) {
              k_x = (sin(pi * x_dist) / (pi * x_dist)) * (sin(pi * x_dist / a) / (pi * x_dist / a));
            }
            else {
              k_x = 0.0f;
            }

            // Apply formula in the Y direction
            if (adj_y - y == 0) {
              k_y = 1.0f;
            }
            else if (y_dist < 1) { 
              k_y = (sin(pi * y_dist) / (pi * y_dist)) * (sin(pi * y_dist / a) / (pi * y_dist / a));
            }
            else {
              k_y = 0.0f;
            }
            sum += k_x * k_y * im.smartAccessor(x, y, c, true); // Multiply weights by pixel value
          }
        }
        output(w, h, c) = sum; // Add summation to pixel 
      }
    }
  }
  return output; // Return output image
}

Image rotate(const Image &im, float theta) {
  // --------- HANDOUT  PS05 ------------------------------
  // rotate an image around its center by theta
  // center around which to rotate
  float centerX = (im.width() - 1.0) / 2.0f; // Initialize center values as coordinates (x,y)
  float centerY = (im.height() - 1.0) / 2.0f;
  Image output(im.width(), im.height(), im.channels()); // Initialize output of the same size
  output.set_color(); // Set to all black for each channel

  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {

      float magnitude = sqrt(pow(w - centerX, 2) + pow(h - centerY, 2)); // Distance from current pixel to center

      float theta_new = acos((w - centerX) / magnitude); // Original angle of current point

      float theta_0 = theta_new - theta; // Make new angle for new point

      float x_0 = magnitude * cos(theta_0) + centerX, y_0 = magnitude * sin(theta_0) + centerY; // Coordinates from original image

      for (int c = 0; c < im.channels(); c++) {
        output(w, h, c) = interpolateLin(im, x_0, y_0, c, false);
      }
    }
  }
  return output; // Return output image
}

// -----------------------------------------------------
// --------- END --- PS05 ------------------------------

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

Image lumiChromi2rgb(const vector<Image> &lc) {
  // luminance is lc[0]
  // chrominance is lc[1]

  // Create chrominance images
  // We copy the input as starting point for the chrominance
  Image im = Image(lc[1].width(), lc[1].height(), lc[1].channels());
  for (int c = 0; c < im.channels(); c++) {
    for (int y = 0; y < im.height(); y++) {
      for (int x = 0; x < im.width(); x++) {
        im(x, y, c) = lc[1](x, y, c) * lc[0](x, y);
      }
    }
  }
  return im;
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
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      for (int k = 0; k < im.channels(); k++) {
        output(i, j, k) = roundf(im(i, j, k) * (pow(2.f, (float)bits) - 1)) /
                          (pow(2.f, (float)bits) - 1.f);
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
