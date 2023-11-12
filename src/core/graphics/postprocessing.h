#pragma once

#include <array>
#include <numeric>
#include <spdlog/spdlog.h>

#include <core/graphics/shader.h>

class PostProcessing {
public:
    struct Vignette {
        bool isActive = false;
        float size = .75f;
    };
    struct Kernel {
        bool isActive = false;
        float blend = 1.0f;
        float offset = 1.0f / 300.0f;

        std::array<float, 9> kernel3x3;
        bool useKernel3x3 = false;
        std::array<float, 25> kernel5x5;
        bool useKernel5x5 = false;
        std::array<float, 49> kernel7x7;
        bool useKernel7x7 = false;

        Vignette vignette;
    };
    Vignette vignette;
    glm::vec3 vignetteColor = glm::vec3(0.0f);
    Kernel kernel;

    void ApplyUniforms(const Shader&) const;
    template<size_t S>
    void ApplyKernel(const std::array<float, S>& k) {
        switch(S) {
            case 9:
                kernel.kernel3x3 = reinterpret_cast<const std::array<float, 9>&>(k);
                kernel.useKernel3x3 = true;
                break;
            case 25:
                kernel.kernel5x5 = reinterpret_cast<const std::array<float, 25>&>(k);
                kernel.useKernel5x5 = true;
                break;
            case 49:
                kernel.kernel7x7 = reinterpret_cast<const std::array<float, 49>&>(k);
                kernel.useKernel7x7 = true;
                break;
            default:
                spdlog::warn("Invalid kernel size!");
                return;
        }
        kernel.isActive = true;
    }
    template<size_t S>
    void ApplyKernel(std::array<float, S> k, float factor) {
        for (auto& e : k) {
            e *= factor;
        }
        ApplyKernel(k);
    }
};

namespace Convolution {
    template <size_t S>
    constexpr void Normalize(std::array<float, S>& kernel) {
        float sum = std::accumulate(kernel.begin(), kernel.end(), 0.0f, std::plus<float>());
        for (float& f : kernel) {
            f /= sum;
        }
    }
    template <size_t S>
    constexpr std::array<float, S> BoxBlur() {
        std::array<float, S> kernel;
        kernel.fill(1.0f / S);
        return kernel;
    }
    template <size_t S>
    constexpr std::array<float, S> GaussianBlur(int sigma = (int) sqrt(S)) {
        std::array<float, S> kernel;
        int root = (int) sqrt(S);
        float s = 2.0f * std::pow((float) sigma, 2.0f);
        for (int y = 0; y < root; y++) {
            for (int x = 0; x < root; x++) {
                float distX = std::abs((float) x - (root - 1));
                float distY = std::abs((float) y - (root - 1));
                float val = std::exp(-(distX * distX + distY * distY) / s) / std::sqrt((float) M_PI * s);
                kernel[y * root + x] = val;
            }
        }
        Normalize(kernel);
        return kernel;
    }
};