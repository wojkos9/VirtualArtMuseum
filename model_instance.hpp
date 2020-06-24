#pragma once
#include "model.hpp"
#include "animate.hpp"
#include <queue>
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

enum AnimType {
    Idle=0, Start, Stop, Walk, Nothing, Rotate
};

//map<AnimType, string> name_map = {{Idle, "Idle"}, {Start,}};
string names[] = {"Idle", "Start", "Stop", "Walk", "Nothing", "Rotate"};
enum TaskType {
    WalkTo, Wait
};

class Task {
    public:
    TaskType tt;
    vec2 dst;
    float time;
    Task(TaskType tt, vec2 dst, float time) : tt(tt), dst(dst), time(time) {}
};


class ModelInstance {

public:
    float t = 0;
    map<int, Bone> *bones;
    Model &ctx;
    vector<mat4> mbs;
    AnimatedModel &amodel;
    bool do_animation = true;
    bool working = true;
    AnimType curr_anim = Nothing;
    int cycles = 1;
    vec3 pos;
    float d_since = 0.f;
    float last_d = 0.f;
    deque<AnimType> animations;
    queue<Task> tasks;
    bool walking = false;
    
    float rot = 0.f;
    float target_rot = 0.f;
    bool rotating = false;
    float rot_speed = 1.f;
    bool clockwise = true;
    bool reached = true;

    vec2 dir = vec2(0, 1);
    vec2 target = vec2(0);

    int hips_node = 64;

    void printAnimations() {
        cout << "Animations\n";
        for (auto it = animations.begin(); it != animations.end(); it++) {
            cout << names[*it] << endl;
        }
    }

    void stop() {
        if (walking) {
            animations.push_back(Stop);
            animations.push_back(Idle);
            cout << "stop" << endl;
            walking = false;
        }
        
    }

    bool reachedGoal() {
        return reached;
    }

    void rotateTo(float rad) {
        if (animations.back() != Rotate) {
            cout << "ROT: " << rad << endl;
            target_rot = rad;
            clockwise = target_rot > rot;
            animations.push_back(Rotate);
        }
        
    }
    int anim_counter = 0;

    void nextAnimation() {
        if (animations.size() > 0) {
            curr_anim = animations.front();
            anim_counter++;
            last_d = 0;
            t = 0;
            animations.pop_front();
            printAnimations();
            cout << "anim " << names[curr_anim] << "/" << animations.size() << endl;
        }
    }

    void update(float dt) {

        if (curr_anim == Rotate) {
            rot += rot_speed * (2*clockwise-1) * dt;
            if (rot >= target_rot && clockwise || rot <= target_rot && (!clockwise)) {
                rot = target_rot;
                //rotating = false;
                nextAnimation();
            }
        } else {
                      
            // Animate bones
            if (curr_anim != Nothing) {
                t += dt;
                working = animate(*bones, ctx, t, &d_since, curr_anim, hips_node);
                //cout << anim_counter << " : " << d_since << endl;

                if (curr_anim != Idle) {
                    float dr = (d_since>last_d)?(d_since-last_d):d_since;
                    dr = dr*0.005;
                    pos += rotateY(vec3(0, 0, dr), rot);
                    if (distance(vec2(pos.x, pos.z), target) <= 0.5f) {
                        stop();
                        reached = true;
                    }
                }
                
                
                last_d = d_since;
            } else {
                working = false;
                t = 0;
                last_d = 0;
            }
            if (!working) {
                nextAnimation();
            }
        }
        
        

        // Compute bone matrices
        int i = 0;
        for(int j : ctx.skins[0].joints) {
            mbs[i] = compute_mn(&(*bones)[j]) * *(*bones)[j].ob;
            i++;
        }
    }

    void start() {
        if (!walking) {
            animations.push_back(Start);
            animations.push_back(Walk);
            cout << "start" << endl;
            walking = true;
        }
    }

    void goTo(vec2 dst) {
        vec2 p = vec2(pos.x, pos.z);
        target = dst;
        dir = normalize(dst-p);
        float new_rot = acos(dot(dir, vec2(0, 1))) * sign(dir.x);
        cout << "ROT" << new_rot << endl;
        rotateTo(new_rot);
        start();
        reached = false;
    }

    
    ModelInstance(AnimatedModel &am) : amodel(am), ctx(am.ctx) {
        bones = copyBones(am.bones);
        prepareBones();
        pos = vec3(0, -0.5f, 0);
    }
    void prepareBones() {

        // CPU buffer for bone matrices
        int n_bones = bones->size();
        mbs = vector<mat4>(n_bones, mat4());
    }
    void draw(Renderer &r) {
       
        r.passBoneMatrices(mbs);
        r.renderModel(amodel);
    }
};