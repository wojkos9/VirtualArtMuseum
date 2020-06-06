#pragma once

#include <set>
#include <cstdio>

#include <GL/glew.h>
#include "utils.hpp"

class Shader {
    std::set<const char*>* u_missing;
    GLuint sp;

    const char *foo;

public:
    Shader() {
        u_missing = new std::set<const char*>();
    }
    Shader(const char *vertex_fname, const char *fragment_fname) : Shader() {
        foo = vertex_fname;
        compile_src(vertex_fname, fragment_fname);
    }
    void compile_src(const char *vertex_fname, const char *fragment_fname) {
        sp = make_program(vertex_fname, fragment_fname);
    }
    GLint loc(const char *name) {
        GLint l = glGetUniformLocation(sp, name);
        if (l < 0 && u_missing->find(name) == u_missing->end()) {
            printf("%s: uniform not found: %s\n", foo, name);
            u_missing->emplace(name);
        }
        
        return l;
    }
    void use() {
        glUseProgram(sp);
    }
};