// npr.cpp
// Assignment 12

#include "npr.h"
#include "filtering.h"
#include "matrix.h"
#include <algorithm>
#include <math.h>

using namespace std;

/**************************************************************
 //                       NPR                                //
 *************************************************************/
Image brush(const Image &im, int x, int y, vector<float> color, const Image &texture) {
	// Draws a brushstroke defined by texture and color at (x,y) in im
	// --------- HANDOUT  PS12 ------------------------------
	Image output = im;
	if (((x < texture.width() / 2) || (x > im.width() - texture.width() / 2)) ||
	    ((y < texture.height() / 2) || (y > im.height() - texture.height() / 2))) {
		return output; // Return input image if X and Y boundary conditions are not satisfied
	}
	else {
		int start_x = x - floor(texture.width() / 2); // Initialize location of pixel iteration
		int start_y = y - floor(texture.height() / 2);

		int j = 0;
		for (int h = start_y; h < start_y + texture.height(); h++) {    // Iterate and make linear combination of
			int i = 0;                                                  // original and texture image with new colors
			for (int w = start_x; w < start_x + texture.width(); w++) {
				if (h >= 0 && h < im.height() && w >= 0 && w < im.width()) { // Check coordinate is in bounds
					for (int c = 0; c < im.channels(); c++) {
						output(w, h, c) = im.smartAccessor(w, h, c, true) * 
							(1 - texture.smartAccessor(i, j, 0, true)) + color.at(c) * texture.smartAccessor(i, j, 0, true);
					}
					i += 1; // Iterate i,j values as indices for texture image
				}
			}
			j += 1;
		}
	}
	return output;
}


Image singleScalePaint(const Image &im, const Image &out, const Image &texture, int size, int N, float noise) {
	// Create painted rendering by splatting brushstrokes at N random locations in your output image
	// --------- HANDOUT  PS12 ------------------------------
	Image output = out;
	float k = static_cast<float>(size) / static_cast<float>(texture.width()); // Static cast to get float types
	Image texture_s = scaleLin(texture, k);                                   // Scale brush by size

	for (int n = 0; n < N; n++) {
		int random_x = rand() % output.width(); // Use modulo on rand to get random coordinates for brush strokes
		int random_y = rand() % output.height();
		float noise = 1 - noise / (2 + noise * rand());
		vector<float> color = {
			im(random_x, random_y, 0) * noise,  // Make color vector based on original image
			im(random_x, random_y, 1) * noise,
		    im(random_x, random_y, 2) * noise,
		};
		output = brush(output, random_x, random_y, color, texture_s); // Repeated brush cycles and replace output
	}
	return output; // Return output image that has been repeatedly brush-stroked
}


Image singleScalePaintImportance(const Image &im, const Image &importance,
								 const Image &out, const Image &texture, int size, int N, float noise) {
	// Create painted rendering but vary the density of the strokes according to an importance map
	// --------- HANDOUT  PS12 ------------------------------
	Image output = out;
	float k = static_cast<float>(size) / static_cast<float>(texture.width()); // Static cast to get float types
	Image texture_s = scaleLin(texture, k); // Scale brush by size

	int num_brush = 0;
	while (num_brush < N) {
		int random_x = rand() % output.width();  // Get random coordinates for brush stroke
		int random_y = rand() % output.height();

		float prob = static_cast<float>(rand() % 100) / 100.0f; // Get probability 
		if (prob < importance(random_x, random_y, 0)) {
			float noise = 1 - noise / (2 + noise * rand());
			vector<float> color = {
				im(random_x, random_y, 0) * noise, // Make color vector based on original image
				im(random_x, random_y, 1) * noise,
		    	im(random_x, random_y, 2) * noise,
			};
			output = brush(output, random_x, random_y, color, texture_s); // Repeated brush cycles and replace output
			num_brush += 1;
		}
	}
	return output;
}


Image sharpnessMap(const Image &im, float sigma) {
	// Calculate sharpness mask 
	// --------- HANDOUT  PS12 ------------------------------
	vector<Image> LC = lumiChromi(im);                                  // Get luminance and chrominance of image
	Image blur_lumi = gaussianBlur_separable(LC.at(0), sigma);          // Apply Gaussian blur
	Image high_freq = LC.at(0) - blur_lumi;                             // Subtract to get high frequency
	Image high_freq_sq = high_freq * high_freq;                         // Square the high frequency
	Image blur_hf_sq = gaussianBlur_separable(high_freq_sq, 4 * sigma); // Apply second Gaussian blur
	Image sharpness_map = blur_hf_sq / blur_hf_sq.max();                // Divide by maximum
	return sharpness_map;
}


