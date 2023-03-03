/* -----------------------------------------------------------------
 * File:    a0.h
 * Created: 2015-08-29
 * -----------------------------------------------------------------
 *
 * Assignment 00
 *
 * ---------------------------------------------------------------*/

#ifndef __A0__H
#define __A0__H

#include "Image.h"
#include "messageassert.h"
#include <iostream>
#include <vector>

using namespace std;

// Return the sum of two numbers
float getSum(float a, float b);

// Print the sum of two numbers
void helloworld(float a, float b);

// Create an image and return it
Image readAnImage(const std::string &filename);

// Simple image class
class SimpleImage {
public:
  // Create an empty image of size width x height
  SimpleImage(int width, int height);
  ~SimpleImage(){};

  // Create a white (1.0f) rectangle in the center of the image, taking up
  // 10% of the image in each direction
  void make_rectangle();

  // Computes the average of all pixel in the image
  float compute_average() const;

  // Prints the image to the terminal
  // denote black (0.0f) as '-' and white (1.0) as '+'
  void print_to_terminal() const;

  // Private field accessors used by out grading scripts
  std::vector<float> &img() { return img_; }
  unsigned int width() { return width_; }
  unsigned int height() { return height_; }

private:
  std::vector<float>
      img_; // stores our image as a flat array of size width_ * height_
  unsigned int width_;  // stores the image width
  unsigned int height_; // stores the image height
};

#endif /* end of include guard: __A0__H */
