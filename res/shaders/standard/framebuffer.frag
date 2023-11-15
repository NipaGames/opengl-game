#version 330 core

out vec4 color;
uniform sampler2D screenTexture;
in vec2 fragmentTexCoord;

struct Config {
    float gamma;
    float contrast;
    float brightness;
    float saturation;
};

struct Vignette {
    bool isActive;
    float size;
    float treshold;
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

    float gamma;
    float contrast;
    float brightness;
    float saturation;
};

uniform Config cfg;
uniform PostProcessing postProcessing;

vec3 applyKernels(float kernelIntensity, float offset) {
    vec3 col = vec3(0.0);
    if (postProcessing.kernel.useKernel3x3) {
        for (int i = 0; i < 9; i++) {
            int x = i / 3;
            int y = i % 3;
            col += vec3(texture(screenTexture, fragmentTexCoord.st + ivec2(x - 1, 1 - y) * offset)) * postProcessing.kernel.kernel3x3[i] * kernelIntensity;
        }
    }
    if (postProcessing.kernel.useKernel5x5) {
        for (int i = 0; i < 25; i++) {
            int x = i / 5;
            int y = i % 3;
            col += vec3(texture(screenTexture, fragmentTexCoord.st + ivec2(x - 2, 2 - y) * offset)) * postProcessing.kernel.kernel5x5[i] * kernelIntensity;
        }
    }
    if (postProcessing.kernel.useKernel7x7) {
        for (int i = 0; i < 49; i++) {
            int x = i / 7;
            int y = i % 3;
            col += vec3(texture(screenTexture, fragmentTexCoord.st + ivec2(x - 3, 3 - y) * offset)) * postProcessing.kernel.kernel7x7[i] * kernelIntensity;
        }
    }
    return col;
}

void main() {
    vec3 col = vec3(0.0);
    float dist = distance(fragmentTexCoord, vec2(.5, .5));
    col = vec3(texture(screenTexture, fragmentTexCoord.st));

    // convolution
    int kernels = 0;
    if (postProcessing.kernel.isActive && postProcessing.kernel.blend > 0.0) {
        col *= (1.0 - postProcessing.kernel.blend);
        kernels = int(postProcessing.kernel.useKernel3x3) + int(postProcessing.kernel.useKernel5x5) + int(postProcessing.kernel.useKernel7x7);
        float offset = postProcessing.kernel.offset;
        float vignetteFactor = 1.0;
        if (postProcessing.kernel.vignette.isActive) {
            vignetteFactor = 1.0 - smoothstep(postProcessing.kernel.vignette.size, .5 * postProcessing.kernel.vignette.size, dist * (.3 + .5));
            offset *= vignetteFactor;
        }
        float kernelIntensity = postProcessing.kernel.blend / float(kernels);
        if (postProcessing.kernel.vignette.isActive && vignetteFactor < postProcessing.kernel.vignette.treshold) {
            col = vec3(texture(screenTexture, fragmentTexCoord.st));
        }
        else {
            col += applyKernels(kernelIntensity, offset);
        }
    }
    
    // contrast
    float contrast = cfg.contrast * postProcessing.contrast;
    col.rgb = ((col.rgb - 0.5) * contrast) + 0.5;
    // brightness
    float brightness = cfg.brightness * postProcessing.brightness;
    col.rgb += brightness - 1.0;
    // clamp to positive only
    col.r = max(col.r, 0.0);
    col.g = max(col.g, 0.0);
    col.b = max(col.b, 0.0);
    // saturation
    float saturation = cfg.saturation * postProcessing.saturation;
    if (saturation != 1.0) {
        float satMax = 3.0 - saturation;
        col.r = (col.r * (1.0 + saturation) + col.g * (1.0 - saturation) + col.b * (1.0 - saturation)) / satMax;
        col.g = (col.r * (1.0 - saturation) + col.g * (1.0 + saturation) + col.b * (1.0 - saturation)) / satMax;
        col.b = (col.r * (1.0 - saturation) + col.g * (1.0 - saturation) + col.b * (1.0 + saturation)) / satMax;
    }
    // gamma
    float gamma = cfg.gamma * postProcessing.gamma;
    if (gamma != 1.0) {
        col.r = pow(col.r, gamma);
        col.g = pow(col.g, gamma);
        col.b = pow(col.b, gamma);
    }

    // vignette
    if (postProcessing.vignette.isActive) {
        float vignetteFactor = 1.0 - smoothstep(postProcessing.vignette.size, .5 * postProcessing.vignette.size, dist * (.3 + .5));
        if (vignetteFactor >= postProcessing.vignette.treshold) {
            col.rgb *= 1.0 - vignetteFactor;
            col.rgb += postProcessing.vignetteColor * vignetteFactor;
        }
    }

    color = vec4(col, 1.0);
}