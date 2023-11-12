#version 330 core

out vec4 color;
uniform sampler2D screenTexture;
in vec2 fragmentTexCoord;

struct Config {
    float gamma;
};

struct Vignette {
    bool isActive;
    float size;
};

struct Kernel {
    bool isActive;
    float offset;
    float blend;

    float kernel3x3[9];
    bool useKernel3x3;

    float kernel5x5[25];
    bool useKernel5x5;

    float kernel7x7[49];
    bool useKernel7x7;

    Vignette vignette;
};

struct PostProcessing {
    Kernel kernel;
    Vignette vignette;
    vec3 vignetteColor;
};

uniform Config cfg;
uniform PostProcessing postProcessing;

void main() {
    vec3 col = vec3(0.0);
    float dist = distance(fragmentTexCoord, vec2(.5, .5));

    // convolution
    int kernels = 0;
    if (postProcessing.kernel.isActive && postProcessing.kernel.blend > 0.0) {
        kernels = int(postProcessing.kernel.useKernel3x3) + int(postProcessing.kernel.useKernel5x5) + int(postProcessing.kernel.useKernel7x7);
        float offset = postProcessing.kernel.offset;
        if (postProcessing.kernel.vignette.isActive) {
            offset *= 1.0 - smoothstep(postProcessing.kernel.vignette.size, .5 * postProcessing.kernel.vignette.size, dist * (.3 + .5));
        }
        float kernelIntensity = postProcessing.kernel.blend / float(kernels);
        if (postProcessing.kernel.useKernel3x3) {
            for (int i = 0; i < 9; i++) {
                int x = i / 3;
                int y = i % 3;
                col += vec3(texture2D(screenTexture, fragmentTexCoord.st + ivec2(x - 1, 1 - y) * offset)) * postProcessing.kernel.kernel3x3[i] * kernelIntensity;
            }
        }
        if (postProcessing.kernel.useKernel5x5) {
            for (int i = 0; i < 25; i++) {
                int x = i / 5;
                int y = i % 3;
                col += vec3(texture2D(screenTexture, fragmentTexCoord.st + ivec2(x - 2, 2 - y) * offset)) * postProcessing.kernel.kernel5x5[i] * kernelIntensity;
            }
        }
        if (postProcessing.kernel.useKernel7x7) {
            for (int i = 0; i < 49; i++) {
                int x = i / 7;
                int y = i % 3;
                col += vec3(texture2D(screenTexture, fragmentTexCoord.st + ivec2(x - 3, 3 - y) * offset)) * postProcessing.kernel.kernel7x7[i] * kernelIntensity;
            }
        }
    }
    if (kernels == 0) {
        col = vec3(texture2D(screenTexture, fragmentTexCoord.st));
    }
    else if (postProcessing.kernel.blend < 1.0) {
        col += vec3(texture2D(screenTexture, fragmentTexCoord.st) * (1.0 - postProcessing.kernel.blend));
    }
    
    color = vec4(col, 1.0);

    // vignette
    if (postProcessing.vignette.isActive) {
        float vignetteModifier = smoothstep(postProcessing.vignette.size, .5 * postProcessing.vignette.size, dist * (.3 + .5));
        color.rgb *= vignetteModifier;
        color.rgb += postProcessing.vignetteColor * (1.0 - vignetteModifier);
    }

    // gamma correction
    color.r = pow(color.r, cfg.gamma);
    color.g = pow(color.g, cfg.gamma);
    color.b = pow(color.b, cfg.gamma);
}