#pragma once
#include "model.hpp"
#include "gltf/tiny_gltf.h"
#include <glm/gtc/quaternion.hpp>

using namespace tinygltf;

mat4 compute_mn(Bone *b) {
    mat4 ret = b->parent==nullptr ? b->mn : compute_mn(b->parent) * b->mn;
    return ret;
}

struct quaternion {
    float x, y, z, w;
};

void animate(map<int, Bone> &bones, Model &context, float t, vector<mat4> &mbs) {
    int a = 0;
    if (context.animations.size()==0) {
        static bool informed = false;
        if (!informed) {
            printf("ERROR: No animations\n");
            informed = true;
        }
        return;
    }
    vector<AnimationChannel> &channels = context.animations[a].channels;
    vector<unsigned char> &buf = context.buffers[0].data;

    // for (auto &b : bones) {
    //     b.second.mn = mat4(1);
    // }
    
    for (int i = 0; i < channels.size(); i++) {
        int node = channels[i].target_node;
        AnimationSampler &sampler = context.animations[a].samplers[channels[i].sampler];
        BufferView &bv_in = context.bufferViews[context.accessors[sampler.input].bufferView];
        BufferView &bv_out = context.bufferViews[context.accessors[sampler.output].bufferView];

        float *in_vals = (float*)&buf[bv_in.byteOffset];
        float dt = in_vals[1]-in_vals[0];
        int nf = bv_in.byteLength / sizeof(float) - 1;
        float atime = nf * dt;

        float t1 = t - (int)(t / atime)*(float)atime;
        int id = (int)(t1 / dt) % nf;
        int id1 = std::min(nf, id+1);

        float alpha = (t1-in_vals[id])/dt;

        switch (channels[i].target_path[0]) {
            case 't': {
                vec3 *out_vals = (vec3*)&buf[bv_out.byteOffset];
                vec3 &a = out_vals[id];
                vec3 &b = out_vals[id1];
                vec3 c = mix(a, b, alpha);
                //printf("t: %f %f %f\n", c.x, c.y, c.z);
                bones[node].mn = translate(mat4(1), mix(a, b, alpha));
                break;
            }
            case 'r': {
                quaternion *out_vals = (quaternion*)&buf[bv_out.byteOffset];
                quaternion &q1 = out_vals[id];
                quaternion &q2 = out_vals[id1];
                quat a(q1.w, q1.x, q1.y, q1.z);
                quat b(q2.w, q2.x, q2.y, q2.z);
                quat c = mix(a, b, alpha);
                bones[node].mn *= mat4_cast(c);
                break;
            }
            case 's': {
                vec3 *out_vals = (vec3*)&buf[bv_out.byteOffset];
                vec3 &a = out_vals[id];
                vec3 &b = out_vals[id1];
                vec3 c = mix(a, b, alpha);
                
                bones[node].mn *= scale(mat4(1), c);
                break;
            }
        }
    }
}