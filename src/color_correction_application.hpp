#pragma once

#include <string>
#include <optional>
#include "image_file_handler.hpp"
#include "color_correction_matrix_solver.hpp"

class ColorCorrectionApplication {
public:
    struct Arguments {
        std::string startImagePath;
        std::string targetImagePath;
        std::optional<std::string> outputImagePath;
    };

    // Parse command line arguments
    static std::optional<Arguments> parseArguments(int argc, char* argv[]);
    
    // Run the complete color correction workflow
    int run(const Arguments& args);
    
    // Print usage information
    void printUsage(const char* programName);

private:
    bool loadAndValidateImages(const Arguments& args, ImageData& startImage, ImageData& targetImage);
    bool solveColorCorrectionMatrix(const ImageData& startImage, const ImageData& targetImage, ColorCorrectionMatrix& matrix);
    bool applyCorrectionAndSave(const ImageData& startImage, const ColorCorrectionMatrix& matrix, const std::string& outputPath);
};