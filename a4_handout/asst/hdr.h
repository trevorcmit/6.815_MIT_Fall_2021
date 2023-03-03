/* --------------------------------------------------------------------------
 * File:    hdr.h
 * Created: 2015-10-08
 * --------------------------------------------------------------------------
 *
 *  Assignment 04
 *
 * ------------------------------------------------------------------------*/

#ifndef __HDR__H
#define __HDR__H

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

using namespace std;

Image computeWeight(const Image &im, float epsilonMini = 0.002,
                    float epsilonMaxi = 0.99);
float computeFactor(const Image &im1, const Image &w1, const Image &im2,
                    const Image &w2);
Image makeHDR(vector<Image> &imSeq, float epsilonMini = 0.002,
              float epsilonMaxi = 0.99);

// Tone Mapping
Image toneMap(const Image &im, float targetBase = 100, float detailAmp = 3,
              bool useBila = false, float sigmaRange = 0.1);
Image exp10Image(const Image &im);
Image log10Image(const Image &im);
float image_minnonzero(const Image &im);

#endif
