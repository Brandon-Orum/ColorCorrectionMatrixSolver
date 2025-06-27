#include "color_correction_matrix_solver.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

ColorCorrectionMatrix ColorCorrectionMatrixSolver::Solve(const ImageData& startImage, const ImageData& targetImage) {
	// Check that images have the same dimensions and are RGB
	if (startImage.width != targetImage.width || startImage.height != targetImage.height) {
		throw std::invalid_argument("Images have different widths: start=" + std::to_string(startImage.width) + 
									", target=" + std::to_string(targetImage.width));
	}
	
	if (startImage.height != targetImage.height) {
		throw std::invalid_argument("Images have different heights: start=" + std::to_string(startImage.height) + 
									", target=" + std::to_string(targetImage.height));
	}
	
	if (startImage.channels != targetImage.channels) {
		throw std::invalid_argument("Images have different number of channels: start=" + std::to_string(startImage.channels) + 
									", target=" + std::to_string(targetImage.channels));
	}
	
	if (startImage.channels != 3) {
		throw std::invalid_argument("Images must be RGB (3 channels), but have " + std::to_string(startImage.channels) + " channels");
	}

	double m_r[3] = { 1.0, 0.0, 0.0 };
	double m_g[3] = { 0.0, 1.0, 0.0 };
	double m_b[3] = { 0.0, 0.0, 1.0 };

	ceres::Problem problem;

	// Iterate through all pixels in the images
	const unsigned char* startData = startImage.data.get();
	const unsigned char* targetData = targetImage.data.get();
	
	for (int y = 0; y < startImage.height; ++y) {
		for (int x = 0; x < startImage.width; ++x) {
			// Calculate pixel index
			int pixelIndex = (y * startImage.width + x) * startImage.channels;
			
			// Extract RGB values and normalize to [0, 1] range
			double startR = startData[pixelIndex] / 255.0;
			double startG = startData[pixelIndex + 1] / 255.0;
			double startB = startData[pixelIndex + 2] / 255.0;
			
			double targetR = targetData[pixelIndex] / 255.0;
			double targetG = targetData[pixelIndex + 1] / 255.0;
			double targetB = targetData[pixelIndex + 2] / 255.0;
			
			// Create Eigen vectors for this pixel
			Eigen::Vector3d start(startR, startG, startB);
			Eigen::Vector3d target(targetR, targetG, targetB);
			
			// Create cost function for this pixel
			ceres::CostFunction* cost_function =
				new ceres::AutoDiffCostFunction<CostFunctor, 3, 3, 3, 3>(new CostFunctor(start, target));
			problem.AddResidualBlock(cost_function, nullptr, m_r, m_g, m_b);
		}
	}

	ceres::Solver::Options options;
	options.minimizer_progress_to_stdout = true;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << summary.BriefReport() << "\n";

	// Construct the result matrix from the optimized parameters
	ColorCorrectionMatrix result;
	result.matrix << m_r[0], m_g[0], m_b[0],
					 m_r[1], m_g[1], m_b[1],
					 m_r[2], m_g[2], m_b[2];
	
	return result;
}

ImageData ColorCorrectionMatrixSolver::ApplyMatrix(const ImageData& inputImage, const ColorCorrectionMatrix& correctionMatrix) {
    // Validate input image
    if (!inputImage.isValid()) {
        throw std::invalid_argument("Input image is not valid");
    }
    
    if (inputImage.channels != 3) {
        throw std::invalid_argument("Image must be RGB (3 channels), but has " + std::to_string(inputImage.channels) + " channels");
    }
    
    // Create output image with same dimensions
    ImageData outputImage;
    outputImage.width = inputImage.width;
    outputImage.height = inputImage.height;
    outputImage.channels = inputImage.channels;
    
    // Allocate memory for output image data
    int totalPixels = inputImage.width * inputImage.height * inputImage.channels;
    unsigned char* outputData = new unsigned char[totalPixels];
    outputImage.data = std::unique_ptr<unsigned char, void(*)(void*)>(outputData, [](void* ptr) { delete[] static_cast<unsigned char*>(ptr); });
    
    const unsigned char* inputData = inputImage.data.get();
    
    // Process each pixel
    for (int y = 0; y < inputImage.height; ++y) {
        for (int x = 0; x < inputImage.width; ++x) {
            // Calculate pixel index
            int pixelIndex = (y * inputImage.width + x) * inputImage.channels;
            
            // Extract RGB values and normalize to [0, 1] range
            double r = inputData[pixelIndex] / 255.0;
            double g = inputData[pixelIndex + 1] / 255.0;
            double b = inputData[pixelIndex + 2] / 255.0;
            
            // Create input color vector
            Eigen::Vector3d inputColor(r, g, b);
            
            // Apply color correction matrix
            Eigen::Vector3d correctedColor = correctionMatrix.apply(inputColor);
            
            // Clamp values to [0, 1] range
            correctedColor[0] = std::max(0.0, std::min(1.0, correctedColor[0]));
            correctedColor[1] = std::max(0.0, std::min(1.0, correctedColor[1]));
            correctedColor[2] = std::max(0.0, std::min(1.0, correctedColor[2]));
            
            // Convert back to [0, 255] range and store in output
            outputData[pixelIndex] = static_cast<unsigned char>(std::round(correctedColor[0] * 255.0));
            outputData[pixelIndex + 1] = static_cast<unsigned char>(std::round(correctedColor[1] * 255.0));
            outputData[pixelIndex + 2] = static_cast<unsigned char>(std::round(correctedColor[2] * 255.0));
        }
    }
    
    return outputImage;
}