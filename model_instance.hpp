#pragma once
#include "model.hpp"
#include "animate.hpp"
class ModelInstance {

public:
    float t = 0;
    map<int, Bone> *bones;
    Model &ctx;
    vector<mat4> mbs;
    AnimatedModel &amodel;
    bool do_animation = true;
    bool working = true;
    int aix = 0;
    int next_aix = 0;
    int cycles = 1;
    int order[3] = {0, 2, 1};
    vec3 pos;
    float d_since = 0.f;
    float last_d = 0.f;
    

    void stop() {
        next_aix = (aix+1)%3;
    }

    void update(float dt) {
        t += dt;
        // Animate bones
        working = animate(*bones, ctx, t, &d_since, order[aix], 64);
        float dr = (d_since>last_d)?(d_since-last_d):d_since;
        dr = dr*0.005;
        //cout << dr << pos.z << endl;
        pos += vec3(0, 0, dr);
        last_d = d_since;
        
        if (!working && next_aix != aix) {
            cycles--;
            cout << "animation " << aix << " " << "cycle " << cycles << endl;
            if (cycles <= 0) {
                aix = next_aix;
                if (aix == 1)
                    cycles = 5;
                else
                    cycles = 1;
                next_aix = (aix+1)%3;
                t = 0;
            }
        }
        

        // Compute bone matrices
        int i = 0;
        for(int j : ctx.skins[0].joints) {
            mbs[i] = compute_mn(&(*bones)[j]) * *(*bones)[j].ob;
            i++;
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