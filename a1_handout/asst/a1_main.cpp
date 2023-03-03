#include "a1.h"
#include <iostream>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of a1.cpp and Image.cpp
int main() {
  // cout << "nothing done in a1_main.cpp, debug me !" << endl;
  
  {
      Image im(256, 128, 3);
      im.set_color(0.3, 0.9, 0.5);
    
      Image out = grayworld(im);
      out.write("./Output/WhiteBalance+1+grayworld.png");
    }
    {
      Image im(256, 128, 3);
      im.set_color(0.1, 1.0, 1.0);
      Image out = grayworld(im);
      out.write("./Output/WhiteBalance+2+grayworld.png");
    }

  {
      Image im(256, 128, 3);
      im.set_color(0.5, 0.5, 0.5);
      Image out = grayworld(im);
      out.write("./Output/WhiteBalance+3+grayworld.png");
    }


    {
      Image im("./Input/flower.png");
      Image out = grayworld(im);
      out.write("./Output/WhiteBalance+4+grayworld.png");
    }

  
  // Image im("./Input/castle_small.png");
  // Image im2("./Input/zebra.png");

  // std::vector<Image> sp = spanish(im);
  // sp[0].write("./Output/Spanish+1+spanish.png");
  // sp[1].write("./Output/Spanish+2+spanish.png");

  // sp = spanish(im2);
  // sp[0].write("./Output/Spanish+3+spanish.png");
  // sp[1].write("./Output/Spanish+4+spanish.png");

  // {
  //   // gray gradient
  //   Image im(256, 32, 3);
  //   for (int z = 0 ; z < 3 ; ++z) {
  //     for (int y = 0 ; y < im.height() ; ++y) {
  //       for (int x = 0 ; x < im.width() ; ++x) {
  //         im(x, y, z) = (float)x / (im.width() - 1);
  //       }
  //     } 
  //   }
  //   Image out = quantize(im, 2);
  //   out.write("./Output/Quantize+1+quantize.png");
  // }
  // {
  //   // green-red gradient
  //   Image im(256, 32, 3);
  
  //   for (int y = 0 ; y < im.height() ; ++y) {
  //     for (int x = 0 ; x < im.width() ; ++x) {
  //       im(x, y, 0) = (float)x / (im.width() - 1);
  //       im(x, y, 1) = 1 - (float)x / (im.width() - 1);
  //       im(x, y, 2) = 0;
  //     }
  //   } 
  //   Image out = quantize(im, 4);
  //   out.write("./Output/Quantize+2+quantize.png");

  // }
  // {
  //   Image im("./Input/flower.png");
  //   Image out = quantize(im, 4);
  //   out.write("./Output/Quantize+3+quantize.png");
  //   return 0;
  // }

  // Image im("./Input/Cambridge1.png");
  // std::vector<Image> out = gamma_test(im, 6, 2.2f);
  // out[0].write("./Output/gammatest_0.png");
  // out[1].write("./Output/gammatest_1.png");

  // Image im("./Input/castle_small.png");
  // Image out = rgb2yuv(im);
  // out.write("./Output/RGBandYUV+1+rgb2yuv.png");

  // Image out2 = yuv2rgb(rgb2yuv(im));
  // out2.write("./Output/RGBandYUV+2+rgb2yuv.png");
  // out2.write("./Output/RGBandYUV+3+yuv2rgb.png");


  // Image im("./Input/zebra.png");
  // Image out = brightnessContrastLumi(im, 1.0f, 1.8f, 0.43f);
  // out.write("./Output/ProcessLuminance+1+brightnessContrastLumi.png");
  // out.write("./Output/ProcessLuminance+2+lumiChromi.png");
  // out.write("./Output/ProcessLuminance+3+brightness.png");
  // out.write("./Output/ProcessLuminance+4+contrast.png");

  // Image im("./Input/castle_small.png");
  // std::vector<Image> LC = lumiChromi(im);
  // cout << "finished lumichromi(im)" << endl;
  // LC[0].write("./Output/LuminanceChrominance+1+lumiChromi.png");
  // LC[1].write("./Output/LuminanceChrominance+2+lumiChromi.png");
  // LC[0].write("./Output/LuminanceChrominance+3+color2gray.png");
  // LC[1].write("./Output/LuminanceChrominance+4+color2gray.png");


  // Colorspace tests
  // Image im("./Input/castle_small.png");
  // Image out = color2gray(im);
  // out.write("./Output/Color2Gray+1+color2gray.png");
  // return 0;

  // Example
  // Image im("./Input/castle_small.png");
  // std::vector<Image> LC = lumiChromi(im);
  // LC[0].write("./Output/castle_luminance.png");
  // LC[1].write("./Output/castle_chrominance.png");
}
