#pragma once

#include <memory>
#include <string>
#include <image_data.hpp>

class ImageFileHandler {
public:
    static ImageData loadImage(const std::string& imagePath);
    static bool saveImage(const ImageData& image, const std::string& imagePath);
    static void printImageInfo(const ImageData& image, const std::string& imagePath);
};