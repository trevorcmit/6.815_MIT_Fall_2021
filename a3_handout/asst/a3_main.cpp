/* -----------------------------------------------------------------
 * File:    a3_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-09-30
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/

#include "Image.h"
#include "align.h"
#include "basicImageManipulation.h"
#include "demosaic.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of demosaic.cpp and align.cpp
int main() {
  clock_t start = clock();

  // cout << "nothing done in a3_main.cpp, debug me !" << endl;
  // Denoise ---------------------------
  // Load sequence
  // vector<Image> seq0;
  // int n_images0 = 16;
  // for (int i = 1; i <= n_images0; ++i) {
  //     ostringstream fname0;
  //     fname0 << "./Input/aligned-ISO3200/1D2N-iso3200-";
  //     fname0 << i;
  //     fname0 << ".png";
  //     seq0.push_back(Image(fname0.str()));
  // }
  // vector<Image> seq1;
  // int n_images1 = 16;
  // for (int i = 1; i <= n_images1; ++i) {
  //     ostringstream fname1;
  //     fname1 << "./Input/aligned-ISO400/1D2N-iso400-";
  //     fname1 << i;
  //     fname1 << ".png";
  //     seq1.push_back(Image(fname1.str()));
  // }
  // Denoise
  // Image out0 = denoiseSeq(seq0);
  // out0.write("./Output/denoised_3200_16.png");
  // Image out1 = denoiseSeq(seq1);
  // out1.write("./Output/denoised_400_16.png");

  //
  // Image SNRIm0 = logSNR(seq0, 1/30.0);
  // SNRIm0.write("./Output/snr_map_3200.png");
  // Image SNRIm1 = logSNR(seq1, 1/30.0);
  // SNRIm1.write("./Output/snr_map_400.png");

  // clock_t end = clock();
  // double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  // cout << "a3_main.cpp runtime: " << duration << "s" << endl;

  // Image first(100, 100, 3);
  // Image second(100, 100, 3);
  // first.create_rectangle(40, 40, 60, 50, 1.0f, 1.0f, 1.0f);
  // second.create_rectangle(50, 60, 70, 70, 1.0f, 1.0f, 1.0f);
  // clock_t start0 = clock();
  // vector<int> c0 = align(first, second, 20);
  // clock_t end0 = clock();
  // clock_t start1 = clock();
  // vector<int> c1 = align_faster(first, second, 20);
  // clock_t end1 = clock();

  // // clock_t end = clock();
  // double duration0 = (end0 - start0) * 1.0f / CLOCKS_PER_SEC;
  // double duration1 = (end1 - start1) * 1.0f / CLOCKS_PER_SEC;
  // cout << "normal: " << duration0 << ", faster: " << duration1 << endl;
  // cout << c0.at(0) << " " << c0.at(1) << endl;
  // cout << c1.at(0) << " " << c1.at(1) << endl;

  // vector<Image> seq1;
  // int n_images1 = 9;
  // for (int i = 1; i <= n_images1; ++i) {
  //     ostringstream fname1;
  //     fname1 << "./Input/green/noise-small-";
  //     fname1 << i;
  //     fname1 << ".png";
  //     seq1.push_back(Image(fname1.str()));
  // }
  // Image out0 = alignAndDenoise(seq1, 20);
  // out0.write("./Output/green_alignanddenoise_9im_20.png");
  
  // // Demosaic ---------------------------
  // Image raw("./Input/raw/signs-small.png");
  // Image green = edgeBasedGreen(raw, 1);
  // green.write("./Output/demosaic_edgebased_green.png"); // edge based green demosaicing
  // Image im1 = basicRorB(raw, 0, 0);
  // Image blue = basicRorB(raw, 1, 1);
  // red.write("./Output/demosaic_red.png");
  // blue.write("./Output/demosaic_blue.png");
  // Image red = basicRorB(raw, 1, 1);
  // red.write("./Output/demosaic_red.png");
  // Image blue = basicRorB(raw, 0, 0);
  // blue.write("./Output/demosaic_blue.png");
  // Image rgb = basicDemosaic(raw, 1, 1, 1, 0, 0);
  // rgb.write("./Output/demosaiced.png");
  // Image rgb_edgebased = improvedDemosaic(raw, 1, 1, 1, 0, 0);
  // rgb_edgebased.write("./Output/demosaiced_improved.png");
  //
  //
  // // Sergey ---------------------------
  Image sergeyImg("./Input/Sergey/00911v_third.png");
  Image rgb2 = split(sergeyImg);
  rgb2.write("./Output/Sergey_split.png");
  Image rgbAlign = sergeyRGB(sergeyImg, 35);
  rgbAlign.write("./Output/Sergey_aligned.png");


  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "a3_main.cpp runtime: " << duration << "s" << endl;
  return 0;
}
