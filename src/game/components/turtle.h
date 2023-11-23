#include "animationcomponent.h"

#include <core/graphics/material.h>
#include <core/graphics/component/light.h>

class Turtle : public AnimationComponent {
private:
    std::vector<std::shared_ptr<Material>> materials_;
    float baseY_;
    Lights::PointLight* light_ = nullptr;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, drop, 15.0f);
    ~Turtle();
    void Interpolate(float) override;
    void End() override;
    void FirstUpdate() override;
};
REGISTER_COMPONENT(Turtle);