#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdio>
#include <iostream>
#include <iomanip>

#include "utils.hpp"

#include "gltf/tiny_gltf.h"
using namespace tinygltf;



Bone* build_bone(Model &model, map<int, Bone>& bones, int node, bool set_children = true) {
    cout << "BONE " << node << endl;
    Bone *bone = &bones[node];
    auto &rot = model.nodes[node].rotation;
    auto &trans = model.nodes[node].translation;
    auto &sca = model.nodes[node].scale;
    quat q;
    //cout << setprecision(2);
   // cout.setf(ios::fixed);
    if (!rot.empty()) {
        q = quat(rot[3], rot[0], rot[1], rot[2]);
        vec3 a = eulerAngles(q) * 180.f / 3.14159265f;
        cout << "angle " << a.x << " " << a.y << " " << a.z << endl;
    }
    if (!trans.empty()) {
        cout << "trans " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
    }
    
    if (!sca.empty()) {
        cout << "sca " << sca[0] << " " << sca[1] << " " << sca[2] << endl;
    }

    mat4 s = !sca.empty() ? scale(mat4(1), vec3(sca[0], sca[1], sca[2])) : mat4(1);
    mat4 t = !trans.empty() ? translate(mat4(1), vec3(trans[0], trans[1], trans[2])) : mat4(1);
    mat4 r = !rot.empty() ? mat4_cast(q) : mat4(1);
    bone->ms = t * r * s;
    // bone->ms = !rot.empty() ? mat4_cast(quat(rot[0], rot[1], rot[2], rot[3])) : mat4(1);
    // if (!trans.empty())
    //     bone->ms = translate(bone->ms, vec3(trans[0], trans[1], trans[2]));
    //cout << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << endl;
    printm(bone->ms);
    

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
    // for (int i = 0; i < joints.size(); i++) {
    //     printm(*bones[i].ob);
    // }
    Bone *bone = build_bone(model, *bones, model.scenes[0].nodes[0]);
    //build_bone(model, *bones, 74);
    //printm(inverse((*bones)[74].ms));
    bone->parent = nullptr;

    
    
    // bone.ob = 
    return bones;
}

M loadModel(const char *path, Model &model) {
    TinyGLTF loader;
    std::string err;
    std::string warn;

    //bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path); // for binary glTF(.glb)
    //return {1, 2};

    if (!warn.empty()) {
    printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
    printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
    printf("Failed to parse glTF\n");
    }

    //static map<string, int> map_sizes = {{"VEC2", 2}, {"VEC3", 3}, {"VEC4", 4}, {"MAT4", 16}, {"SCALAR", 1}};

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int i_attrib = 0;
    int count;
    struct param {
        string name;
        bool integer;
        param(string name, bool integer) : name(name), integer(integer) {}
    };
    for (param prop : {param("POSITION", false), param("JOINTS_0", true), param("WEIGHTS_0", false)}) {
        int i_buf = model.meshes[0].primitives[0].attributes[prop.name];
        BufferView bv = model.bufferViews[i_buf];
        Accessor acc = model.accessors[i_buf];
        count = acc.count;
        vector<unsigned char> &buf = model.buffers[0].data;


        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, bv.byteLength, &model.buffers[0].data[bv.byteOffset], GL_STATIC_DRAW);
        if (prop.integer)
            glVertexAttribIPointer(i_attrib, acc.type, acc.componentType, 0, 0);
        else
            glVertexAttribPointer(i_attrib, acc.type, acc.componentType, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        cout << bv.byteLength << " " << acc.count << endl;

        //for (int i = 0; i < acc.)
        
        i_attrib++;
    }
    int i_el = model.meshes[0].primitives[0].indices;
    BufferView bv = model.bufferViews[i_el];
    Accessor acc = model.accessors[i_el];
    GLuint EB;
    glGenBuffers(1, &EB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bv.byteLength, &model.buffers[0].data[bv.byteOffset], GL_STATIC_DRAW);
    
    glBindVertexArray(0);

    cout << "Count: " << acc.count << endl;


    int main_node = model.scenes[0].nodes[0];
    int bone_node = model.skins[0].joints[0];
    // for (int node : model.nodes[main_node].children) {
    //     if (model.nodes[node].mesh == -1) {
    //         bone_node = node;
    //     }
    // }

    map<int, Bone> *bones = create_bones(model, bone_node);

    return M(VAO, acc.count, bones, bone_node);
}
