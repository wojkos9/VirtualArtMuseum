#include "gltf/tiny_gltf.h"
using namespace tinygltf;

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace glm;

#include <iostream>
using namespace std;

void printm(mat4 m) {
    for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++)
    printf("%.2f ", m[i][j]);
    printf("\n");}
    printf("\n");
}
void printv(vec3 &v) {
    printf("%.2f %.2f %.2f\n", v.x, v.y, v.z);
}
#define printvv(v) printf(#v" "),printv(v);
float dst2(vec3 v) {
    return v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}
float dst(vec3 v) {
    return sqrt(dst2(v));
}

void decompose(mat4 m) {
    //mat4 m = inverse(mat);
    vec3 t = m[3];
    vec3 s = vec3(dst(m[0]), dst(m[1]), dst(m[2]));
    mat4 rm = mat4(vec4(m[0])/s.x, vec4(m[1])/s.y, vec4(m[2])/s.z, vec4(0, 0, 0, m[3][3]));
    //mat3 r3 = mat3(vec3(m[0])/s.x, vec3(m[1])/s.y, vec3(m[2])/s.z);
    vec3 e;
    extractEulerAngleXYZ(rm, e.x, e.y, e.z);
    printvv(t);
    vec3 r = degrees(e);
    printvv(r);
    printvv(s);
}

mat4 get_ms(Model &model, int node) {
    auto &rot = model.nodes[node].rotation;
    auto &trans = model.nodes[node].translation;
    auto &sca = model.nodes[node].scale;

    mat4 s = !sca.empty() ? scale(mat4(1), vec3(sca[0], sca[1], sca[2])) : mat4(1);
    mat4 t = !trans.empty() ? translate(mat4(1), vec3(trans[0], trans[1], trans[2])) : mat4(1);
    mat4 r = !rot.empty() ? mat4_cast(quat(rot[3], rot[0], rot[1], rot[2])) : mat4(1);
    return t * r * s;
}

int main() {
    // mat4 t = translate(mat4(1), vec3(1, 2, 3));
    // mat4 r = rotate(mat4(1), radians(30.f), vec3(0, 1, 0));
    // mat4 s = scale(mat4(1), vec3(4, 5, 6));
    // mat4 m = t * r * s;
    // mat4 iv = inverse(m);
    // printm(r);
    // printm(iv);
    // decompose(iv);
    // return 0;
    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    //bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, "/home/wojtek/walk.glb"); // for binary glTF(.glb)
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

    mat4 gt = get_ms(model, model.scenes[0].nodes[0]);
    printf("GT:\n");
    printm(gt);
    decompose(gt);

    int ibv = model.accessors[model.skins[0].inverseBindMatrices].bufferView;
    vector<int> &joints = model.skins[0].joints;
    BufferView &bv = model.bufferViews[ibv];
    vector<unsigned char> &buf = model.buffers[0].data;
    mat4 *mats = (mat4*)&buf[bv.byteOffset];
    int i = 0;
    for (int j : joints) {
        printf("OB_%d %s\n", j, model.nodes[j].name.c_str());
        
        mat4 obi = inverse(mats[i]);
        printm(obi);
        decompose(obi);
        printf("\n");
        i++;
        if (i >= 3)
        break;
    }
    // for (int j : model.scenes[0].nodes) {
    //     printf("MS %d\n", j);
    //     printm(get_ms(model, j));
    // }

    return 0;
}