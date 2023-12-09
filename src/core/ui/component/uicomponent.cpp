#include "uicomponent.h"
#include <core/entity/entity.h>

using namespace UI;

UIComponent::UIComponent(Canvas* c, int p) :
    canvas_(c),
    priority_(p)
{ }

UIComponent::~UIComponent() {
    if (isAddedToCanvas_ && canvas_ != nullptr)
        canvas_->RemoveUIComponent(this);
}

void UIComponent::AddToCanvas() {
    if (canvas_ == nullptr)
        return;
    canvas_->AddUIComponent(this, priority_);
    isAddedToCanvas_ = true;
}

void UIComponent::AddToCanvas(Canvas* canvas) {
    canvas_ = canvas;
    AddToCanvas();
}

UITransform UIComponent::GetTransform() {
    if (transformFrom == UITransformFrom::ENTITY_TRANSFORM_2D) {
        UITransform trans;
        trans.pos = glm::vec2(parent->transform->position.x, parent->transform->position.y);
        // janky ass way to determine the size
        trans.size = parent->transform->size.z;
        return trans;
    }
    return transform;
}

void UIComponent::SetTransform(const UITransform& trans) {
    transform = trans;
    if (transformFrom == UITransformFrom::ENTITY_TRANSFORM_2D) {
        parent->transform->position.x = transform.pos.x;
        parent->transform->position.y = transform.pos.y;
        parent->transform->size.z = transform.size;
    }
}