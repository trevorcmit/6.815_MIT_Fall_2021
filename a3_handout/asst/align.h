/* --------------------------------------------------------------------------
 * File:    align.h
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#ifndef __ALIGN__H
#define __ALIGN__H

#include <vector>
#include "Image.h"

Image denoiseSeq(const std::vector<Image> &imgs);
Image logSNR(const std::vector<Image> &imSeq, float scale = 1.0 / 20.0);
std::vector<int> align(const Image &im1, const Image &im2, int maxOffset = 20);
std::vector<int> align_faster(const Image &im1, const Image &im2, int maxOffset = 20);
Image alignAndDenoise(const std::vector<Image> &imSeq, int maxOffset = 10);
Image split(const Image &sergeyImg);
Image sergeyRGB(const Image &sergeyImg, int maxOffset = 20);
Image roll(const Image &im, int xRoll, int yRoll);

#endif
