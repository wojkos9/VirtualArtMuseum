#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace glm;

struct Bone {
    mat4 *ob;
    mat4 ms;
    mat4 mn;
    Bone *parent = nullptr;
    vector<Bone*> children;
    Bone() : children() {}
};

struct M {
    GLuint vao;
    int count;
    map<int, Bone> *bones;
    int bone_node;
    M(GLuint vao,
    int count,
    map<int, Bone> *bones,
    int bone_node) : vao(vao), count(count), bones(bones), bone_node(bone_node){}
};

void printm(mat4 m) {
    for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++)
    printf("%.2f ", m[i][j]);
    printf("\n");}
    printf("\n");
}

GLuint make_shader(const char *fname, GLuint type) {
    GLuint sh;

    ifstream file;
    file.open(fname, ios::in);
    stringstream ss;
    ss << file.rdbuf();

    string s = ss.str();
    const char *src = s.c_str();

    sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    GLint cs=1;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &cs);
    if (cs == 0) {
        char buf[256];
        glGetShaderInfoLog(sh, 256, NULL, buf);
        printf("Error [%s]:\n%s\n", fname, buf);
    }
    return sh;
}

void decompose(mat4 &m1){//, vec3 &t, vec3 &r, vec3 &s) {
    vec3 sc;
    mat4 t = translate(mat4(1), vec3(1.1842f, -5.9616, -206.46f));
    mat4 r = mat4_cast(quat(0.728f, -0.685f, -0.006f, -0.005f));
    mat4 s = scale(mat4(1), vec3(100.f, 100.f, 100.f));


    mat4 m = inverse(m1);
    printf("-----------------\n");
    printm(m);
    printm(t * r * s);
    for (int i = 0; i < 3; i++) {
        sc[i] = sqrt(m[i][0]*m[i][0]+m[i][1]*m[i][1]+m[i][2]*m[i][2]+m[i][3]*m[i][3]);
        printf("S.%d: %f\n", sc[i]);
    }
}