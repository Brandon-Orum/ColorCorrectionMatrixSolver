#pragma once

#include <ceres/ceres.h>
#include <Eigen/Dense>
#include <image_data.hpp>

// Represents a 3x3 color correction matrix
struct ColorCorrectionMatrix {
    Eigen::Matrix3d matrix;

    ColorCorrectionMatrix()
        : matrix(Eigen::Matrix3d::Identity()) {}

    ColorCorrectionMatrix(const Eigen::Matrix3d& m, const Eigen::Vector3d& o)
        : matrix(m) {}

    // Apply the color correction to a color vector (RGB)
    Eigen::Vector3d apply(const Eigen::Vector3d& color) const {
        return matrix * color;
    }
};

class ColorCorrectionMatrixSolver {
public:
    ColorCorrectionMatrix Solve(const ImageData& startImage, const ImageData& targetImage);

private:
    struct CostFunctor {
        Eigen::Vector3d source;
        Eigen::Vector3d target;
        
        CostFunctor(const Eigen::Vector3d& src, const Eigen::Vector3d& tgt)
            : source(src), target(tgt) {}
        
        template <typename T>
        bool operator()(const T* const m_r, const T* const m_g, const T* const m_b, T* residual) const {
            residual[0] = (m_r[0] * source[0]) + (m_g[0] * source[1]) + (m_b[0] * source[2]) - target[0];
            residual[1] = (m_r[1] * source[0]) + (m_g[1] * source[1]) + (m_b[1] * source[2]) - target[1];
            residual[2] = (m_r[2] * source[0]) + (m_g[2] * source[1]) + (m_b[2] * source[2]) - target[2];
            return true;
        }
    };
};