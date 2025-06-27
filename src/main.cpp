#include <iostream>
#include <stdexcept>
#include "image_file_handler.hpp"
#include "color_correction_matrix_solver.hpp"

int main(int argc, char* argv[])
{
    // Check for correct number of arguments
    if (argc != 3 && argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <start_image_path> <target_image_path> [output_image_path]" << std::endl;
        std::cerr << "  If output_image_path is provided, the corrected image will be saved to that path." << std::endl;
        return -1;
    }

    const char* startImagePath = argv[1];
    const char* targetImagePath = argv[2];
    const char* outputImagePath = (argc == 4) ? argv[3] : nullptr;

    // Load start image
    ImageData startImage = ImageFileHandler::loadImage(startImagePath);
    if (!startImage.isValid()) {
        return -1;
    }
    ImageFileHandler::printImageInfo(startImage, startImagePath);

    // Load target image
    ImageData targetImage = ImageFileHandler::loadImage(targetImagePath);
    if (!targetImage.isValid()) {
        return -1;
    }
    ImageFileHandler::printImageInfo(targetImage, targetImagePath);

    if (startImage.width != targetImage.width || startImage.height != targetImage.height) {
        std::cerr << "Error: Images must have the same dimensions." << std::endl;
		return -1;
    }

	std::cout << "\nSolving color correction matrix..." << std::endl;
    ColorCorrectionMatrixSolver solver;
    ColorCorrectionMatrix colorCorrectionMatrix = solver.Solve(startImage, targetImage);

    std::cout << "Color correction matrix solved successfully!" << std::endl;
    std::cout << "Matrix:" << std::endl;
    std::cout << colorCorrectionMatrix.matrix << std::endl;
    
    // Apply the color correction matrix to the start image only if output path is provided
    if (outputImagePath != nullptr) {
        std::cout << "\nApplying color correction matrix to start image..." << std::endl;
        try {
            ImageData correctedImage = ColorCorrectionMatrixSolver::ApplyMatrix(startImage, colorCorrectionMatrix);
            
            // Save the corrected image to the specified path
            if (ImageFileHandler::saveImage(correctedImage, outputImagePath)) {
                std::cout << "Color-corrected image saved as: " << outputImagePath << std::endl;
            } else {
                std::cerr << "Failed to save corrected image to: " << outputImagePath << std::endl;
                return -1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error applying color correction: " << e.what() << std::endl;
            return -1;
        }
    } else {
        std::cout << "\nNo output path specified. Matrix solved but not applied." << std::endl;
    }
    
    return 0;
}