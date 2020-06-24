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
        position = vec3(0, 0.2f, 0);
        resizeView(854, 480, fov);
        rotate(vec2(0));
        v = lookAt(vec3(0, 0, 0), dir, vec3(0, 1, 0));
    }

    void resizeView(float w, float h, float fov=70.f) {
        p = perspective(radians(fov), (float)w/h, 0.1f, 500.f);
    }

    void rotate(vec2 dr) {
        rot += dr;
        dir = vec3(sin(rot.x)*cos(rot.y), sin(rot.y), -cos(rot.x)*cos(rot.y));
        m_dirty = true;
    }
    void move(vec3 t) {
         bool legalMove=true;
        if ((position + t).x >9.8 || (position+ t).x <-9.8)  
            legalMove=false;
            
        if ((position + t).y > 0.35 || (position+ t).y <-0.35)
            legalMove = false;
            
        if ((position + t).z > 1.3f || (position+ t).z <-1.3f)
            legalMove = false;

        if ((position + t).z > 1.3f || (position+ t).z <-1.3f)
            legalMove = false;

        if(  ((position + t).z > 0.36f || (position+ t).z <-0.36f) &&   ((position + t).x >0.3 && (position+ t).x <0.9))
            legalMove = false;
        /////
   

        
        if(  ((position + t).z > 0.36f || (position+ t).z <-0.36f) &&   ((position + t).x >5.3 && (position+ t).x <5.9))
            legalMove = false;
        
        if(  ((position + t).z > 0.36f || (position+ t).z <-0.36f) &&   ((position + t).x <-4.2 && (position+ t).x >-4.8))
            legalMove = false;

        if(legalMove)
            position +=vec3(t.x, 0, t.z);
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