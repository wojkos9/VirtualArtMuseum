#pragma once
#include "model.hpp"

class ModelInstance {

public:
    float t = 0;
    map<int, Bone> *bones;
    Model &ctx;
    vector<mat4> mbs;
    AnimatedModel &amodel;
    

    void update(float dt) {
        t += dt;

        // Animate bones
        animate(*bones, ctx, t);

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