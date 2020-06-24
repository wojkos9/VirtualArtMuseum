#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;
using namespace glm;


#ifdef __linux__
#include <unistd.h>
void sleep(int msec) {
    usleep(std::max(1000 * msec, 0));
}
#elif _WIN32
#include <windows.h>
void sleep(int msec) {
    Sleep(std::max(msec, 0));
}
#endif

struct Bone {
    int id;
    mat4 *ob;
    mat4 ms;
    mat4 mn;
    Bone *parent;
    vector<Bone*> children;
    Bone(int id) : id(id), children(), parent(nullptr) {}
    Bone() : parent(nullptr) {}
};

map<int, Bone>* copyBones(map<int, Bone>* bones) {
    map<int, Bone>* bones2 = new map<int, Bone>(*bones);
    for (pair<const int, Bone> &p : *bones) {
        Bone &b = p.second;
        Bone &b1 =  (*bones2)[b.id];
        b1.parent = b.parent!=nullptr ? &(*bones2)[b.parent->id] : nullptr;
        for (Bone* &c : b1.children) {
            c = &(*bones2)[c->id];
        }
    }
    return bones2;
}

struct material {
    GLuint tex_id;
};
struct vao {
    GLuint index;
    GLuint eb_id;
    size_t count;
    int material;
    string name;
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

GLuint texture_quad() {
    float xy[4] = {0.45f, 0.25f, 0.65f, 0.45f};
    GLfloat qdata[] = {
        0, 0, 0, xy[0], xy[1],
        0, 1, 0, xy[0], xy[3],
        1, 0, 0, xy[2], xy[1],
        1, 1, 0, xy[2], xy[3]
    };
    GLushort qix[] = {0, 2, 3, 0, 3, 1};

    GLuint quad;
    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);
    GLuint qbuf[2];
    glGenBuffers(2, &qbuf[0]);
    glBindBuffer(GL_ARRAY_BUFFER, qbuf[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(qdata), qdata, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qbuf[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(qix), qix, GL_STATIC_DRAW);
    return quad;
}

GLuint gen_dot() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float v[] = {0, 0, 0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return vao;
}

#include "stb_image.h"

unsigned int loadTexture( char const *path){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureWidth, textureHeight, textureNrChannels;
    unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &textureNrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture " << path << std::endl;
    }
    stbi_image_free(data);
    return texture;
}
