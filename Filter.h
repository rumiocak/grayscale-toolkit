#ifndef FILTER_H
#define FILTER_H

#include "GrayscaleImage.h"

class Filter {
public:
    // Apply the Mean Filter
    static void apply_mean_filter(GrayscaleImage& image, int kernelSize = 3);

    // Apply Gaussian Smoothing Filter
    static void apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize = 3, double sigma = 1.0);

    // Apply Unsharp Masking Filter
    static void apply_unsharp_mask(GrayscaleImage& image, int kernelSize = 3, double amount = 1.5);

    // Utility functions for matrix allocation and deallocation
    static int** allocate_matrix(int height, int width);
    static void free_matrix(int** matrix, int height);
};

#endif // FILTER_H
