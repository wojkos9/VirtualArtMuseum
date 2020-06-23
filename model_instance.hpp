#pragma once
#include "model.hpp"
#include "animate.hpp"
#include <queue>

enum AnimType {
    Start=0, Walk=1, Stop=2, Idle
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
    AnimType curr_anim = Idle;
    int cycles = 1;
    int order[3] = {0, 2, 1};
    vec3 pos;
    float d_since = 0.f;
    float last_d = 0.f;
    queue<AnimType> animations;
    bool walking = false;

    void stop() {
        if (walking) {
            animations.push(Stop);
            animations.push(Idle);
            cout << "stop" << endl;
            walking = false;
        }
        
    }

    void update(float dt) {

        if (!working) {
            if (animations.size() > 0) {
                curr_anim = animations.front();
                last_d = 0;
                animations.pop();
                cout << "anim " << curr_anim << "/" << animations.size() << endl;
            }
        }
        
        // Animate bones
        if (curr_anim != Idle) {
            t += dt;
            working = animate(*bones, ctx, t, &d_since, order[curr_anim], 64);
            float dr = (d_since>last_d)?(d_since-last_d):d_since;
            dr = dr*0.005;
            //cout << dr << pos.z << endl;
            pos += vec3(0, 0, dr);
            last_d = d_since;
        } else {
            working = false;
            t = 0;
            last_d = 0;
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
            animations.push(Start);
            animations.push(Walk);
            cout << "start" << endl;
            walking = true;
        }
        
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