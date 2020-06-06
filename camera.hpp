#pragma once

#include "main.hpp"
#include "movable.hpp"

class MCamera {
    protected:
    mat4 v, p;
    vec3 position;
    vec3 dir;
    bool m_dirty = true;
    vec2 rot;
    friend class Renderer;
    public:
    MCamera(float fov=70.f) {
        p = perspective(radians(fov), (float)640/480, 0.1f, 500.f);
        rotate(vec2(0));
        v = lookAt(vec3(0, 0, 0), dir, vec3(0, 1, 0));
    }

    void rotate(vec2 dr) {
        rot += dr;
        dir = vec3(sin(rot.x)*cos(rot.y), sin(rot.y), -cos(rot.x)*cos(rot.y));
        m_dirty = true;
    }
    void move(vec3 t) {
        position += t;
        m_dirty = true;
    }

    mat4& get_p() {
        return p;
    }

    mat4& get_v() {
        if (m_dirty) {
            v = translate(lookAt(vec3(0, 0, 0), dir, vec3(0, 1, 0)), -position);
            m_dirty = false;
        }
        return v;
    }

};