#include <iostream>
#include <stdexcept>
#include "image_loader.hpp"
#include "color_correction_matrix_solver.hpp"

int main(int argc, char* argv[])
{
    // Check for correct number of arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <start_image_path> <target_image_path>" << std::endl;
        return -1;
    }

    const char* startImagePath = argv[1];
    const char* targetImagePath = argv[2];

    // Load start image
    ImageData startImage = ImageLoader::loadImage(startImagePath);
    if (!startImage.isValid()) {
        return -1;
    }
    ImageLoader::printImageInfo(startImage, startImagePath);

    // Load target image
    ImageData targetImage = ImageLoader::loadImage(targetImagePath);
    if (!targetImage.isValid()) {
        return -1;
    }
    ImageLoader::printImageInfo(targetImage, targetImagePath);

    if (startImage.width != targetImage.width || startImage.height != targetImage.height) {
        std::cerr << "Error: Images must have the same dimensions." << std::endl;
		return -1;
    }

    ColorCorrectionMatrixSolver solver;
    ColorCorrectionMatrix colorCorrectionMatrix = solver.Solve(startImage, targetImage);

    std::cout << "Color correction matrix solved successfully!" << std::endl;
    std::cout << "Matrix:" << std::endl;
    std::cout << colorCorrectionMatrix.matrix << std::endl;
}