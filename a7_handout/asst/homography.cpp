#include "homography.h"
#include "matrix.h"

using namespace std;

void applyHomography(const Image &source, const Matrix &H, Image &out, bool bilinear) {
  // --------- HANDOUT  PS06 ------------------------------
  // Transform image source using the homography H, and composite in onto out.
  // if bilinear == true, using bilinear interpolation. Use nearest neighbor
  // otherwise.
  Matrix H_inv = H.inverse(); // Find inverse of H to use for calculations

  for (int h = 0; h < out.height(); h++) {
    for (int w = 0; w < out.width(); w++) {

      Matrix coords = Matrix::Zero(3, 1); // create column matrix [x y 1]
      coords << w, h, 1;

      Matrix xyw_prime = H_inv * coords; // Multiply H^-1 times output column matrix

      float x_prime = xyw_prime(0, 0) / xyw_prime(2, 0); // x'/w'
      float y_prime = xyw_prime(1, 0) / xyw_prime(2, 0); // y'/w'

      if ((x_prime < source.width() && x_prime > 0) &&   // Check bounds, only utilize if valid in source image
          (y_prime < source.height() && y_prime > 0)) {
        
        for (int c = 0; c < out.channels(); c++) { // Iterate for all channels, coords are the same
          if (bilinear) {
            out(w, h, c) = interpolateLin(source, x_prime, y_prime, c, true); // If True use bilinear
          }
          else {
            out(w, h, c) = source.smartAccessor(round(x_prime), round(y_prime), c, true);
          }
        }
      }
    }
  }
  
}

Matrix computeHomography(const CorrespondencePair correspondences[4]) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute a homography from 4 point correspondences.
	Matrix A(8, 8); // Matrix to represent system of equations for all four correspondences (8 equations)
	Matrix B(8, 1); // Column vector of new x and y coordinates for all correspondences

  for (int n = 0; n < 4; n++) {
    B(2*n)   = correspondences[n].point2[0]; // New x coordinate
    B(2*n+1) = correspondences[n].point2[1]; // New y coordinate

    A(2*n, 0) = correspondences[n].point1[0]; // x'
    A(2*n, 1) = correspondences[n].point1[1]; // y'
    A(2*n, 2) = correspondences[n].point1[2]; // z' (this value will be 1)
    A(2*n, 3) = 0;
    A(2*n, 4) = 0;
    A(2*n, 5) = 0;
    A(2*n, 6) = -correspondences[n].point1[0]*correspondences[n].point2[0]; // -x'x
    A(2*n, 7) = -correspondences[n].point1[1]*correspondences[n].point2[0]; // -y'x
    A(2*n+1, 0) = 0;
    A(2*n+1, 1) = 0;
    A(2*n+1, 2) = 0;
    A(2*n+1, 3) = correspondences[n].point1[0]; // x'
    A(2*n+1, 4) = correspondences[n].point1[1]; // y'
    A(2*n+1, 5) = correspondences[n].point1[2]; // z'
    A(2*n+1, 6) = -correspondences[n].point1[0]*correspondences[n].point2[1]; // -x'y
    A(2*n+1, 7) = -correspondences[n].point1[1]*correspondences[n].point2[1]; // -y'y
  }

  Matrix x(8, 1);      // Initialize column vector for x
	x = A.inverse() * B; // Solving Ax = b
  Matrix output(3, 3); // Turn x into a 3x3 matrix
	output << x(0, 0), x(1, 0), x(2, 0), x(3, 0), x(4, 0), x(5, 0), x(6, 0), x(7, 0), 1; // 1 is bottom right corner
  return output;
}

BoundingBox computeTransformedBBox(int imwidth, int imheight, Matrix H) {
  // --------- HANDOUT  PS06 ------------------------------
  // Predict the bounding boxes that encompasses all the transformed
  // coordinates for pixels frow and Image with size (imwidth, imheight)

  Matrix src0(3, 1), src1(3, 1), src2(3, 1), src3(3, 1); // Source coord. column vectors
	Matrix out0(3, 1), out1(3, 1), out2(3, 1), out3(3, 1); // Output coord. column vectors

  src0 << 0, 0, 1;
	src1 << 0, imheight, 1; // Add four corner coordinates to source column vectors 
	src2 << imwidth, 0, 1;
	src3 << imwidth, imheight, 1;

  out0 = H * src0;
	out1 = H * src1; // Multiply H by source column vectors to fill output column vectors
	out2 = H * src2;
	out3 = H * src3;

  out0(0, 0) /= out0(2, 0);
	out0(1, 0) /= out0(2, 0); // Divide out z factor
	out1(0, 0) /= out1(2, 0);
	out1(1, 0) /= out1(2, 0);
	out2(0, 0) /= out2(2, 0);
	out2(1, 0) /= out2(2, 0);
	out3(0, 0) /= out3(2, 0);
	out3(1, 0) /= out3(2, 0);

  int x_min = out0(0, 0), x_max = out0(0, 0), y_min = out0(1, 0), y_max = out0(1, 0); // Initialize bounds

  vector<Matrix> outs = {out1, out2, out3}; // Make vector so iteration can do work in less lines
  for (int n = 0; n < 3; n++) {
    if (outs.at(n)(0, 0) < x_min) x_min = outs.at(n)(0, 0); // Conditionals for each boundary
    if (outs.at(n)(0, 0) > x_max) x_max = outs.at(n)(0, 0);
    if (outs.at(n)(1, 0) < y_min) y_min = outs.at(n)(1, 0);
    if (outs.at(n)(1, 0) > y_max) y_max = outs.at(n)(1, 0);
  }
  return BoundingBox(x_min, x_max, y_min, y_max); // Return Box object with solved coordinates
}

