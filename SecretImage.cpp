#include "SecretImage.h"

// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
    width = image.get_width();
    height = image.get_height();

    // Allocate memory for the upper and lower triangular matrices
    int upper_size = calculate_upper_size(width);
    int lower_size = calculate_lower_size(width);

    upper_triangular = new int[upper_size]; // Allocate memory for upper triangular matrix
    lower_triangular = new int[lower_size]; // Allocate memory for lower triangular matrix

    int upper_index = 0; // Index for upper triangular matrix
    int lower_index = 0; // Index for lower triangular matrix

    // Loop through each pixel in the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixel_value = image.get_pixel(i, j); // Get pixel value

            // Store pixel values in respective triangular matrices
            if (i <= j) {
                if (upper_index < upper_size) {
                    upper_triangular[upper_index++] = pixel_value; // Store in upper matrix
                } else {
                    throw std::runtime_error("Upper triangular matrix overflow.");
                }
            } else {
                if (lower_index < lower_size) {
                    lower_triangular[lower_index++] = pixel_value; // Store in lower matrix
                } else {
                    throw std::runtime_error("Lower triangular matrix overflow.");
                }
            }
        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int w, int h, int* upper, int* lower) {
    width = w;  // Set the width
    height = h; // Set the height

    // Allocate memory for the upper and lower triangular matrices
    int upper_size = calculate_upper_size(width);
    int lower_size = calculate_lower_size(width);

    upper_triangular = new int[upper_size]; // Allocate memory for the upper triangular matrix
    lower_triangular = new int[lower_size]; // Allocate memory for the lower triangular matrix

    // Copy data from parameters to instance variables
    for (int i = 0; i < upper_size; ++i) {
        upper_triangular[i] = upper[i]; // Copy upper triangle data
    }

    for (int i = 0; i < lower_size; ++i) {
        lower_triangular[i] = lower[i]; // Copy lower triangle data
    }
}

// Destructor: frees the dynamically allocated arrays
SecretImage::~SecretImage() {
    // Free the memory for the upper and lower triangular matrices
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height); // Create a new image with original dimensions

    int upper_index = 0; // Index for upper triangular matrix
    int lower_index = 0; // Index for lower triangular matrix

    // Loop through each pixel to reconstruct the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i <= j) {
                // Assign pixel value from the upper triangle (including diagonal)
                image.set_pixel(i, j, upper_triangular[upper_index++]);
            } else {
                // Assign pixel value from the lower triangle (excluding diagonal)
                image.set_pixel(i, j, lower_triangular[lower_index++]); // Lower triangle pixel
            }
        }
    }

    return image; // Return the reconstructed image
}

// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {
    // Update the lower and upper triangular matrices based on the provided GrayscaleImage

    int upper_index = 0; // Initialize index for the upper triangular matrix
    int lower_index = 0; // Initialize index for the lower triangular matrix

    // Loop through each pixel to update the triangular arrays
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Check if the pixel is in the upper or lower triangle
            if (i <= j) {
                upper_triangular[upper_index++] = image.get_pixel(i, j); // Save pixel value in the upper triangular matrix
            } else {
                lower_triangular[lower_index++] = image.get_pixel(i, j); // Save pixel value in the lower triangular matrix
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Could not open file for writing");
    }

    out << width << " " << height << std::endl; // Write the image dimensions

    int upper_size = calculate_upper_size(width); // Get the size of the upper triangular matrix
    int lower_size = calculate_lower_size(width); // Get the size of the lower triangular matrix

    for (int i = 0; i < upper_size; ++i) {
        out << upper_triangular[i]; // Write each element of the upper triangular array
        if (i < upper_size - 1) { // Add space if not the last element
            out << " ";
        }
    }
    out << std::endl;

    for (int i = 0; i < lower_size; ++i) {
        out << lower_triangular[i]; // Write each element of the lower triangular array
        if (i < lower_size - 1) { // Add space if not the last element
            out << " ";
        }
    }
    out << std::endl;

    out.close(); // Close the file stream
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file"); // Error if file can not be opened
    }

    int w, h;
    file >> w >> h;  // Read width and height from the file
    if (w <= 0 || h <= 0) {
        throw std::runtime_error("Invalid image dimensions read from file.");
    }

    int upper_size = calculate_upper_size(w); // Calculate the size of the upper triangular array
    int lower_size = calculate_lower_size(w); // Calculate the size of the lower triangular array

    // Allocate memory for both triangular arrays
    int* upper = new int[upper_size];
    int* lower = new int[lower_size];

    // Read the upper triangular array from the file
    for (int i = 0; i < upper_size; ++i) {
        if (!(file >> upper[i])) {
            delete[] upper; // Free allocated memory if read fails
            delete[] lower; // Free allocated memory if read fails
            throw std::runtime_error("Failed to read upper triangular data from file."); // Error if reading fails
        }
    }

    // Read the lower triangular array from the file
    for (int i = 0; i < lower_size; ++i) {
        if (!(file >> lower[i])) {
            delete[] upper; // Free allocated memory if read fails
            delete[] lower; // Free allocated memory if read fails
            throw std::runtime_error("Failed to read lower triangular data from file."); // Error if reading fails
        }
    }

    file.close();  // Close the file after reading

    // Return a SecretImage object initialized with the loaded data
    SecretImage secret_image(w, h, upper, lower);

    // Cleanup: Free dynamically allocated arrays
    delete[] upper;
    delete[] lower;

    return secret_image;
}

// Function to calculate the size of the upper triangle
int SecretImage::calculate_upper_size(int width) {
    return (width * (width + 1)) / 2;
}

// Function to calculate the size of the lower triangle
int SecretImage::calculate_lower_size(int width) {
    return (width * (width - 1)) / 2;
}

// Returns a pointer to the upper triangular part of the secret image.
int * SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int * SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}