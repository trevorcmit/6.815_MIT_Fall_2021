/* -----------------------------------------------------------------
 * File:    morphing.cpp
 * Created: 2015-09-25
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/

#include "morphing.h"
#include <cassert>

using namespace std;

Vec2f operator+(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the vector sum of a an b
  return Vec2f(a.x + b.x, a.y + b.y); //  Return sum of components in each component
}

Vec2f operator-(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a-b
  return Vec2f(a.x - b.x, a.y - b.y); //  Return difference of components in each component
}

Vec2f operator*(const Vec2f &a, float f) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a*f
  return Vec2f(a.x * f, a.y * f); // Multiply each component by factor f
}

Vec2f operator/(const Vec2f &a, float f) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a/f
  return Vec2f(a.x / f, a.y / f); // Divide each component by factor f
}

float dot(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the dot product of a and b.
  return (a.x * b.x) + (a.y * b.y); // Perform dot product calculation
}

float length(const Vec2f &a) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the length of a.
  return sqrt(pow(a.x, 2) + pow(a.y, 2)); // Square root of sum of squares of components
}

Vec2f perpendicular(const Vec2f &a) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a vector that is perpendicular to a.
  // Either direction is fine.
  return Vec2f(-1.0f * a.y, a.x); // -1 times Y component for X component, X component for Y component
}

// The Segment constructor takes in 2 points P(x1,y1) and Q(x2,y2) corresponding
// to the ends of a segment and initialize the local reference frame e1,e2.
Segment::Segment(Vec2f P_, Vec2f Q_) : P(P_), Q(Q_) {
  // // --------- HANDOUT  PS05 ------------------------------
  // // The initializer list above ": P(P_), Q(Q_)" already copies P_
  // // and Q_, so you don't have to do it in the body of the constructor.
  // You should:
  // * Initialize the local frame e1,e2 (see header file)
  Vec2f PQ = Q - P;
  Vec2f e1 = PQ / length(PQ);   // Unit vector setting orthonormal frame
  Vec2f e2 = perpendicular(PQ); // Completes orthonormal frame
}

Vec2f Segment::XtoUV(Vec2f X) const {
  // --------- HANDOUT  PS05 ------------------------------
  // Compute the u,v coordinates of a point X with
  // respect to the local frame of the segment.
  // e2 ^
  //    |
  // v  +  * X
  //    | /
  //    |/
  //    *--+------>-----*
  //    P  u     e1     Q
  //                    u=1
  //
  // * Be careful with the different normalization for u and v
  float u = dot(X - P, Q - P)                / pow(length(Q - P), 2);
  float v = dot(X - P, perpendicular(Q - P)) / length(Q - P); 
  return Vec2f(u, v); // Return both u and v
}

Vec2f Segment::UVtoX(Vec2f uv) const {
  // --------- HANDOUT  PS05 ------------------------------
  // compute the (x, y) position of a point given by the (u,v)
  // location relative to this segment.
  // * Be careful with the different normalization for u and v
  Vec2f x = (Q - P) * uv.x; //
  Vec2f y = (perpendicular(Q - P) * uv.y) / length(Q - P); 
  return P + x + y;
}

float Segment::distance(Vec2f X) const {
  // --------- HANDOUT  PS05 ------------------------------
  // Implement distance from a point X(x,y) to the segment. Remember the 3
  // cases from class.
  Vec2f uv = XtoUV(X); // If else if else statement for the three cases from class and in the paper
  if (uv.x > 1) {
    return sqrt(pow(Q.x - X.x, 2) + pow(Q.y - X.y, 2));
  }
  else if (uv.x < 0) {
    return sqrt(pow(P.x - X.x, 2) + pow(P.y - X.y, 2));
  }
  else {
    return abs(uv.y);
  }
}

Image warpBy1(const Image &im, const Segment &segBefore,
              const Segment &segAfter) {
  // --------- HANDOUT  PS05 ------------------------------
  // Warp an entire image according to a pair of segments.
  Image output(im.width(), im.height(), im.channels());
  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {
      Vec2f coords_xy = Vec2f(w, h);
      Vec2f coords_uv = segAfter.XtoUV(coords_xy);
      Vec2f source_xy = segBefore.UVtoX(coords_uv);
      for (int c = 0; c < output.channels(); c++) {
        output(w, h, c) = interpolateLin(im, source_xy.x, source_xy.y, c, true);
      }
    }
  }
  return output; 
}

float Segment::weight(Vec2f X, float a, float b, float p) const {
  // --------- HANDOUT  PS05 ------------------------------
  // compute the weight of a segment to a point X(x,y) given the weight
  // parameters a,b, and p (see paper for details).
  return pow(pow(length(X), p) / (a + distance(X)), b);
}

Image warp(const Image &im, const vector<Segment> &src_segs,
           const vector<Segment> &dst_segs, float a, float b, float p) {
  // --------- HANDOUT  PS05 ------------------------------
  // Warp an image according to a vector of before and after segments using segment weighting
  Image output(im.width(), im.height(), im.channels());

  for (int h = 0; h < output.height(); h++) { // Iterate over all pixels in the output image
    for (int w = 0; w < output.width(); w++) {

      float dsum_x = 0.0f, dsum_y = 0.0f, weightsum = 0.0f; // Initialize sum values
      Vec2f coords_xy = Vec2f(w, h);                        // Store coords outside of loop since they are static
      for (int i = 0; i < src_segs.size(); i++) {
        Vec2f coords_uv = dst_segs.at(i).XtoUV(coords_xy);        // Calculate u,v based on P_i Q_i
        Vec2f source_xy = src_segs.at(i).UVtoX(coords_uv);        // Calculate X_i' based on u,v and P_i' Q_i'
        Vec2f displace = source_xy - coords_xy;                   // Calculate displacement D_i = X_i' - X_i
        float weight = dst_segs.at(i).weight(coords_xy, a, b, p); // Calculate weight of vector

        dsum_x += displace.x * weight; // Add Displacement times Weight to DSUM metric
        dsum_y += displace.y * weight;
        weightsum += weight;           // Add weight to weightsum metric
      }
      float x_prime = w + (dsum_x / weightsum); // Find source image pixel to draw from
      float y_prime = h + (dsum_y / weightsum);
      for (int c = 0; c < output.channels(); c++) { // Pull pixel for each channel
        output(w, h, c) = interpolateLin(im, x_prime, y_prime, c, true);
      }
    }
  }
  return output; // Return output image
}

vector<Image> morph(const Image &im_before, const Image &im_after,
                    const vector<Segment> &segs_before,
                    const vector<Segment> &segs_after, int N, float a, float b,
                    float p) {
  // --------- HANDOUT  PS05 ------------------------------
  // return a vector of N+2 images: the two inputs plus N images that morphs
  // between im_before and im_after for the corresponding segments. im_before
  // should be the first image, im_after the last.
  compareDimensions(im_before, im_after); // Check for mismatched dimensions

  float float_n = static_cast<float>(N); // Case N as a float to use in fractional calculations
  vector<Image>output = {im_before};     // Initialize output array with first image added to it
  float time = 1.0f / (float_n + 1.0f);  // Calculate time increment as a float

  for (int n = 1; n < N + 1; n++) { // Iterate over all number of images required to span before to after
    vector<Segment> current_segs = {};
    for (int i = 0; i < segs_before.size(); i++) { // For each segment in the sequence...
      current_segs.push_back(                      // Add partial amount of difference between before/after
        Segment(
          Vec2f(segs_before.at(i).getP() + (segs_after.at(i).getP() - segs_before.at(i).getP()) * time * n), 
          Vec2f(segs_before.at(i).getQ() + (segs_after.at(i).getQ() - segs_before.at(i).getQ()) * time * n)
        )
      );
    }
    Image new_before = warp(im_before, segs_before, current_segs, a, b, p); // Warp before image to current segs
    Image new_after  = warp(im_after, segs_after, current_segs, a, b, p);   // Warp after image to current segs

    output.push_back( // Do linear interpolation of before and after image based on time increment
      new_before * ((float_n + 1 - n)/(float_n + 1)) + new_after * ((n)/(float_n + 1))
    );
  }
  output.push_back(im_after); // Add input after image
  return output;
}
