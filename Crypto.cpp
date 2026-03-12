#include "Crypto.h"
#include "GrayscaleImage.h"

// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array; // Vector to store the least significant bits
    GrayscaleImage grayscale_image = secret_image.reconstruct(); // Reconstruct the grayscale image

    int width = grayscale_image.get_width();
    int height = grayscale_image.get_height();
    int total_pixels = width * height; // Calculate total number of pixels

    int total_message_bits = message_length * 7; // Calculate required bits for the message

    // Check if the image has enough pixels to hide the message
    if (total_pixels < total_message_bits) {
        throw std::runtime_error("There are not enough pixels available in the image to hide the message.");
    }

    int starting_pixel = total_pixels - total_message_bits; // Start from the last bits needed

    // Extract the least significant bits from the pixels
    for (int i = starting_pixel; i < total_pixels; ++i) {
        int pixel_value = grayscale_image.get_pixel(i / width, i % width);
        LSB_array.emplace_back(pixel_value & 1); // Get the least significant bit
    }

    return LSB_array; // Return the array of least significant bits
}

// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message; // Variable to hold the final decrypted message

    // Ensure the LSB array size is a multiple of 7 for proper ASCII conversion
    if (LSB_array.size() % 7 != 0) {
        throw std::runtime_error("The size of the LSB array must be a multiple of 7.");
    }

    // Process the LSB array in chunks of 7 bits to form ASCII characters
    for (int i = 0; i < LSB_array.size(); i += 7) {
        int ascii_value = 0; // Initialize to store the ASCII value for the character

        // Calculate the ASCII value from the 7 bits
        for (int j = 0; j < 7; ++j) {
            ascii_value += LSB_array[i + j] * (1 << (6 - j)); // Use bit shifting to form the ASCII value
        }

        // Convert the ASCII value to a character and add it to the message
        message += static_cast<char>(ascii_value);
    }

    return message; // Return the complete decrypted message
}

// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array; // Vector to store the least significant bits (LSBs)

    // Convert each character in the message to a 7-bit binary representation
    for (char c : message) {
        std::bitset<7> bits(static_cast<unsigned char>(c)); // Create a 7-bit binary representation of the character

        // Collect the bits into the LSB array in reverse order
        for (int i = 0; i < 7; ++i) {
            LSB_array.emplace_back(bits[6 - i]); // Add bits starting from the least significant bit
        }
    }

    return LSB_array; // Return the complete LSB array representing the message
}

// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {
    int width = image.get_width();
    int height = image.get_height();
    int total_pixels = width * height; // Calculate the total number of pixels in the image

    // Check if the image has enough pixels to embed the LSB array
    if (total_pixels < LSB_array.size()) {
        throw std::runtime_error("There are not enough pixels in the image to embed the LSB array.");
    }

    // Determine the starting pixel index for embedding the LSB array
    int starting_pixel = total_pixels - LSB_array.size();

    // Loop through the LSB array and embed each bit into the image
    for (int i = 0; i < LSB_array.size(); ++i) {
        int row = (starting_pixel + i) / width; // Calculate the row for the current pixel
        int column = (starting_pixel + i) % width; // Calculate the column for the current pixel
        int pixel_value = image.get_pixel(row, column); // Get the current pixel value

        // Check if pixel value is within valid range
        if (pixel_value < 255 || LSB_array[i] == 0) {
            // Update the least significant bit (LSB)
            if (LSB_array[i] == 1) {
                pixel_value |= 1; // Set the LSB to 1
            } else {
                pixel_value &= ~1; // Set the LSB to 0
            }
        }

        image.set_pixel(row, column, pixel_value); // Write the updated pixel value back to the image
    }

    SecretImage secret_image(image); // Create a SecretImage object from the modified image

    return secret_image; // Return the updated secret image
}