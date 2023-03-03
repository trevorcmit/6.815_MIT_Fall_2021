#include "homography.h"
#include "matrix.h"
#include <cassert>
#include <iostream>

using namespace std;

void testStitchScience() {
  // // --------- HANDOUT  PS06 ------------------------------
  // 6.865 write a test
  // notice these are flip to align on the right-hand side
  Image im1("Input/science-1.png");
  Image im2("Input/science-2.png");
  CorrespondencePair corresp[4] = {
    CorrespondencePair( 44,184.125,1, 232,178.125,1),
    CorrespondencePair( 35,301.125,1, 233,293.125,1),
    CorrespondencePair( 104,336.125,1, 307,325.125,1),
    CorrespondencePair( 105,205.125,1, 296,190.125,1)
  };
  Image st = stitch(im1, im2, corresp);
  st.write("./Output/science-stitch.png");
}

void testStitchConvention() {
  // // --------- HANDOUT  PS06 ------------------------------
  // 6.865 write a test
  Image im1("Input/convention-1.png");
  Image im2("Input/convention-2.png");
  CorrespondencePair corresp[4] = {
    CorrespondencePair( 190,303.125,1, 52,317.125,1),
    CorrespondencePair( 305,291.125,1, 164,294.125,1),
    CorrespondencePair( 322,207.125,1, 174,215.125,1),
    CorrespondencePair( 186,208.125,1, 43,218.125,1)
  };
  Image st = stitch(im1, im2, corresp);
  st.write("./Output/convention-stitch.png");
}

void testStitchBoston1() {
  // // --------- HANDOUT  PS06 ------------------------------
  // 6.865 write a test
  Image im1("Input/boston1-1.png");
  Image im2("Input/boston1-2.png");
  CorrespondencePair corresp[4] = {
    CorrespondencePair( 185,301.125,1, 26,308.125,1),
    CorrespondencePair( 292,270.125,1, 136,270.125,1),
    CorrespondencePair( 269,158.125,1, 113,162.125,1),
    CorrespondencePair( 214,87.125,1, 59,83.125,1)
  };
  Image st = stitch(im1, im2, corresp);
  st.write("./Output/boston1-stitch.png");
}
