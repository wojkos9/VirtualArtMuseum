#pragma once
#include "model.hpp"

class Human {
    AnimatedModel &model;
    vec3 position;
    float rotation;
    public:
    Human(AnimatedModel &model) : model(model) {
        
    }
    void update(float dt) {
        model.update(dt);
    }

    void draw(Renderer &r) {
        r.i();
        r.tr(position);
        
        r.renderModel(model);
    }
};