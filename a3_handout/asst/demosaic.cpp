/* --------------------------------------------------------------------------
 * File:    demosaic.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 * ------------------------------------------------------------------------*/

#include "demosaic.h"
#include <cmath>

using namespace std;

Image basicGreen(const Image &raw, int offset) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns a single-channel
  // 2D image corresponding to the green channel using simple interpolation
  Image output(raw.width(), raw.height(), 1); // Initialize 1 channel output image

  for (int h = 0; h < output.height(); h++) {
    output(0, h, 0) = raw(0, h, 0);                                   // Copy Left edge column
    output(output.width() - 1, h, 0) = raw(output.width() - 1, h, 0); // Copy Right edge column
  }
  for (int w = 0; w < output.width(); w++) {
    output(w, 0, 0) = raw(w, 0, 0);                                     // Copy Top edge row
    output(w, output.height() - 1, 0) = raw(w, output.height() - 1, 0); // Copy Bottom edge row
  }

  if (offset = 0) {
    for (int h = 1; h < output.height() - 1; h++) {  // Iterate over pixels in row-major order
      for (int w = 1; w < output.width() - 1; w++) {
        if (w % 2 == h % 2) {
          output(w, h, 0) = raw(w, h, 0); // Top left corner is green
        }
        else {
          output(w, h, 0) = (raw(w, h + 1, 0) + raw(w, h - 1, 0) + raw(w - 1, h, 0) + raw(w + 1, h, 0)) / 4.0f;
        }
      }
    }
  }
  else if (offset = 1) {
    for (int h = 1; h < output.height() - 1; h++) {  // Iterate over pixels in row-major order
      for (int w = 1; w < output.width() - 1; w++) {
        if (w % 2 == h % 2) { // One to right of top left corner
          output(w, h, 0) = (raw(w, h + 1, 0) + raw(w, h - 1, 0) + raw(w - 1, h, 0) + raw(w + 1, h, 0)) / 4.0f;
        }
        else {
          output(w, h, 0) = raw(w, h, 0);
        }
      }
    }
  }
  return output; // Return output image
}

Image basicRorB(const Image &raw, int offsetX, int offsetY) {
  // --------- HANDOUT  PS03 ------------------------------
  //  Takes as input a raw image and returns a single-channel
  //  2D image corresponding to the red or blue channel using simple interpolation
  Image output(raw.width(), raw.height(), 1); // Initialize 1 channel output image

  for (int h = 0; h < output.height(); h++) {
    output(0, h, 0) = raw(0, h, 0);                                   // Copy Left edge column
    output(output.width() - 1, h, 0) = raw(output.width() - 1, h, 0); // Copy Right edge column
  }
  for (int w = 0; w < output.width(); w++) {
    output(w, 0, 0) = raw(w, 0, 0);                                     // Copy Top edge row
    output(w, output.height() - 1, 0) = raw(w, output.height() - 1, 0); // Copy Bottom edge row
  }

  for (int h = 1; h < output.height() - 1; h++) {  // Iterate over pixels in row-major order
    for (int w = 1; w < output.width() - 1; w++) {
      if (w % 2 == offsetX && h % 2 == offsetY) {  // Case 1: B or R is present at that pixel
        output(w, h, 0) = raw(w, h, 0);
      }
      else if (w % 2 != offsetX && h % 2 == offsetY) { // Case 2: X not aligned, take left/right neighbors
        output(w, h, 0) = (raw(w - 1, h, 0) + raw(w + 1, h, 0)) / 2.0f;
      }
      else if (w % 2 == offsetX && h % 2 != offsetY) { // Case 3: Y not aligned, take up/down neighbors
        output(w, h, 0) = (raw(w, h - 1, 0) + raw(w, h + 1, 0)) / 2.0f;
      }
      else {                                         // Case 4: Neither aligned, take average of corners
        output(w, h, 0) = (raw(w - 1, h - 1, 0) + raw(w - 1, h + 1, 0) + raw(w + 1, h - 1, 0) + raw(w + 1,h + 1, 0)) / 4.0f; 
      }
    }
  }
  return output;
}

Image basicDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                    int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // --------- HANDOUT  PS03 ------------------------------
  // takes as input a raw image and returns an rgb image
  // using simple interpolation to demosaic each of the channels
  Image green = basicGreen(raw, offsetGreen);
  Image blue  = basicRorB(raw, offsetBlueX, offsetBlueY); // Demosaic R, G, and B individually
  Image red   = basicRorB(raw, offsetRedX, offsetRedY);
  Image output(raw.width(), raw.height(), 3);
  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      output(w, h, 0) = red(w, h, 0);   // Red channel
      output(w, h, 1) = green(w, h, 0); // Green channel
      output(w, h, 2) = blue(w, h, 0);  // Blue channel
    }
  }
  return output; // Return final output image
}

