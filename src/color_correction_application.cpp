#include "color_correction_application.hpp"
#include <iostream>

std::optional<ColorCorrectionApplication::Arguments> 
ColorCorrectionApplication::parseArguments(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        return std::nullopt;
    }
    
    Arguments args;
    args.startImagePath = argv[1];
    args.targetImagePath = argv[2];
    if (argc == 4) {
        args.outputImagePath = argv[3];
    }
    
    return args;
}

int ColorCorrectionApplication::run(const Arguments& args) {
    ImageData startImage, targetImage;
    if (!loadAndValidateImages(args, startImage, targetImage)) {
        return -1;
    }
    
    ColorCorrectionMatrix matrix;
    if (!solveColorCorrectionMatrix(startImage, targetImage, matrix)) {
        return -1;
    }
    
    std::cout << "Color correction matrix solved successfully!" << std::endl;
    std::cout << "Matrix:" << std::endl;
    std::cout << matrix.matrix << std::endl;
    
    if (args.outputImagePath.has_value()) {
        if (!applyCorrectionAndSave(startImage, matrix, args.outputImagePath.value())) {
            return -1;
        }
    } else {
        std::cout << "\nNo output path specified. Matrix solved but not applied." << std::endl;
    }
    
    return 0;
}

bool ColorCorrectionApplication::loadAndValidateImages(const Arguments& args, 
                                                      ImageData& startImage, 
                                                      ImageData& targetImage) {
    // Load start image
    startImage = ImageFileHandler::loadImage(args.startImagePath);
    if (!startImage.isValid()) {
        return false;
    }
    ImageFileHandler::printImageInfo(startImage, args.startImagePath);
    
    // Load target image
    targetImage = ImageFileHandler::loadImage(args.targetImagePath);
    if (!targetImage.isValid()) {
        return false;
    }
    ImageFileHandler::printImageInfo(targetImage, args.targetImagePath);
    
    // Validate dimensions
    if (startImage.width != targetImage.width || startImage.height != targetImage.height) {
        std::cerr << "Error: Images must have the same dimensions." << std::endl;
        return false;
    }
    
    return true;
}

bool ColorCorrectionApplication::solveColorCorrectionMatrix(const ImageData& startImage, 
                                                          const ImageData& targetImage, 
                                                          ColorCorrectionMatrix& matrix) {
    std::cout << "\nSolving color correction matrix..." << std::endl;
    
    try {
        ColorCorrectionMatrixSolver solver;
        matrix = solver.Solve(startImage, targetImage);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error solving color correction matrix: " << e.what() << std::endl;
        return false;
    }
}

bool ColorCorrectionApplication::applyCorrectionAndSave(const ImageData& startImage, 
                                                       const ColorCorrectionMatrix& matrix, 
                                                       const std::string& outputPath) {
    std::cout << "\nApplying color correction matrix to start image..." << std::endl;
    
    try {
        ImageData correctedImage = ColorCorrectionMatrixSolver::ApplyMatrix(startImage, matrix);
        
        if (ImageFileHandler::saveImage(correctedImage, outputPath)) {
            std::cout << "Color-corrected image saved as: " << outputPath << std::endl;
            return true;
        } else {
            std::cerr << "Failed to save corrected image to: " << outputPath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error applying color correction: " << e.what() << std::endl;
        return false;
    }
}

void ColorCorrectionApplication::printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <start_image_path> <target_image_path> [output_image_path]" << std::endl;
    std::cerr << "  If output_image_path is provided, the corrected image will be saved to that path." << std::endl;
}