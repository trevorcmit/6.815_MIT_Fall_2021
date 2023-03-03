/* -----------------------------------------------------------------
 * File:    a5_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-09-30
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/

#include "Image.h"
#include "basicImageManipulation.h"
#include "morphing.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void testNearestNeighbor() {
  // Test nearest neighbor
  const Image im("./Input/BostonRainbow-crop-400.png");
  float fact = 3.5;
  Image scaledNN = scaleNN(im, fact);
  scaledNN.write("./Output/testNN.png");
}

void testBilinearInterpolation() {
  // Test bilinear interpolation
  cout << endl << "linear interpolation" << endl;
  cout << "--------------------" << endl;
  Image test(2, 2, 1);
  test(0, 0, 0) = 0.0f;
  test(0, 1, 0) = 0.25f;
  test(1, 0, 0) = 0.5f;
  test(1, 1, 0) = 1.0f;

  float x = 0.25f, y = 0.0;
  cout << "interpolate at (" << x << ", " << y
       << "): " << interpolateLin(test, x, y, 0, false) << ", should be 0.125"
       << endl;

  x = 0.0;
  y = 0.25f;
  cout << "interpolate at (" << x << ", " << y
       << "): " << interpolateLin(test, x, y, 0, false) << ", should be 0.0625"
       << endl;

  x = 0.5;
  y = 0.5f;
  cout << "interpolate at (" << x << ", " << y
       << "): " << interpolateLin(test, x, y, 0, false) << ", should be 0.4375"
       << endl;
}

void testBilinearRescaling() {
  // Test bilinear
  const Image im("./Input/BostonRainbow-crop-400.png");
  float fact = 3.5;
  Image scaled = scaleLin(im, fact);
  scaled.write("./Output/testLin.png");
}

void testBicubicRescaling() {
  // Test Bicubic rescaling
  const Image im("./Input/BostonRainbow-crop-400.png");
  float B = 1/3, C = 1/3;
  Image scaled = scaleBicubic(im, 3.5f, B, C);
  scaled.write("./Output/testBicubic.png");
}

void testLanczosRescaling() {
  // Test Bicubic rescaling
  const Image im("./Input/BostonRainbow-crop-400.png");
  float A = 3;
  Image scaled = scaleLanczos(im, 3.5f, A);
  scaled.write("./Output/testLanczos.png");
}

void testRotation() {
  const Image im("./Input/BostonRainbow-crop-400.png");

  // float theta = M_PI / 8;
  float theta = 3.14159265 / 4;

  Image rot = rotate(im, theta);
  rot.write("./Output/testRotate.png");
}

void testVectorOperations() {
  // Test vector lib
  Vec2f a(10.0f, 3.0f);
  Vec2f b(-4.1f, 2.7f);
  float f = 2.0f;
  cout << endl << "vector operations" << endl;
  cout << "-----------------" << endl;

  cout << "a(" << a.x << ", " << a.y << ")" << endl;
  cout << "b(" << b.x << ", " << b.y << ")" << endl;
  cout << "f=" << f << endl;

  // a+b
  Vec2f c = a + b;
  cout << "a+b: (" << c.x << ", " << c.y << ")" << endl;

  // a-b
  c = a - b;
  cout << "a-b: (" << c.x << ", " << c.y << ")" << endl;

  // a*f
  c = a * f;
  cout << "a*f: (" << c.x << ", " << c.y << ")" << endl;

  // a/f
  c = a / f;
  cout << "a/f: (" << c.x << ", " << c.y << ")" << endl;

  // <a,b>
  float s = dot(a, b);
  cout << "<a,b>=" << s << endl;

  // ||a||
  float l = length(a);
  cout << "||a||=" << l << endl;

  Vec2f p_a = perpendicular(a);
  cout << "<a, perpendicular(a)>=" << dot(a, p_a) << endl;
}

void testSegment() {
  // Test segment
  // are P,Q,e1 e1 correctly implemented?
  // are the u,v coordinates meaningful?
  // What should be u and v for P,Q ?
  // Come up with a few test cases !

  Segment seg(Vec2f(4.0f, 3.0f), Vec2f(7.0f, 8.0f));

  Vec2f uv = seg.XtoUV(Vec2f(5.0f, 5.0f));
  Vec2f x  = seg.UVtoX(uv);

  cout << "Original = <5.0, 5.0>" << endl; 
  cout << "XtoUV = <" << uv.x << " " << uv.y << ">" << endl;
  cout << "UVtoX = <" << x.x << " " << x.y << ">" << endl;

  cout << endl << "Distance: " << seg.distance(Vec2f(5.0f, 5.0f)) << endl;
}

void testWarpBy1() {
  // Test warpBy1 ----------------------------------
  Image fredo("./Input/fredo2.png");
  Image bear("./Input/bear.png");

  // define before and after segments
  Segment segBefore(Vec2f(81, 105), Vec2f(122, 112));
  Segment segAfter(Vec2f(131, 168), Vec2f(134, 127));

  Segment bearb(Vec2f(0, 0), Vec2f(10, 0));
  Segment beara(Vec2f(10,10), Vec2f(30,15));

  // warp
  Image warp1 = warpBy1(fredo, segBefore, segAfter);
  Image warpbear = warpBy1(bear, bearb, beara);
  warp1.write("./Output/fredo_upside_down.png");
  warpbear.write("./Output/bear_warp.png");
  // ------------------------------------------------
}

void testWarp() {
  // Test warpBy1 ----------------------------------
  // Image fredo("./Input/fredo2.png");
  Image bear("./Input/bear.png");

  // define before and after segments
  // Segment segBefore(Vec2f(81, 105), Vec2f(122, 112));
  // Segment segAfter(Vec2f(131, 168), Vec2f(134, 127));

  Segment bearb1(Vec2f(0, 0), Vec2f(10, 0));
  Segment beara1(Vec2f(10, 10), Vec2f(30, 15));

  Segment bearb2(Vec2f(5, 5), Vec2f(20, 8));
  Segment beara2(Vec2f(10, 15), Vec2f(20, 30));

  vector<Segment> dst = {beara1, beara2};
  vector<Segment> src = {bearb1, bearb2};

  Image out = warp(bear, src, dst);

  // warp
  // Image warp1 = warpBy1(fredo, segBefore, segAfter);
  // Image warpbear = warpBy1(bear, bearb, beara);
  // warp1.write("./Output/fredo_upside_down.png");
  out.write("./Output/bear_warp_weighted.png");
  // ------------------------------------------------
}

void testMorph() {
  // Test morph
  Image fredo("./Input/fredo2.png");
  Image werewolf("./Input/werewolf.png");

  // ... use the UI to specify segments
  vector<Segment> segsBefore;
  segsBefore.push_back(Segment(Vec2f(85, 102), Vec2f(119, 108)));
  segsBefore.push_back(Segment(Vec2f(143, 117), Vec2f(169, 114)));
  vector<Segment> segsAfter;
  segsAfter.push_back(Segment(Vec2f(86, 93), Vec2f(114, 104)));
  segsAfter.push_back(Segment(Vec2f(137, 94), Vec2f(152, 79)));

  // This should monsterify Fredo a little.
  vector<Image> out = morph(fredo, werewolf, segsBefore, segsAfter, 1);

  // This is how you can write an image sequence
  for (int i = 0; i < (int)out.size(); ++i) {
    ostringstream fname;
    fname << "./Output/test_morph_";
    fname << setfill('0') << setw(2);
    fname << i;
    fname << ".png";
    out[i].write(fname.str());
  }
}

void classMorph() {
  // Test morph
  Image fredo("./Input/myface.png");
  Image werewolf("./Input/mikaeln.png");

  // ... use the UI to specify segments
  vector<Segment> segsBefore;
  segsBefore.push_back(Segment(Vec2f(164, 302), Vec2f(219, 299)));
  segsBefore.push_back(Segment(Vec2f(342, 300), Vec2f(294, 292)));
  segsBefore.push_back(Segment(Vec2f(206, 368), Vec2f(238, 295)));
  segsBefore.push_back(Segment(Vec2f(292, 372), Vec2f(275, 298)));
  segsBefore.push_back(Segment(Vec2f(194, 409), Vec2f(233, 402)));
  segsBefore.push_back(Segment(Vec2f(239, 402), Vec2f(293, 425)));
  segsBefore.push_back(Segment(Vec2f(299, 428), Vec2f(248, 450)));
  segsBefore.push_back(Segment(Vec2f(238, 451), Vec2f(194, 413)));
  segsBefore.push_back(Segment(Vec2f(139, 389), Vec2f(173, 468)));
  segsBefore.push_back(Segment(Vec2f(177, 475), Vec2f(251, 522)));
  segsBefore.push_back(Segment(Vec2f(259, 522), Vec2f(345, 473)));
  segsBefore.push_back(Segment(Vec2f(352, 465), Vec2f(376, 392)));
  segsBefore.push_back(Segment(Vec2f(135, 326), Vec2f(154, 190)));
  segsBefore.push_back(Segment(Vec2f(165, 178), Vec2f(342, 173)));
  segsBefore.push_back(Segment(Vec2f(354, 178), Vec2f(383, 322)));
  segsBefore.push_back(Segment(Vec2f(3, 480), Vec2f(150, 438)));
  segsBefore.push_back(Segment(Vec2f(365, 455), Vec2f(497, 504)));
  vector<Segment> segsAfter;
  segsAfter.push_back(Segment(Vec2f(184, 297), Vec2f(220, 293)));
  segsAfter.push_back(Segment(Vec2f(286, 288), Vec2f(251, 290)));
  segsAfter.push_back(Segment(Vec2f(217, 335), Vec2f(226, 296)));
  segsAfter.push_back(Segment(Vec2f(256, 333), Vec2f(244, 298)));
  segsAfter.push_back(Segment(Vec2f(205, 375), Vec2f(236, 366)));
  segsAfter.push_back(Segment(Vec2f(240, 367), Vec2f(270, 372)));
  segsAfter.push_back(Segment(Vec2f(272, 376), Vec2f(239, 393)));
  segsAfter.push_back(Segment(Vec2f(235, 397), Vec2f(206, 380)));
  segsAfter.push_back(Segment(Vec2f(169, 355), Vec2f(185, 399)));
  segsAfter.push_back(Segment(Vec2f(190, 404), Vec2f(230, 434)));
  segsAfter.push_back(Segment(Vec2f(242, 432), Vec2f(277, 407)));
  segsAfter.push_back(Segment(Vec2f(283, 399), Vec2f(301, 352)));
  segsAfter.push_back(Segment(Vec2f(148, 312), Vec2f(171, 242)));
  segsAfter.push_back(Segment(Vec2f(177, 236), Vec2f(286, 232)));
  segsAfter.push_back(Segment(Vec2f(293, 234), Vec2f(304, 308)));
  segsAfter.push_back(Segment(Vec2f(25, 535), Vec2f(164, 459)));
  segsAfter.push_back(Segment(Vec2f(297, 446), Vec2f(437, 533)));

  // This should monsterify Fredo a little.
  vector<Image> out = morph(fredo, werewolf, segsBefore, segsAfter, 30);

  // This is how you can write an image sequence
  for (int i = 0; i < (int)out.size(); ++i) {
    ostringstream fname;
    fname << "./Output/class_morph_";
    fname << setfill('0') << setw(2);
    fname << i;
    fname << ".png";
    out[i].write(fname.str());
  }
}

// This is a way for you to test your functions.
// We will only grade the contents of morphing.cpp and
// basicImageManipulation.cpp
int main() {
  clock_t start = clock();
  // cout << "nothing done in a5_main.cpp, debug me !" << endl;
  // testNearestNeighbor();
  // testBilinearInterpolation();
  // testBilinearRescaling();
  // testBicubicRescaling();
  // testLanczosRescaling();
  // testRotation();
  // testVectorOperations();
  // testSegment();
  // testWarpBy1();
  // testWarp();
  // testMorph();
  classMorph();

  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "a5_main.cpp runtime: " << duration << "s" << endl;
  return 0;
}
