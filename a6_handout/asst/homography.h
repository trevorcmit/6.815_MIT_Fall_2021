/* --------------------------------------------------------------------------
 * File:    homography.h
 * Created: 2015-10-17
 * --------------------------------------------------------------------------
 *
 * Problem set 6
 *
 * ------------------------------------------------------------------------*/

#ifndef __HOMOGRAPHY__H
#define __HOMOGRAPHY__H

#include "Image.h"
#include "basicImageManipulation.h"
#include "matrix.h"
#include <cmath>
#include <iostream>

using namespace std;

/*********************************************************************
 *                              Classes                              *
 *********************************************************************/

class BoundingBox
//
// +--------------+ y1
// |              |
// |   a BBox !!  |
// |              |
// +--------------+ y2
// x1             x2
//
{
public:
  BoundingBox(int x1, int x2, int y1, int y2)
      : x1(x1), x2(x2), y1(y1), y2(y2){};
  int x1, x2;
  int y1, y2;
};

class CorrespondencePair
// Pair of corresponding points defined in homogeneous coordinates.
// point1 is in the first image,
// point2 in the second.
{
public:
  CorrespondencePair(Vec3f point1, Vec3f point2)
      : point1(point1), point2(point2){};
  CorrespondencePair(int x1, int y1, int z1, int x2, int y2, int z2) {
    point1 << x1, y1, z1;
    point2 << x2, y2, z2;
  };
  Vec3f point1;
  Vec3f point2;
};

/*********************************************************************
 *                             Functions                             *
 *********************************************************************/

// Apply homographies
void applyHomography(const Image &source, const Matrix &H, Image &out,
                     bool bilinear = false);
void applyHomographyFast(const Image &source, const Matrix &H, Image &out,
                         bool bilinear = false);
void applyHomographyBbox(const Image &src, const Matrix &H, Image &out,
                         BoundingBox bbox, bool bilinear);

// Estimate homography
Matrix computeHomography(const CorrespondencePair correspondences[4]);
void addConstraint(Matrix &system, int i, const CorrespondencePair corresp);

// bbox related functions
BoundingBox computeTransformedBBox(int imwidth, int imheight, Matrix H);
BoundingBox bboxUnion(BoundingBox B1, BoundingBox B2);
Matrix makeTranslation(BoundingBox B);

// would help you debug
Image drawBoundingBox(const Image &im, BoundingBox bbox);

// stitching
Image stitch(const Image &im1, const Image &im2,
             const CorrespondencePair correspondences[4]);

#endif
