#include "postprocessing.h"

#define PP_UNIFORM_NAME(uniform) "postProcessing."##uniform
#define PP_UPDATE_UNIFORM(shader, uniform) shader.SetUniform(PP_UNIFORM_NAME(#uniform), uniform)

void PostProcessing::ApplyUniforms(const Shader& shader) const {
    PP_UPDATE_UNIFORM(shader, kernel.isActive);
    PP_UPDATE_UNIFORM(shader, kernel.blend);
    PP_UPDATE_UNIFORM(shader, kernel.offset);
    PP_UPDATE_UNIFORM(shader, kernel.useKernel3x3);
    PP_UPDATE_UNIFORM(shader, kernel.useKernel5x5);
    PP_UPDATE_UNIFORM(shader, kernel.useKernel7x7);
    PP_UPDATE_UNIFORM(shader, kernel.vignette.isActive);
    PP_UPDATE_UNIFORM(shader, kernel.vignette.size);

    PP_UPDATE_UNIFORM(shader, kernel.kernel3x3);
    PP_UPDATE_UNIFORM(shader, kernel.kernel5x5);
    PP_UPDATE_UNIFORM(shader, kernel.kernel7x7);

    PP_UPDATE_UNIFORM(shader, vignette.isActive);
    PP_UPDATE_UNIFORM(shader, vignette.size);
    PP_UPDATE_UNIFORM(shader, vignetteColor);
}