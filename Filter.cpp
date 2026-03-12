#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>

// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {
    int radius = kernelSize / 2; // Calculate the kernel radius

    // Get image dimensions and data
    int height = image.get_height();
    int width = image.get_width();
    int** image_data = image.get_data(); // Pointer to the original image data

    // Allocate memory for the filtered image
    int** new_data = allocate_matrix(height, width);

    // Calculate mean value for each pixel based on neighbors
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int sum = 0;
            int count = kernelSize * kernelSize; // Total pixels in the kernel

            // Sum the values of neighboring pixels
            for (int kernel_i = -radius; kernel_i <= radius; ++kernel_i) {
                for (int kernel_j = -radius; kernel_j <= radius; ++kernel_j) {
                    int neighbor_i = i + kernel_i;
                    int neighbor_j = j + kernel_j;

                    // Treat out-of-bounds pixels as 0
                    if (neighbor_i < 0 || neighbor_i >= height || neighbor_j < 0 || neighbor_j >= width) {
                        sum += 0;
                    } else {
                        sum += image_data[neighbor_i][neighbor_j]; // Add neighbor pixel value
                    }
                }
            }

            // Calculate the mean and store in the new matrix
            new_data[i][j] = sum / count;
        }
    }

    // Update the original image with the filtered data
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image_data[i][j] = new_data[i][j];
        }
    }

    // Free the temporary matrix memory
    free_matrix(new_data, height);
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {
    int radius = kernelSize / 2; // Radius of the kernel

    // Create a Gaussian kernel based on the sigma value
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize, 0));
    double sum = 0.0;

    // Populate the kernel with values using the Gaussian function
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            int x = i - radius; // X distance from the center
            int y = j - radius; // Y distance from the center

            // Calculate the Gaussian value for the kernel element
            kernel[i][j] = (1 / (2 * M_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += kernel[i][j];
        }
    }

    // Normalize the kernel so that its sum equals 1
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;
        }
    }

    // Get image dimensions and data
    int height = image.get_height();
    int width = image.get_width();
    int** image_data = image.get_data();

    // Allocate memory for the new image data
    int** new_data = allocate_matrix(height, width);

    // Apply Gaussian smoothing to each pixel in the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double weighted_sum = 0.0;

            // Apply the Gaussian kernel to the pixel's neighborhood
            for (int kernel_i = -radius; kernel_i <= radius; ++kernel_i) {
                for (int kernel_j = -radius; kernel_j <= radius; ++kernel_j) {
                    int neighbor_i = i + kernel_i; // Row of the neighboring pixel
                    int neighbor_j = j + kernel_j; // Column of the neighboring pixel

                    // Check if the neighboring pixel is within the image bounds
                    if (neighbor_i >= 0 && neighbor_i < height && neighbor_j >= 0 && neighbor_j < width) {
                        weighted_sum += image_data[neighbor_i][neighbor_j] * kernel[kernel_i + radius][kernel_j + radius];
                    }
                }
            }

            // Store the result in the new matrix
            new_data[i][j] = static_cast<int>(weighted_sum);
        }
    }

    // Update the original image with the new filtered data
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image_data[i][j] = new_data[i][j];
        }
    }

    // Free the dynamically allocated memory for the new data
    free_matrix(new_data, height);
}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {
    // Blur the image using Gaussian smoothing with a default sigma of 1.0
    GrayscaleImage blurredImage = image;
    apply_gaussian_smoothing(blurredImage, kernelSize, 1.0);

    // Get the dimensions of the image
    int height = image.get_height();
    int width = image.get_width();
    int** image_data = image.get_data(); // Original image data
    int** blurred_data = blurredImage.get_data(); // Blurred image data

    // Allocate memory for the sharpened image data
    int** new_data = allocate_matrix(height, width);

    // Apply the unsharp mask formula: original + amount * (original - blurred)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int sharpened_pixel = static_cast<int>(image_data[i][j] + amount * (image_data[i][j] - blurred_data[i][j]));

            // Clip the pixel values to the valid range [0-255]
            if (sharpened_pixel < 0) {
                new_data[i][j] = 0;
            } else if (sharpened_pixel > 255) {
                new_data[i][j] = 255;
            } else {
                new_data[i][j] = sharpened_pixel;
            }
        }
    }

    // Update the original image with the sharpened values
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image_data[i][j] = new_data[i][j];
        }
    }

    // Free the dynamically allocated memory for the new data
    free_matrix(new_data, height);
}

// Memory allocation function
int** Filter::allocate_matrix(int height, int width) {
    int** matrix = new int*[height];
    for (int i = 0; i < height; ++i) {
        matrix[i] = new int[width];
    }
    return matrix;
}

// Memory deallocation function
void Filter::free_matrix(int** matrix, int height) {
    for (int i = 0; i < height; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