Image painterly(const Image &im, const Image &texture, int N, int size, int noise) {
	// Create painterly rendering using a first layer of coarse strokes followed by smaller strokes in high detail areas
	// --------- HANDOUT  PS12 ------------------------------
	Image output(im.width(), im.height(), im.channels());
	Image first_pass = singleScalePaint(im, output, texture, size, N, noise); // First pass large strokes
	Image importance = sharpnessMap(im);
	Image second_pass = singleScalePaintImportance(im, importance, first_pass, texture, int(size/4), N, noise); // Small strokes
	return second_pass;
}


Image gradientX(const Image &im, bool clamp) { // Helper function taken from PS07
  Filter sobelX(3, 3);
  sobelX(0, 0) = -1.0;
  sobelX(1, 0) = 0.0;
  sobelX(2, 0) = 1.0;
  sobelX(0, 1) = -2.0;
  sobelX(1, 1) = 0.0;
  sobelX(2, 1) = 2.0;
  sobelX(0, 2) = -1.0;
  sobelX(1, 2) = 0.0;
  sobelX(2, 2) = 1.0;

  Image imSobelX = sobelX.convolve(im, clamp);
  return imSobelX;
}


Image gradientY(const Image &im, bool clamp) { // Helper function taken from PS07
  // sobel filtering in y direction
  Filter sobelY(3, 3);
  sobelY(0, 0) = -1.0;
  sobelY(1, 0) = -2.0;
  sobelY(2, 0) = -1.0;
  sobelY(0, 1) = 0.0;
  sobelY(1, 1) = 0.0;
  sobelY(2, 1) = 0.0;
  sobelY(0, 2) = 1.0;
  sobelY(1, 2) = 2.0;
  sobelY(2, 2) = 1.0;

  Image imSobelY = sobelY.convolve(im, clamp);
  return imSobelY;
}


Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
 	// Compute xx/xy/yy Tensor of an image. (stored in that order)
 	// --------- HANDOUT  PS07 ------------------------------
 	vector<Image> lumi_chromi = lumiChromi(im); // Get luminance/chrominance of input image
  	Image blur_lumi = gaussianBlur_separable(lumi_chromi.at(0), sigmaG); // Do 2D Gaussian Blue
  	Image blur_lumi_gradX = gradientX(blur_lumi, true);
	Image blur_lumi_gradY = gradientY(blur_lumi, true); // Gradients in each direction

  	Image tensor(im.width(), im.height(), 3); // Setup output 3 channel image
  	for (int h = 0; h < im.height(); h++) {
    	for (int w = 0; w < im.width(); w++) {
      		tensor(w, h, 0) = pow(blur_lumi_gradX(w, h, 0), 2);                    // I_x^2
      		tensor(w, h, 1) = blur_lumi_gradX(w, h, 0) * blur_lumi_gradY(w, h, 0); // I_x * I_y
      		tensor(w, h, 2) = pow(blur_lumi_gradY(w, h, 0), 2);                    // I_y^2
    	}
  	}
  	return gaussianBlur_separable(tensor, sigmaG * factorSigma); // Apply weighting function as a Gaussian
}


Image testAngle(const Image &im, float sigmaG, float factor) {
	// Extracts orientation of features in im. Angles should be mapped to [0,1]
	// --------- HANDOUT  PS12 ------------------------------
	Image tensor = computeTensor(im, sigmaG, factor);     // Call compute tensor from the previous PSET
	Image output(im.width(), im.height(), im.channels());

	for (int h = 0; h < im.height(); h++) {    // Iterate over all pixels to get min. angles for each one
    	for (int w = 0; w < im.width(); w++) {
			Matrix I(2, 2);
			I << tensor(w, h, 0), tensor(w, h, 1), tensor(w, h, 1), tensor(w, h, 2); // Fill input for eigensolver
			Eigen::EigenSolver<Matrix> eigensolver;                                  // Solve matrix
			eigensolver.compute(I);
			Matrix eigenvectors = eigensolver.eigenvectors().real();                 // Get real part of eigenvectors
			float angle1 = atan2(eigenvectors(0, 1), eigenvectors(0, 0));            // Compute angle with both vectors
			float angle2 = atan2(eigenvectors(1, 1), eigenvectors(1, 0));            // y_2', x_2'

			if (abs(angle1) < abs(angle2)) {                                         // Get min angle for all channels
				for (int c = 0; c < output.channels(); c++) {                        // Angle 1 < Angle 2 case
					output(w, h, c) = (angle1 + 3.14159265f) / (2 * 3.14159265f);    // Scale [0,2pi] to [0,1]
				}
			}
			else {
				for (int c = 0; c < output.channels(); c++) {                        // Angle 2 < Angle 1 case
					output(w, h, c) = (angle2 + 3.14159265f) / (2 * 3.14159265f);
				}
			}	
		}
	}
    return output; // Return output angles
}


