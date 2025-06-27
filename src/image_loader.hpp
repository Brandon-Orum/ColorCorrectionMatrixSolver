#pragma once

#include <memory>
#include <string>
#include <image_data.hpp>

class ImageLoader {
public:
    static ImageData loadImage(const std::string& imagePath);
    static void printImageInfo(const ImageData& image, const std::string& imagePath);
};