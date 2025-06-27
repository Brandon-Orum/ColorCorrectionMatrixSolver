#include "image_file_handler.hpp"
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageData ImageFileHandler::loadImage(const std::string& imagePath) {
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

bool ImageFileHandler::saveImage(const ImageData& image, const std::string& imagePath) {
    if (!image.isValid()) {
        std::cerr << "Error: Cannot save invalid image data to '" << imagePath << "'" << std::endl;
        return false;
    }
    
    // Determine file format from extension
    std::string extension = imagePath.substr(imagePath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    int result = 0;
    
    if (extension == "png") {
        result = stbi_write_png(imagePath.c_str(), image.width, image.height, 
                               image.channels, image.data.get(), 
                               image.width * image.channels);
    }
    else if (extension == "jpg" || extension == "jpeg") {
        result = stbi_write_jpg(imagePath.c_str(), image.width, image.height, 
                               image.channels, image.data.get(), 90); // 90% quality
    }
    else if (extension == "bmp") {
        result = stbi_write_bmp(imagePath.c_str(), image.width, image.height, 
                               image.channels, image.data.get());
    }
    else if (extension == "tga") {
        result = stbi_write_tga(imagePath.c_str(), image.width, image.height, 
                               image.channels, image.data.get());
    }
    else {
        std::cerr << "Error: Unsupported file format '" << extension << "' for '" << imagePath << "'" << std::endl;
        std::cerr << "Supported formats: PNG, JPG/JPEG, BMP, TGA" << std::endl;
        return false;
    }
    
    if (!result) {
        std::cerr << "Error: Failed to save image to '" << imagePath << "'" << std::endl;
        return false;
    }
    
    std::cout << "Successfully saved image to: " << imagePath << std::endl;
    return true;
}

void ImageFileHandler::printImageInfo(const ImageData& image, const std::string& imagePath) {
    if (!image.isValid()) {
        std::cerr << "Error: Invalid image data for '" << imagePath << "'" << std::endl;
        return;
    }

    std::cout << "Successfully loaded image: " << imagePath << std::endl;
    std::cout << "  Dimensions: " << image.width << "x" << image.height << " pixels" << std::endl;
    std::cout << "  Channels: " << image.channels << std::endl;
}