BoundingBox bboxUnion(BoundingBox B1, BoundingBox B2) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute the bounding box that tightly bounds the union of B1 an B2.
  return BoundingBox( // Ternary operator to get minimums/maximums over both boxes
    (B1.x1 < B2.x1) ? B1.x1 : B2.x1, 
    (B1.x2 > B2.x2) ? B1.x2 : B2.x2, 
    (B1.y1 < B2.y1) ? B1.y1 : B2.y2, 
    (B1.y2 > B2.y2) ? B1.y2 : B2.y2
  );
}

Matrix makeTranslation(BoundingBox B) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute a translation matrix (as a homography matrix) that translates the
  // top-left corner of B to (0,0).
  Matrix output = Matrix::Zero(3, 3); // Initialize 3x3 output homography matrix
  output << 1, 0, -1 * B.x1, 0, 1, -1 * B.y1, 0, 0, 1; // Fill matrix
  return output;
}

Image stitch(const Image &im1, const Image &im2, const CorrespondencePair correspondences[4]) {
  // --------- HANDOUT  PS06 ------------------------------
  // Transform im1 to align with im2 according to the set of correspondences.
  // make sure the union of the bounding boxes for im2 and transformed_im1 is
  // translated properly (use makeTranslation)

  Matrix H = computeHomography(correspondences); // Compute homography from correspondences

  BoundingBox bbox = bboxUnion(
    computeTransformedBBox(im1.width(), im1.height(), H), // Bounding box for applying homography to im1
    computeTransformedBBox(im2.width(), im2.height(), Matrix::Identity(3,3)) // Add image 2 size
  );
  Matrix translate = makeTranslation(bbox); // Get translate based on combination of both boxes

  Image output(bbox.x2 - bbox.x1, bbox.y2 - bbox.y1, im2.channels()); // Create output
  output.set_color(); // Set to black so pixels can be adjusted by applyHomography

  applyHomography(im1, translate * H, output, true); // translation and homography on image 1
  applyHomography(im2, translate, output, true);     // solely translation on image 2
  return output;
}

// debug-useful
Image drawBoundingBox(const Image &im, BoundingBox bbox) {
  // --------- HANDOUT  PS06 ------------------------------
  /*
    ________________________________________
   / Draw me a bounding box!                \
   |                                        |
   | "I jumped to my                        |
   | feet, completely thunderstruck. I      |
   | blinked my eyes hard. I looked         |
   | carefully all around me. And I saw a   |
   | most extraordinary small person, who   |
   | stood there examining me with great    |
   | seriousness."                          |
   \              Antoine de Saint-Exupery  /
    ----------------------------------------
           \   ^__^
            \  (oo)\_______
               (__)\       )\/\
                   ||----w |
                   ||     ||
  */
  Image output = im;
  output.create_rectangle(bbox.x1, bbox.y1, bbox.x2, bbox.y2, 1.0f, 1.0f, 1.0f);
  return output;
}


void applyHomographyFast(const Image &source, const Matrix &H, Image &out, bool bilinear) {
  // --------- HANDOUT  PS06 ------------------------------
  // Same as apply but change only the pixels of out that are within the
  // predicted bounding box (when H maps source to its new position).
  Matrix H_inv = H.inverse(); // Find inverse of H to use for calculations

  BoundingBox bounds = computeTransformedBBox(source.width(), source.height(), H);

  for (int h = bounds.y1; h < bounds.y2; h++) {
    for (int w = bounds.x1; w < bounds.x2; w++) {

      Matrix coords = Matrix::Zero(3, 1); // create column matrix [x y 1]
      coords << w, h, 1;

      Matrix xyw_prime = H_inv * coords; // Multiply H^-1 times output column matrix

      float x_prime = xyw_prime(0, 0) / xyw_prime(2, 0); // x'/w'
      float y_prime = xyw_prime(1, 0) / xyw_prime(2, 0); // y'/w'

      if ((x_prime < source.width() && x_prime > 0) &&   // Check bounds, only utilize if valid in source image
          (y_prime < source.height() && y_prime > 0)) {
        
        for (int c = 0; c < out.channels(); c++) { // Iterate for all channels, coords are the same
          if (bilinear) {
            out(w, h, c) = interpolateLin(source, x_prime, y_prime, c, true); // If True use bilinear
          }
          else {
            out(w, h, c) = source.smartAccessor(round(x_prime), round(y_prime), c, true);
          }
        }
      }
    }
  }
  
}

Matrix compute_homography_rotation(const CorrespondencePair correspondences[2]) {
  // --------- EXTRA CREDIT PS06 ------------------------------
  // Using two points, create  3x3 homography matrix (rotation matrix)
  Matrix H = Matrix::Zero(3, 3);

  return H;
}