Image edgeBasedGreen(const Image &raw, int offset) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes a raw image and outputs a single-channel
  // image corresponding to the green channel taking into account edges
  Image output(raw.width(), raw.height(), 1); // Initialize 1 channel output image

  for (int h = 0; h < output.height(); h++) { // Loops to keep image borders the same as input
    output(0, h, 0) = raw(0, h, 0);                                   // Copy Left edge column
    output(output.width() - 1, h, 0) = raw(output.width() - 1, h, 0); // Copy Right edge column
  }
  for (int w = 0; w < output.width(); w++) {
    output(w, 0, 0) = raw(w, 0, 0);                                     // Copy Top edge row
    output(w, output.height() - 1, 0) = raw(w, output.height() - 1, 0); // Copy Bottom edge row
  }

  if (offset = 0) {
    for (int h = 1; h < output.height() - 1; h++) {  // Iterate over pixels in row-major order
      for (int w = 1; w < output.width() - 1; w++) {
        if (w % 2 == h % 2) { // If offset is zero, want coordinates to have equal parity
          output(w, h, 0) = raw(w, h, 0); // Top left corner is green
        }
        else {
          if (abs(raw(w - 1, h, 0) - raw(w + 1, h, 0)) < abs(raw(w, h + 1, 0) - raw(w, h - 1, 0))) {
            output(w, h, 0) = (raw(w + 1, h, 0) + raw(w - 1, h, 0)) / 2.0f; // Horizontal neighbors better
          }
          else {
            output(w, h, 0) = (raw(w, h + 1, 0) + raw(w, h - 1, 0)) / 2.0f; // Vertical neighbors better
          }
        }
      }
    }
  }
  else if (offset = 1) {
    for (int h = 1; h < output.height() - 1; h++) {  // Iterate over pixels in row-major order
      for (int w = 1; w < output.width() - 1; w++) {
        if (w % 2 != h % 2) { // If offset is zero, want coordinates with unequal parity
          output(w, h, 0) = raw(w, h, 0); // One to right of top left corner is green
        }
        else {
          if (abs(raw(w - 1, h, 0) - raw(w + 1, h, 0)) < abs(raw(w, h + 1, 0) - raw(w, h - 1, 0))) {
            output(w, h, 0) = (raw(w + 1, h, 0) + raw(w - 1, h, 0)) / 2.0f; // Horizontal neighbors better
          }
          else {
            output(w, h, 0) = (raw(w, h + 1, 0) + raw(w, h - 1, 0)) / 2.0f;  // Vertical neighbors better
          }
        }
      }
    }
  }
  return output; 
}

Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                             int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // --------- HANDOUT  PS03 ------------------------------
  Image green = edgeBasedGreen(raw, offsetGreen); // Used edge-based green demosaicing
  Image blue  = basicRorB(raw, offsetBlueX, offsetBlueY); // Demosaic R, G, and B individually
  Image red   = basicRorB(raw, offsetRedX, offsetRedY);
  Image output(raw.width(), raw.height(), 3);
  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      output(w, h, 0) = red(w, h, 0);   // Red channel
      output(w, h, 1) = green(w, h, 0); // Green channel
      output(w, h, 2) = blue(w, h, 0);  // Blue channel
    }
  }
  return output; 
}

Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY) {
  // --------- HANDOUT  PS03 ------------------------------
  // Perform interpolation on (Raw (R or B) - Green), then add back Green 
  Image green_3D(raw.width(), raw.height(), 3);
  for (int h = 0; h < green.height(); h++) {
    for (int w = 0; w < green.width(); w++) {
      for (int c = 0; c < 3; c++) {
        green_3D(w, h, c) = green(w, h, 0); // Construct three channel version of green
      }                                     // for subtraction from 3D raw input image
    }
  }
  return basicRorB(raw - green_3D, offsetX, offsetY) + green; // Perform basicRorB on difference between (R & G) or (B & G)
}

Image improvedDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                       int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns an rgb image
  // using edge-based green demosaicing for the green channel and
  // simple green based demosaicing of the red and blue channels
  Image green = edgeBasedGreen(raw, offsetGreen); // Used edge-based green demosaicing
  Image blue  = greenBasedRorB(raw, green, offsetBlueX, offsetBlueY); // Demosaic R, G, and B individually
  Image red   = greenBasedRorB(raw, green, offsetRedX, offsetRedY);
  Image output(raw.width(), raw.height(), 3);
  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      output(w, h, 0) = red(w, h, 0);   // Red channel
      output(w, h, 1) = green(w, h, 0); // Green channel
      output(w, h, 2) = blue(w, h, 0);  // Blue channel
    }
  }
  return output; 
}
