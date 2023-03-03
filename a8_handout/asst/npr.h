/* --------------------------------------------------------------------------
 * File:    npr.h
 * Created: 2021-10-28
 * --------------------------------------------------------------------------
 *
 *  Assignment 12
 *
 * ------------------------------------------------------------------------*/

#ifndef __NPR__H
#define __NPR__H

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

using namespace std;

Image brush(const Image &im, int x, int y, vector<float> color, const Image &texture);
Image singleScalePaint(const Image &im, const Image &out, const Image &texture, int size=10, int N=10000, float noise=0.3);
Image singleScalePaintImportance(const Image &im, const Image &importance, const Image &out, const Image &texture, 
			int size=10, int N=10000, float noise=0.3);

Image sharpnessMap(const Image &im, float sigma=1.0f);
Image painterly(const Image &im, const Image &texture, int N=10000, int size=50, int noise=0.3);

Image computeTensor(const Image &im, float sigmaG=1.0f, float factor=4.0f);


Image testAngle(const Image &im, float sigmaG=3.0f, float factor=5.0f);

vector<Image> rotateBrushes(const Image &im, int nAngles);

Image singleScaleOrientedPaint(const Image &im, const Image &importance, const Image &out, const Image &tensor,
	const Image &texture, int size, int N, float noise, int nAngles=36);

Image orientedPaint(const Image &im, const Image &texture, int N=10000, int size=50, int noise=0.3);


#endif
