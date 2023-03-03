/* --------------------------------------------------------------------------
 * File:    a6_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-10-17
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "homography.h"
#include "matrix.h"
#include <cassert>
#include <ctime>
#include <iostream>

// for 6.865 only
#include "homography_extra_tests.h"

using namespace std;

void testEigen() {
  //--------------------------------------------------------
  // In this pset we use Eigen as our linear algebra library
  //--------------------------------------------------------

  // This is how you allocate a matrix:
  int n_rows = 2, n_cols = 3;
  Matrix M(n_rows, n_cols);
  // At this point, M contains garbage data.

  // You can inspect it like so
  // (depending on the platform this can be junk or zeros,
  // but to be sure, always initialize it yourself!!!)
  cout << "Matrix with no initialization: " << endl << M << endl;

  // You can modify the individual entries like this:
  M(0, 2) = 0.4f;
  cout << "after changing one value: " << endl << M << endl;

  // Or you can initialize a matrix of all zeros like this
  Matrix M2 = Matrix::Zero(n_rows, n_cols);
  cout << "Matrix initialized to zeros: " << endl << M2 << endl;

  // other ways to initialize
  // https://eigen.tuxfamily.org/dox/group__TutorialAdvancedInitialization.html
  Matrix A(3, 2);
  Matrix B(2, 4);
  // this assumes row-major order (which is Eigen's default)
  A << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
  cout << "Matrix initialized with comma initializer : " << endl << A << endl;

  // all twos
  B.fill(2.0);
  cout << "Matrix fill : " << endl << B << endl;

  // matrix multiplication
  Matrix C = A * B;
  cout << "A * B = " << endl << C << endl;

  // We provide two shortcuts for 2D and 3D vectors:
  Vec2f x; // equivalent to Matrix x(2,1);

  // you can set elements as before
  x(0) = 1.0;
  x(1) = 2.0;
  cout << "vector x = " << endl << x << endl;

  // these allow specific size constructor
  // equivalent to Matrix y(3,1) initialized to [1; 2; 3]
  Vec3f y(1.0, 2.0, 3.0);
  cout << "vector y = " << endl << y << endl;

  // You can do matrix-vector product like so:
  Matrix E = A * x;
  cout << "A * x = " << endl << E << endl;

  // You get the inverse of a matrix as:
  Matrix F(2, 2);
  F << 1.0, 2.0, 3.0, 4.0;
  Matrix G = F.inverse();
  cout << "matrix F: " << endl << F << endl;
  cout << "inverse of F: " << endl << G << endl;

  // For more information on the operations you can do with Eigen,
  // consult this page: http://eigen.tuxfamily.org/index.php?title=Main_Page

  // To solve the linear system Ax = b, you can do x = A^{-1}*b
  // or use a solver routine such as:
  Matrix H = A.fullPivLu().solve(y);
  cout << "solving A * x = y for x yields : " << endl << H << endl;
}

// test apply homography function
void testApplyHomography() {
  Matrix H(3, 3);
  H << 1.23445, -0.0258246, 96, 0.210363, 1.00462, 171, 0.00108756, -0.00010989,
      1;

  cout << "Given manual homography for train example:" << endl;
  cout << H << endl;

  // Load inputs
  Image poster("Input/poster.png");
  Image green("Input/green.png");

  // apply homography
  applyHomography(poster, H, green, true);
  green.write("Output/apply_homography.png");
}

void testComputeHomography() {
  Image poster("Input/poster.png");
  Image green("Input/green.png");

  // build 3D array (easier to initialize)
  float w = (float)poster.width();
  float h = (float)poster.height();
  Vec3f p1;
  Vec3f p2;

  CorrespondencePair corresp[4] = {
      CorrespondencePair(0, 0, 1, 96, 171, 1),
      CorrespondencePair(w - 1, 0, 1, 235, 174, 1),
      CorrespondencePair(w - 1, h - 1, 1, 235, 232, 1),
      CorrespondencePair(0, h - 1, 1, 95, 238, 1)};

  // Compute homography.
  Matrix Hcomputed = computeHomography(corresp);

  // This should get you the values from
  // testApplyHomography(); (or close to)
  cout << "Computed homography for bus example from given point pairs" << endl;
  cout << Hcomputed << endl;

  // Transform image and write output
  applyHomography(poster, Hcomputed, green, true);
  green.write("Output/computeHomography.png");
}

void testComputeTransformedBBox() {
  Matrix H(3, 3);
  H << 1.23445, -0.0258246, 96, 0.210363, 1.00462, 171, 0.00108756, -0.00010989,
      1;

  // Load inputs
  Image poster("Input/poster.png");

  BoundingBox bbox = computeTransformedBBox(poster.width(), poster.height(), H);
  cout << "Computed transformed BBox for Poster:" << endl;
  cout << bbox.x1 << " " << bbox.x2 << " " << bbox.y1 << " " << bbox.y2 << " "
       << endl;

  // Students TODO: drawBoundingBox would be useful to implement for
  // debugging
  Image green("Input/green.png");
  Image greenwithbox = drawBoundingBox(green, bbox);
  greenwithbox.write("Output/draw_bbox.png");
}

// test union
void testBBoxUnion() {
  BoundingBox bbox1(-15, 25, 3, 109);
  BoundingBox bbox2(-75, 19, 9, 20);
  BoundingBox bbox = bboxUnion(bbox1, bbox2);
  assert(bbox.x1 == -75 && bbox.x2 == 25 && bbox.y1 == 3 && bbox.y2 == 109);
  cout << bbox.x1 << " " << bbox.x2 << " " << bbox.y1 << " " << bbox.y2 << " " << endl;
}

void testTranslate() {
  BoundingBox bbox(-15, 25, 3, 109);

  // look at translation matrix
  cout << "Translation Matrix:" << endl;
  Matrix m = makeTranslation(bbox);
  cout << m << endl;

  // transform top left of bounding box, see if it's [0,0].
  Matrix b(3, 1);
  b << bbox.x1, bbox.y1, 1;
  cout << "Corner of translated Bounding Box:" << endl;
  cout << m * b << endl;
}

// test monu
void testStitchMonu() {
  // notice these are flip to align on the right-hand side
  Image monu1 = Image("Input/monu-2.png");
  Image monu2 = Image("Input/monu-1.png");
  CorrespondencePair corresp[4] = {
      CorrespondencePair(226, 154, 1, 417, 156, 1),
      CorrespondencePair(4, 106, 1, 210, 118, 1),
      CorrespondencePair(57, 260, 1, 249, 254, 1),
      CorrespondencePair(246, 295, 1, 441, 314, 1)};

  Image monu = stitch(monu1, monu2, corresp);
  monu.write("./Output/monu-stitch.png");
}

// test stitch with stata
void testStitchStata() {
  Image im1("Input/stata-1.png");
  Image im2("Input/stata-2.png");
  CorrespondencePair corresp[4] = {
      CorrespondencePair(218, 209, 1, 4, 232, 1),
      CorrespondencePair(300, 425, 1, 62, 465, 1),
      CorrespondencePair(337, 209, 1, 125, 247, 1),
      CorrespondencePair(336, 396, 1, 102, 433, 1)};
  Image st = stitch(im1, im2, corresp);
  st.write("./Output/stata-stitch.png");
}

// test stitch with Guedelon
void testStitchGuedelon() {
  Image im1 = Image("./Input/guedelon-1.png");
  Image im2 = Image("./Input/guedelon-2.png");
  CorrespondencePair corresp[4] = {CorrespondencePair(310, 338, 1, 127, 333, 1),
                                   CorrespondencePair(225, 182, 1, 61, 173, 1),
                                   CorrespondencePair(216, 346, 1, 35, 341, 1),
                                   CorrespondencePair(224, 425, 1, 36, 423, 1)};

  Image out = stitch(im1, im2, corresp);
  out.write("./Output/guedelon-stitch.png");
}


void testApplyHomographyFast() {
  clock_t start;

  Matrix H(3, 3);
  H << 1.23445, -0.0258246, 96, 0.210363, 1.00462, 171, 0.00108756, -0.00010989,
      1;

  cout << H << endl;

  // apply homography and output image
  Image poster("Input/poster.png");
  Image green("Input/green.png");
  start = std::clock();
  applyHomography(poster, H, green, false);
  float timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
  printf("Time taken with normal applyhomography: %fms\n", timediff);
  green.write("Output/greenwithposter_slow.png");

  // apply it fast
  green = Image("Input/green.png");
  start = std::clock();
  applyHomographyFast(poster, H, green, false);
  timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
  printf("Time taken with fast time: %fms\n", timediff);
  green.write("Output/greenwithposter_fast.png");
}

// This is a way for you to test your functions.
int main() {
  // cout << "nothing done in a6_main.cpp, but read me! "
  //      << "There's some useful info here." << endl;

  clock_t start = clock();

  // testEigen();
  testApplyHomography();
  testComputeHomography();
  testComputeTransformedBBox();
  testBBoxUnion();
  testTranslate();
  testStitchMonu();
  testStitchStata();
  testStitchGuedelon();
  testApplyHomographyFast();

  ////--------------- for 6.865 only
  testStitchScience();
  testStitchConvention();
  testStitchBoston1();

  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "a6_main.cpp runtime: " << duration << "s" << endl;
  return 0;
}
