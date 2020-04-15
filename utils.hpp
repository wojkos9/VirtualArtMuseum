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

struct material {
    GLuint tex_id;
};
struct vao {
    GLuint index;
    GLuint eb_id;
    size_t count;
    int material;
};

struct M {
    vector<vao> vaos;
    map<int, Bone> *bones;
    vector<material> materials;
    M(vector<vao> vaos, map<int, Bone> *bones, vector<material> materials) : vaos(vaos), bones(bones), materials(materials){}
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

GLuint make_program(const char *vertex_fname, const char *fragment_fname) {
    GLuint vs = make_shader(vertex_fname, GL_VERTEX_SHADER);
    GLuint fs = make_shader(fragment_fname, GL_FRAGMENT_SHADER);

    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    GLint ls=1;
    glGetProgramiv(sp, GL_LINK_STATUS, &ls);
    printf("Program status:%d\n", ls);
    if (ls == 0) {
        char buf[256];
        glGetProgramInfoLog(sp, 256, NULL, buf);
        printf("%s\n", buf);
    }
    return sp;
}
