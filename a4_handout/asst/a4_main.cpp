/* --------------------------------------------------------------------------
 * File:    a4_main.cpp
 * Created: 2015-10-06
 * --------------------------------------------------------------------------
 * ------------------------------------------------------------------------*/

#include "Image.h"
#include "basicImageManipulation.h"
#include "hdr.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void testComputeWeight() {
  // load in image and invert gamma correction
  Image im1("./Input/ante1-1.png");
  im1 = gamma_code(im1, 1.0 / 2.2);

  // compute the weight image and save it
  Image weight = computeWeight(im1, 0.002, 0.99);
  weight.write("./Output/weight.png");
}

void testComputeFactor() {
  // load 2 images
  Image im1("./Input/ante2-1.png");
  Image im2("./Input/ante2-2.png");

  // invert gamma correction
  im1 = gamma_code(im1, 1.0 / 2.2);
  im2 = gamma_code(im2, 1.0 / 2.2);

  // compute weight images and save them
  Image w1 = computeWeight(im1);
  Image w2 = computeWeight(im2);
  w1.write("./Output/ante2-w1.png");
  w2.write("./Output/ante2-w2.png");

  // compute the factor
  float factor = computeFactor(im1, w1, im2, w2);
  cout << "factor: " << factor << endl;
}

void testMakeHDR() {
  // load an image sequence
  vector<Image> imSeq;
  imSeq.push_back(gamma_code(Image("./Input/design-1.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-2.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-3.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-4.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-5.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-6.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-7.png"), 1.0 / 2.2));

  // generate an hdr image
  Image hdr = makeHDR(imSeq);

  // save out images clipped to different ranges.
  float maxVal = hdr.max();
  Image hdrScale0 = gamma_code(hdr / maxVal, 2.2);
  hdrScale0.write("./Output/scaledHDR_design_0.png");
  Image hdrScale2 = gamma_code((2e2) * hdr / maxVal, 2.2);
  hdrScale2.write("./Output/scaledHDR_design_2.png");
  Image hdrScale4 = gamma_code((2e4) * hdr / maxVal, 2.2);
  hdrScale4.write("./Output/scaledHDR_design_4.png");
  Image hdrScale6 = gamma_code((2e6) * hdr / maxVal, 2.2);
  hdrScale6.write("./Output/scaledHDR_design_6.png");
  Image hdrScale8 = gamma_code((2e8) * hdr / maxVal, 2.2);
  hdrScale8.write("./Output/scaledHDR_design_8.png");
  Image hdrScale10 = gamma_code((2e10) * hdr / maxVal, 2.2);
  hdrScale10.write("./Output/scaledHDR_design_10.png");

  // load images
  vector<Image> imSeq0;
  imSeq0.push_back(gamma_code(Image("./Input/ante2-1.png"), 1.0 / 2.2));
  imSeq0.push_back(gamma_code(Image("./Input/ante2-2.png"), 1.0 / 2.2));
  Image hdr0 = makeHDR(imSeq0);
  float maxVal0 = hdr0.max();
  Image hdrScale000 = gamma_code(hdr0 / maxVal0, 2.2);
  hdrScale000.write("./Output/scaledHDR_ante2.png");
}

// HDR and Tone Mapping on Ante2 images
void testToneMapping_ante2() {

  // load images
  vector<Image> imSeq;
  imSeq.push_back(gamma_code(Image("./Input/ante2-1.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/ante2-2.png"), 1.0 / 2.2));

  // create hdr image
  Image hdr = makeHDR(imSeq);

  // tone map with gaussian blur
  Image tm = toneMap(hdr, 100, 1, false);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/ante2-tonedHDRsimple-gauss.png");

  // tone map with bilateral
  tm = toneMap(hdr, 100, 3, true, 0.1);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/ante2-tonedHDRsimple-bilateral.png");
}

// HDR and Tone Mapping on Ante3 images
void testToneMapping_ante3() {

  // load images
  vector<Image> imSeq;
  imSeq.push_back(gamma_code(Image("./Input/ante3-1.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/ante3-2.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/ante3-3.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/ante3-4.png"), 1.0 / 2.2));

  // create hdr image
  Image hdr = makeHDR(imSeq);

  // tone map with gaussian blur
  Image tm = gamma_code(toneMap(hdr, 100, 1), 2.2f);
  tm.write("./Output/ante3-tonedHDRsimple-gauss.png");

  // tone map with bilateral
  tm = gamma_code(toneMap(hdr, 100, 3, true, 0.1), 2.2f);
  tm.write("./Output/ante3-tonedHDRsimple-bilateral.png");
}

// HDR and Tone Mapping on Boston Images
void testToneMapping_boston() {

  // load images
  vector<Image> imSeq;
  imSeq.push_back(gamma_code(Image("./Input/boston-1.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/boston-2.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/boston-3.png"), 1.0 / 2.2));

  // create hdr image
  Image hdr = makeHDR(imSeq);

  // tone map with gaussian blur
  Image tm = toneMap(hdr, 100, 1, false);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/boston-tonedHDRsimple-gauss.png");

  // tone map with bilateral
  tm = toneMap(hdr, 100, 3, true, 0.1);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/boston-tonedHDRsimple-bilateral.png");
}

void testToneMapping_design() {

  // load images
  vector<Image> imSeq;
  imSeq.push_back(gamma_code(Image("./Input/design-1.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-2.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-3.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-4.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-5.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-6.png"), 1.0 / 2.2));
  imSeq.push_back(gamma_code(Image("./Input/design-7.png"), 1.0 / 2.2));

  // create hdr image
  Image hdr = makeHDR(imSeq);

  // tone map with gaussian blur
  Image tm = toneMap(hdr, 100, 1, false);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/design-tonedHDRsimple-gauss.png");

  // tone map with bilateral
  tm = toneMap(hdr, 100, 3, true, 0.1);
  tm = gamma_code(tm, 2.2);
  tm.write("./Output/design-tonedHDRsimple-bilateral.png");

  // Note: bilateral filtering these images takes a very long time. It is not
  // necessary to attempt this for testing
}

// This is a way for you to test your functions.
// We will only grade the contents of demosaic.cpp and align.cpp
int main() {
  clock_t start = clock();
  // cout << "nothing done in a4_main.cpp, debug me !" << endl;

  // testComputeWeight();
  // testComputeFactor();
  // testMakeHDR();
  // testToneMapping_ante2();
  testToneMapping_ante3();
  // testToneMapping_boston();
  // testToneMapping_design();

  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "a4_main.cpp runtime: " << duration << "s" << endl;
  return 0;
}
