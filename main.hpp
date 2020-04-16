#pragma once

#include <glm/glm.hpp>
using namespace glm;

static int win_w = 640, win_h = 480;
static bool cursor_free = true;

// Camera:
float speed = 1.f;
vec3 position(0, 0, 3);
vec3 vel(0);
vec3 dir(0, 0, -1);
float cam_angle_x = 0.f, cam_angle_y = 0.f;

float fps_limit = 60.f;

mat4 mvp, m, v, p;

void update_projection();