vector<Image> rotateBrushes(const Image &im, int nAngles) {
	// helper function, Returns list of nAngles images rotated by 1*2pi/nAngles
	// --------- HANDOUT  PS12 ------------------------------
	vector<Image> output;
	for (int i = 0; i < nAngles; i++) { // from 0 to nAngles-1, nAngles = 2pi so we don't need to repeat it
		output.push_back(
			rotate(im, static_cast<float>(i) * 2.0f * 3.1415926535f / static_cast<float>(nAngles))
		);
	}
	return output; // Vector of Image objects
}


Image singleScaleOrientedPaint(const Image &im, const Image &importance, const Image &out, const Image &tensor, 
		                       const Image &texture, int size, int N, float noise, int nAngles) {
	// Similar to singleScalePaintImportant but brush strokes are oriented according to tensor
	// --------- HANDOUT  PS12 ------------------------------
	Image output = out;
	float k = static_cast<float>(size) / static_cast<float>(texture.width()); // Static cast to get float types
	Image texture_s = scaleLin(texture, k);                                   // Scale brush by size
	int num_brush = 0;

	vector<float> angles;                                                     // Get vector of possible angles
	for (int j = 0; j < nAngles; j++) {
		angles.push_back(static_cast<float>(j) * 2.0f * 3.14159265f / static_cast<float>(nAngles));
	}
	vector<Image> brushes = rotateBrushes(texture_s, nAngles);                // Get all brush rotations

	while (num_brush < N) {
		int random_x = rand() % output.width();                               // Get random coordinates for brush stroke
		int random_y = rand() % output.height();

		float prob = static_cast<float>(rand() % 100) / 100.0f;               // Get probability 
		if (prob < importance(random_x, random_y, 0)) {
			float angle = 2.0f * 3.14159265f * tensor(random_x, random_y, 0) + 3.14159265f/2; // Get angle at coordinate
			float dist = abs(angle - angles.at(0));                           // Set temporary closest angle
			int index = 0;
			for (int i = 1; i < nAngles; i++) {                               // Check all possible angles
				float temp_dist = abs(angle - angles.at(i));                  // Store and keep index of nearest angle  
				if (temp_dist < dist) {
					index = i;
					dist = temp_dist;
				}
			}                                                                 // After for loop, now have closest angle

			float noise = 1 - noise / (2 + noise * rand());
			vector<float> color = {
				im(random_x, random_y, 0) * noise,                            // Make color vector based on original image
				im(random_x, random_y, 1) * noise,
		        im(random_x, random_y, 2) * noise,
			};

			output = brush(output, random_x, random_y, color, brushes.at(index)); // Use rotated brush
			num_brush += 1;
		}
	}
	return output;
}


Image orientedPaint(const Image &im, const Image &texture, int N, int size, int noise) {
	// Similar to painterly() but strokes are oriented along the directions of maximal structure
	// --------- HANDOUT  PS12 ------------------------------
	Image output(im.width(), im.height(), im.channels());  // Initialize output image
	Image equal(im.width(), im.height(), 1);               // Make importance map that is equal everywhere
	equal.set_color(1.0f);
	Image tensor = testAngle(im);                          // Get tensors to reference in SSOP call                          
	Image importance = sharpnessMap(im);                   // Get importance to reference in SSOP 2nd Pass

	Image first_pass = singleScaleOrientedPaint(im, equal, output, tensor, texture, size, N, noise);
	Image second_pass = singleScaleOrientedPaint(im, importance, first_pass, tensor, texture, int(size/4), N, noise);
	return second_pass;
}
