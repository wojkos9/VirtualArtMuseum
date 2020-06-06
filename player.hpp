#pragma once
#include "camera.hpp"

class Player : public MCamera {
    vec3 vel;
    public:
    Player() : MCamera(), vel(0) {

    }

    void set_moving_dir(vec3 vel_tmp) {
        vel = -vel_tmp.z * dir + vel_tmp.x * cross(dir, vec3(0, 1, 0)) + vec3(0, vel_tmp.y, 0);
    }

    void update(float dt) {
        move(vel * dt);
    }
};