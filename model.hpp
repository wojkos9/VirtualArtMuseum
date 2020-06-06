#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdio>
#include <iostream>
#include <iomanip>

#include "utils.hpp"
#include "animate.hpp"

#include "gltf/tiny_gltf.h"

using namespace tinygltf;

Bone* build_bone(Model &model, map<int, Bone>& bones, int node, bool set_children = true) {

    Bone *bone = &bones[node];
    auto &rot = model.nodes[node].rotation;
    auto &trans = model.nodes[node].translation;
    auto &sca = model.nodes[node].scale;
    quat q;

    mat4 s = !sca.empty() ? scale(mat4(1), vec3(sca[0], sca[1], sca[2])) : mat4(1);
    mat4 t = !trans.empty() ? translate(mat4(1), vec3(trans[0], trans[1], trans[2])) : mat4(1);
    mat4 r = !rot.empty() ? mat4_cast(quat(rot[3], rot[0], rot[1], rot[2])) : mat4(1);
    bone->ms = t * r * s;    

    if (set_children)
        for (int c : model.nodes[node].children) {
            if (model.nodes[c].mesh != -1)
                continue;
            Bone *child = build_bone(model, bones, c);
            child->parent = bone;
            bone->children.push_back(child);
        }
    return bone;
}

map<int, Bone>* create_bones(Model &model, int bone_node) {
    vector<int> &joints = model.skins[0].joints;
    int ibv = model.accessors[model.skins[0].inverseBindMatrices].bufferView;
    BufferView &bv = model.bufferViews[ibv];
    map<int, Bone> *bones = new map<int, Bone>();
    int i = 0;
    for (int joint : joints) {
        (*bones)[joint] = Bone();
        (*bones)[joint].ob = (mat4*)&model.buffers[0].data[bv.byteOffset+i*sizeof(mat4)];
        i++;
    }

    for (int node : model.scenes[0].nodes)
        build_bone(model, *bones, node);

    return bones;
}

class AnimatedModel {
    map<int, Bone> *bones;
    vector<vao> vaos;
    vector<material> materials;
    Model ctx;
    float t = 0;
    vector<mat4> mbs;
    friend class Renderer;
    public:
    AnimatedModel() {

    }
    void load(const char *path) {
        TinyGLTF loader;
        std::string err;
        std::string warn;

        //bool ret = loader.LoadASCIIFromFile(&ctx, &err, &warn, path);
        bool ret = loader.LoadBinaryFromFile(&ctx, &err, &warn, path);

        if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
        }

        if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
        }

        if (!ret) {
        printf("Failed to parse glTF\n");
        }

        struct param {
            string name;
            bool integer;
            param(string name, bool integer) : name(name), integer(integer) {}
        };
        

        for (auto &m : ctx.materials) {
            int i =m.values["baseColorTexture"].json_double_value["index"];
            
            Image tex = ctx.images[i];
            GLuint tex_id;

            GLfloat pixels[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0};

            glGenTextures(1, &tex_id);
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, tex.pixel_type, &tex.image[0]);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            materials.push_back({tex_id});

        }

        
        for (Mesh &mesh : ctx.meshes) {
            int i_mtl = mesh.primitives[0].material;
            printf("Model %s: mat %d\n", mesh.name.c_str(), i_mtl);
            GLuint VAO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            int i_attrib = 0;
            vector<unsigned char> &buf = ctx.buffers[0].data;
            for (param prop : 
            {param("POSITION", false), param("JOINTS_0", true), param("WEIGHTS_0", false), param("TEXCOORD_0", false), param("NORMAL", false)}) {
                int i_buf = mesh.primitives[0].attributes[prop.name];
                Accessor acc = ctx.accessors[i_buf];
                BufferView bv = ctx.bufferViews[acc.bufferView];

                GLuint VBO;
                glGenBuffers(1, &VBO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, bv.byteLength, &ctx.buffers[0].data[bv.byteOffset], GL_STATIC_DRAW);
                if (prop.integer)
                    glVertexAttribIPointer(i_attrib, acc.type, acc.componentType, 0, 0);
                else
                    glVertexAttribPointer(i_attrib, acc.type, acc.componentType, GL_FALSE, 0, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                i_attrib++;
            }
            Accessor acc = ctx.accessors[mesh.primitives[0].indices];
            int i_el = acc.bufferView;
            BufferView bv = ctx.bufferViews[i_el];
            GLuint EB;
            glGenBuffers(1, &EB);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bv.byteLength, &ctx.buffers[0].data[bv.byteOffset], GL_STATIC_DRAW);
            
            glBindVertexArray(0);

            vaos.push_back({VAO, EB, acc.count, i_mtl, mesh.name});
        }


        int main_node = ctx.scenes[0].nodes[0];
        int bone_node = ctx.skins[0].joints[0];

        bones = create_bones(ctx, bone_node);
        prepareBones();
    }

    void prepareBones() {
        // Move bones to default position
        for (pair<const int, Bone> &b : *bones) {
            b.second.mn = b.second.ms ;
        }

        // CPU buffer for bone matrices
        int n_bones = bones->size();
        mbs = vector<mat4>(n_bones, mat4());
    }

    void update(float dt) {
        t += dt;

        // Animate bones
        animate(*bones, ctx, t, mbs);

        // Compute bone matrices
        int i = 0;
        for(int j : ctx.skins[0].joints) {
            mbs[i] = compute_mn(&(*bones)[j]) * *(*bones)[j].ob;
            i++;
        }
    }
    
};