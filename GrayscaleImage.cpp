#include "GrayscaleImage.h"
#include <iostream>
#include <cstring> // For memcpy
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdexcept>

// Constructor: load from a file
GrayscaleImage::GrayscaleImage(const char *filename)
{
    // Image loading code using stbi
    int channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_grey);

    if (image == nullptr)
    {
        std::cerr << "Error: Could not load image " << filename << std::endl;
        exit(1);
    }

    // Dynamically allocate memory for a 2D matrix based on the given dimensions
    data = new int *[height]; // Create an array of pointers for each row
    for (int i = 0; i < height; ++i)
    {
        data[i] = new int[width]; // Allocate memory for each row
    }

    // Fill the matrix with pixel values from the image
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // Convert the unsigned char pixel value to int and store it in the matrix
            data[i][j] = static_cast<int>(image[i * width + j]);
        }
    }

    // Free the dynamically allocated memory of stbi image
    stbi_image_free(image);
}

// Constructor: initialize from a pre-existing data matrix
GrayscaleImage::GrayscaleImage(int **inputData, int h, int w)
{
    height = h; // Set the height of the image
    width = w;  // Set the width of the image

    // Dynamically allocate memory for a 2D matrix
    data = new int *[height]; // Create an array of pointers for each row
    for (int i = 0; i < height; ++i)
    {
        data[i] = new int[width]; // Allocate memory for each row
    }

    // Initialize the image with values from the provided data matrix
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            data[i][j] = inputData[i][j];
        }
    }
}

// Constructor to create a blank image of given width and height
GrayscaleImage::GrayscaleImage(int w, int h) : width(w), height(h) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Width and height must be positive integers.");
    }

    // Dynamically allocate memory for a 2D matrix
    data = new int *[height]; // Create an array of pointers for each row
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width]; // Allocate memory for each row
    }

    // Initialize all pixel values in the matrix to 0 (creating a blank image)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] = 0; // Set each pixel value to 0, representing black in grayscale
        }
    }
}

// Copy constructor
GrayscaleImage::GrayscaleImage(const GrayscaleImage &other)
{
    // Copy the width and height from the other image
    width = other.width;
    height = other.height;

    // Dynamically allocate memory for a 2D matrix
    data = new int *[height]; // Create an array of pointers for each row
    for (int i = 0; i < height; ++i)
    {
        data[i] = new int[width]; // Allocate memory for each row
    }

    // Copy the pixel values from the other image into the new matrix
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            data[i][j] = other.data[i][j];
        }
    }
}

// Destructor
GrayscaleImage::~GrayscaleImage()
{
    // Destructor: deallocate memory for the matrix
    for (int i = 0; i < height; ++i)
    {
        delete[] data[i];
    }
    delete[] data;
}

// Equality operator
bool GrayscaleImage::operator==(const GrayscaleImage &other) const
{
    // Check if two images have the same dimensions and pixel values.
    // If they do, return true.
    if (width == other.width && height == other.height)
    {
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                if (data[i][j] != other.data[i][j])
                    return false;
            }
        }
        return true;
    }

    return false;
}

// Addition operator
GrayscaleImage GrayscaleImage::operator+(const GrayscaleImage &other) const
{
    // Create a new image for the result
    GrayscaleImage result(width, height);

    // Iterate through each pixel to add the corresponding pixel values from both images
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // Calculate the sum of the pixel values from both images
            int sum = data[i][j] + other.data[i][j];
            // Ensure the result does not exceed the maximum value for a pixel (255)
            if (sum > 255)
            {
                sum = 255; // Clamp the value to 255 if it exceeds
            }
            result.data[i][j] = sum; // Assign the clamped sum to the corresponding pixel in the result image
        }
    }

    return result;
}

// Subtraction operator
GrayscaleImage GrayscaleImage::operator-(const GrayscaleImage &other) const
{
    // Create a new image for the result
    GrayscaleImage result(width, height);

    // Iterate through each pixel to subtract the corresponding pixel values from the two images
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate the difference between the pixel values
            int difference = data[i][j] - other.data[i][j];
            // Ensure the result does not go below the minimum value for a pixel (0)
            if (difference < 0)
            {
                difference = 0; // Clamp the value to 0 if it becomes negative
            }
            result.data[i][j] = difference; // Assign the clamped difference to the corresponding pixel in the result image
        }
    }

    return result;
}

// Get a specific pixel value
int GrayscaleImage::get_pixel(int row, int col) const
{
    return data[row][col];
}

// Set a specific pixel value
void GrayscaleImage::set_pixel(int row, int col, int value)
{
    data[row][col] = value;
}

// Function to save the image to a PNG file
void GrayscaleImage::save_to_file(const char *filename) const
{
    // Create a buffer to hold the image data in the format stb_image_write expects
    unsigned char *imageBuffer = new unsigned char[width * height];

    // Fill the buffer with pixel data (convert int to unsigned char)
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            imageBuffer[i * width + j] = static_cast<unsigned char>(data[i][j]);
        }
    }

    // Write the buffer to a PNG file
    if (!stbi_write_png(filename, width, height, 1, imageBuffer, width))
    {
        std::cerr << "Error: Could not save image to file " << filename << std::endl;
    }

    // Clean up the allocated buffer
    delete[] imageBuffer;
}
