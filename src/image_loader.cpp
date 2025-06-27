#include "image_loader.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageData ImageLoader::loadImage(const std::string& imagePath) {
    ImageData imageData;
    
    // Load image using stb_image
    unsigned char* rawData = stbi_load(imagePath.c_str(), &imageData.width, &imageData.height, &imageData.channels, 0);
    
    if (!rawData) {
        std::cerr << "Error: Failed to load image '" << imagePath << "': " << stbi_failure_reason() << std::endl;
        return imageData; // Return invalid ImageData
    }
    
    // Wrap the raw pointer in a unique_ptr with custom deleter
    imageData.data = std::unique_ptr<unsigned char, void(*)(void*)>(rawData, stbi_image_free);
    
    return imageData;
}

void ImageLoader::printImageInfo(const ImageData& image, const std::string& imagePath) {
    if (!image.isValid()) {
        std::cerr << "Error: Invalid image data for '" << imagePath << "'" << std::endl;
        return;
    }
    
    std::cout << "Successfully loaded image: " << imagePath << std::endl;
    std::cout << "  Dimensions: " << image.width << "x" << image.height << " pixels" << std::endl;
    std::cout << "  Channels: " << image.channels << std::endl;
}