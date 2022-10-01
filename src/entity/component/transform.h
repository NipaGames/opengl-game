#include "entity/component.h"

#include <opengl.h>

class Transform : Component {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
};