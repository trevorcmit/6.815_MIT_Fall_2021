/* -----------------------------------------------------------------
 * File:    a0.cpp
 * Created: 2015-09-09
 * Modified: 2017-09-05
 * -----------------------------------------------------------------
 *
 * Implementation file for PS00
 *
 * We define and implement the functions we declared in a0.h
 *
 * ---------------------------------------------------------------*/

// This command imports all the content of a0.h, thus declaring our functions.
#include "a0.h"

// We use the namespace "std" so that we can write the command "cout" instead
// of the verbose version "std::cout".
using namespace std;

// Return the sum of two numbers
float getSum(float a, float b) {
  // --------- HANDOUT  PS00 ------------------------------
  // Create a variable c that is the sum of a and b
  // Return it
  float c = a + b;
  return c;
  // return 0.0f;
}

// Print the sum of two numbers to the screen
void helloworld(float a, float b) {
  // --------- HANDOUT  PS00 ------------------------------
  // Create a variable c that is the sum of a and b
  // Use cout to display the values
  cout << "Hello World!" << endl;
  cout << "The value of a is " << a << "." << endl;
  cout << "The value of b is " << b << "." << endl;
  cout << "The sum of a and b is " << getSum(a,b) << "." << endl;
}

// Create an image and return it
// You can test your function by calling my_image.write(filename) to write the
// output somewhere
Image readAnImage(const std::string &filename) {
  // --------- HANDOUT  PS00 ------------------------------
  // Use the constructor in Image.h
  return Image(filename); // Change this
}

// Create an empty image of size width x height
SimpleImage::SimpleImage(int width, int height) {
    // --------- HANDOUT  PS00 ------------------------------
    // Make sure keep around all necessary data
    // Initialize the image to empty (0.0f)
    float w = width, h = height;
    vector<float> pixels;
    pixels.push_back(w);
    pixels.push_back(h); // Keep height and width stored at end of array.
    for (int n = 0; n < width * height; n++) {
      pixels.push_back(0.0f); // Fill rest of vector with empty data
    }
    // return pixels;
}

// Computes the average pixel value of the image
float SimpleImage::compute_average() const {
  // --------- HANDOUT  PS00 ------------------------------
  // int w = width();
  return 0.0f; // Change this
}

// Create a white (1.0f) rectangle in the center of the image, taking up
// 10% of the image in each direction
void SimpleImage::make_rectangle() {
  // --------- HANDOUT  PS00 ------------------------------
  // Denote white as 1.0f
  // Make sure rectangle is centered in the image and includes the center
}

// Print the image to the terminal.
// denote black (0.0f) as '-' and white (1.0) as '+'
void SimpleImage::print_to_terminal() const {
  // --------- HANDOUT  PS00 ------------------------------
  // print the image as ASCII characters
}
