/* --------------------------------------------------------------------------
 * File:    align.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "align.h"

using namespace std;

Image denoiseSeq(const vector<Image> &imSeq) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Basic denoising by computing the average of a sequence of images
  Image output(imSeq.at(0).width(), imSeq.at(0).height(), imSeq.at(0).channels()); // Initialize output image
  for (int h = 0; h < output.height(); h++) {
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) { // Iterate over pixels in row-major order
        for (int n = 0; n < imSeq.size(); n++) {
          output(w, h, c) = output(w, h, c) + (imSeq.at(n)(w, h, c) / imSeq.size()); // Add all values at that pixel
        }
      }
    }
  }
  return output; // Return output image
}

Image logSNR(const vector<Image> &imSeq, float scale) {
  // // --------- HANDOUT  PS03 ------------------------------
  // returns an image visualizing the per-pixel and
  // per-channel log of the signal-to-noise ratio scaled by scale.
  Image avg = denoiseSeq(imSeq); // Get the Expected Value (E[i]) of each pixel
  Image output(avg.width(), avg.height(), avg.channels()); // Initialize output image

  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) {

        float sigma_sqr = 0.0f;                   // Initialize value for sigma squared (variance)
        for (int n = 0; n < imSeq.size(); n++) {
          sigma_sqr += pow(imSeq.at(n)(w, h, c) - avg(w, h, c), 2); // square difference between pixel and the avg
        }
        sigma_sqr /= (imSeq.size() - 1 + 0.000001); // Divide by n-1, sigma_sqr is now equal to variance

        output(w, h, c) = scale * 10.0f * log10(avg(w, h, c) * avg(w, h, c) / (sigma_sqr + 0.000001)); // Add log SNR to image
      }
    }
  }
  return output; // Return output image
}

vector<int> align(const Image &im1, const Image &im2, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // returns the (x,y) offset that best aligns im2 to match im1.
  vector<int> output;

  float best_diff = im1.height() * im1.width() * im1.channels() * 2.0f; // Initialize best sum of squared pixel difference counter
  int best_offset_x = 0, best_offset_y = 0; // Initialize best offset counters

  for (int y = -1 * maxOffset; y <= maxOffset; y++) { // Brute force over all offset values
    for (int x = -1 * maxOffset; x <= maxOffset; x++) {
      
      float difference = 0.0f; // Initialize variable to hold sum of squared pixel differences
      for (int h = maxOffset; h < im1.height() + maxOffset; h++) {  // Iterate over pixels in row-major order
        for (int w = maxOffset; w < im1.width() + maxOffset; w++) {
          for (int c = 0; c < im1.channels(); c++) {
            difference += pow(im1.smartAccessor(w, h, c) - im2.smartAccessor(w + x, h + y, c), 2); // squared pixel difference
          }
        }
      }
      if (difference < best_diff) {
        best_diff = difference;     // Keep track of the best option so far
        best_offset_x = x;
        best_offset_y = y; 
      }
    }
  }
  output.push_back(best_offset_x);
  output.push_back(best_offset_y);
  return output; // Return vector of the best x and y translations
}

Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Registers all images to the first one in a sequence and outputs
  // a denoised image even when the input sequence is not perfectly aligned.
  Image output(imSeq.at(0).width(), imSeq.at(0).height(), imSeq.at(0).channels());
  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      for (int c = 0; c < output.channels(); c++) { // Making new object and iterating guarantees hard copy is made
        output(w, h, c) = imSeq.at(0)(w, h, c) / imSeq.size();
      }
    }
  }
  cout << "Image #1 loaded, beginning aligning and denoising..." << endl;

  for (int n = 1; n < imSeq.size(); n++) {
    vector<int> offset = align(imSeq.at(0), imSeq.at(n), maxOffset); // Use align as helper function to get x and y offsets
    for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
      for (int w = 0; w < output.width(); w++) {
        for (int c = 0; c < output.channels(); c++) { 
          output(w, h, c) = output(w, h, c) + (
            imSeq.at(n).smartAccessor(w + offset.at(0), h + offset.at(1), c) / imSeq.size()
          );
        } // We divide by imSeq.size() so we don't need to average at the end, just doing it while iterating
      }
    }
    cout << "Aligned Image #" << n + 1 << " with [" << offset.at(0) << "," << offset.at(1) << "]..." << endl;
  }
  cout << "Finished!" << endl;
  return output;
}

Image split(const Image &sergeyImg) {
  // --------- HANDOUT  PS03 ------------------------------
  // 6.865 only:
  Image output(sergeyImg.width(), floor(sergeyImg.height() / 3), 3); // Floor 1/3 of input image for height

  for (int h = 0; h < output.height(); h++) {   // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
      output(w, h, 2) = sergeyImg(w, h);        // Blue is on top
      output(w, h, 1) = sergeyImg(w, h + floor(sergeyImg.height() / 3));    // Green in middle
      output(w, h, 0) = sergeyImg(w, h + 2 * floor(sergeyImg.height() / 3));    // Red on bottom
    }
  }
  return output; // Return output image with floored height value
}

Image sergeyRGB(const Image &sergeyImg, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  Image sergey_RGB = split(sergeyImg);                      // Perform split of input image 
  Image red(sergey_RGB.width(), sergey_RGB.height(), 1);    // Initialize one channel R, G and B images
  Image green(sergey_RGB.width(), sergey_RGB.height(), 1);
  Image blue(sergey_RGB.width(), sergey_RGB.height(), 1);
  Image output(sergey_RGB.width(), sergey_RGB.height(), 3); // Initialize RGB output

  for (int h = 0; h < sergey_RGB.height(); h++) {   // Iterate over pixels in row-major order
    for (int w = 0; w < sergey_RGB.width(); w++) {
      red(w, h, 0)   = sergey_RGB(w, h, 0);
      green(w, h, 0) = sergey_RGB(w, h, 1); // Put split output into single channel R, G, and B images
      blue(w, h, 0)  = sergey_RGB(w, h, 2);
    }
  }
  vector<int> G_to_R = align(red, green, maxOffset), B_to_R = align(red, blue, maxOffset); // Align G and B to R channel

  for (int h = 0; h < output.height(); h++) {  // Iterate over pixels in row-major order
    for (int w = 0; w < output.width(); w++) {
        output(w, h, 0) = red.smartAccessor(w, h, 0); // Apply offsets found by align function
        output(w, h, 1) = green.smartAccessor(w + G_to_R.at(0), h + G_to_R.at(1), 0);
        output(w, h, 2) = blue.smartAccessor(w + B_to_R.at(0), h + B_to_R.at(1), 0);
    }
  }
  return output; // Return output image
}

vector<int> align_faster(const Image &im1, const Image &im2, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // returns the (x,y) offset that best aligns im2 to match im1.
  vector<int> output;
  float best_diff = im1.height() * im1.width() * im1.channels() * 2.0f; // Initialize best sum of squared pixel difference counter
  int best_offset_x = 0, best_offset_y = 0; // Initialize best offset counters

  for (int x = -1 * maxOffset; x <= maxOffset; x++) {
    int current_offset_y = -1 * maxOffset; // Set current offset in the Y direction
    Image current(im1.width(), im1.height(), im1.channels());
    for (int h = 0; h < current.height(); h++) {
      for (int w = 0; w < current.width(); w++) { // Fill current image with the starting offset value
        for (int c = 0; c < current.channels(); c++) {
          current(w, h, c) = im2.smartAccessor(w + x, h + current_offset_y, c);
        }
      }
    }

    while (current_offset_y <= maxOffset) { // While loop to iterate over values of Y offset

      float difference = 0.0f; // Initialize sum to determine success of offset
      for (int h = 0; h < current.height(); h++) {
        for (int w = 0; w < current.width(); w++) { // Iterate over current offset to get differences for summation
          for (int c = 0; c < current.channels(); c++) {
            difference += pow(im1.smartAccessor(w, h, c) - current.smartAccessor(w, h, c), 2); // Get difference
          }
        }
      }
      for (int w = 0; w < current.width(); w++) {       // Iterate over width and channels to remove vector values
        for (int c = 0; c < current.channels(); c++) {  // and add new ones. 
          current.image_data.erase(current.image_data.begin()); // Remove first row
          current.image_data.push_back(im2.smartAccessor(w + x, current.height() + current_offset_y, c)); // Add new bottom row
        }
      }
      if (difference < best_diff) { // Update best difference if it is better
        best_diff = difference;  
        best_offset_x = x;
        best_offset_y = current_offset_y; 
      } 
      current_offset_y += 1; // Iterate current y offset for the while loop
    } 
  }

  output.push_back(best_offset_x);
  output.push_back(best_offset_y);
  return output; // Return vector of the best x and y translations
}

/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
Image roll(const Image &im, int xRoll, int yRoll) {

  int xNew, yNew;
  Image imRoll(im.width(), im.height(), im.channels());

  // for each pixel in the original image find where its corresponding
  // location is in the rolled image
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {

      // use modulo to figure out where the new location is in the
      // rolled image. Then take care of when this returns a negative
      // number
      xNew = (x + xRoll) % im.width();
      yNew = (y + yRoll) % im.height();
      xNew = (xNew < 0) * (imRoll.width() + xNew) + (xNew >= 0) * xNew;
      yNew = (yNew < 0) * (imRoll.height() + yNew) + (yNew >= 0) * yNew;

      // assign the rgb values for each pixel in the original image to
      // the location in the new image
      for (int z = 0; z < im.channels(); z++) {
        imRoll(xNew, yNew, z) = im(x, y, z);
      }
    }
  }

  // return the rolled image
  return imRoll;
}
