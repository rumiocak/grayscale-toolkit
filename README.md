# ClearVision 🔍
A C++-based image processing application developed for BBM203: Software Practicum I at Hacettepe University.

## About the Project
This project implements **ClearVision Defense** — an image enhancement and steganography system. The application applies denoising and sharpening filters to grayscale images, and supports hiding/revealing secret messages within pixel data using LSB steganography.

## Features
- Three image filters: **Mean (Box)**, **Gaussian Smoothing**, **Unsharp Masking**
- Pixel-wise image addition, subtraction, and equality comparison
- Secure image storage by splitting into upper/lower triangular matrices
- Disguise and reveal images via `.dat` file format
- Embed and extract secret ASCII messages using least significant bits (LSBs)
- Robust error handling and edge padding for boundary pixels

## Technologies
- C++11
- Object-Oriented Programming (Abstraction, Encapsulation, Inheritance, Operator Overloading)
- `stb_image.h` single-header library for PNG/JPG I/O
- Dynamic memory allocation

## How to Run
```bash
g++ -std=c++11 -o clearvision main.cpp GrayscaleImage.cpp SecretImage.cpp Filter.cpp Crypto.cpp
./clearvision mean input.png 3
```

## Input Commands
| Command | Description |
|---------|-------------|
| `mean <img> <kernel_size>` | Apply mean filter |
| `gauss <img> <kernel_size> <sigma>` | Apply Gaussian smoothing |
| `unsharp <img> <kernel_size> <amount>` | Apply unsharp mask |
| `add <img1> <img2>` | Add two images pixel-wise |
| `sub <img1> <img2>` | Subtract two images pixel-wise |
| `equals <img1> <img2>` | Check if two images are identical |
| `disguise <img>` | Split image into triangular matrices and save as `.dat` |
| `reveal <dat>` | Reconstruct image from `.dat` file |
| `enc <img> <message>` | Embed secret message into image LSBs |
| `dec <img> <msg_len>` | Extract hidden message from image |

## Course
**BBM203** - Software Practicum I  
Hacettepe University, Fall 2024
