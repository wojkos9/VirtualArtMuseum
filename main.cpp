#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gltf/tiny_gltf.h"
using namespace tinygltf;

#include "shaderprintf.h"

#include <iostream>

#include <cstdio>

#include <thread>
#include <chrono>

#include "utils.hpp"
#include "model.hpp"
#include "animate.hpp"
#include "shader.hpp"

#include "shaders/flags.h"

using namespace std;
using namespace glm;

#include "callbacks.hpp"


void update_projection() {
    p = perspective(radians(70.f), (float)win_w/win_h, 0.1f, 500.f);
}

GLFWwindow* create_window(int width, int height) {
    GLFWwindow* win;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    win = glfwCreateWindow(width, height, "My Window", NULL, NULL);

    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPosCallback(win, mouse_callback);
    glfwSetWindowSizeCallback(win, window_size_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    return win;
}

void print_gfx_info() {
    GLint maj, min;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);

    printf("Version %d.%d\n", maj, min);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    printf("Vendor: %s\n", vendor);
}

void draw_debug(Shader &sp, M &model, GLuint vbo_bones, GLuint vao_bones, vec3 lp);

int main(int argc, char *argv[]) {
    
    glewExperimental = 1;
    if (! glfwInit()) {
        printf("GLFW fail\n");
    }

    GLFWwindow* win = create_window(win_w, win_h);
    glfwMakeContextCurrent(win);
    print_gfx_info();
    

    if (glewInit() != GLEW_OK) {
        printf("GLEW fail\n");
    }

    Shader sp("shaders/shaded.vsh", "shaders/shaded.fsh");

    // Initialize the projection matrix
    update_projection();
    
    glClearColor(0.6f, 0.5f, 8, 1);
    
    Model context;
    
    // TODO: in a different thread
    const char *model_fname = argc < 2 ? "models/walk.glb" : argv[1];
    M model = loadModel(model_fname, context);

    // Move bones to default position
    for (pair<const int, Bone> &b : *model.bones) {
        b.second.mn = b.second.ms ;
    }

    // CPU buffer for bone matrices
    int n_bones = (*model.bones).size();
    vector<mat4> mbs(n_bones, mat4());

    

    // GPU buffer for bone matrices
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, n_bones * sizeof(mat4), NULL, GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // DEBUG: for visualizing bones
    GLuint vao_bones;
    glGenVertexArrays(1, &vao_bones);
    glBindVertexArray(vao_bones);

    GLuint vbo_bones;
    glGenBuffers(1, &vbo_bones);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_bones);
    int sz = (*model.bones).size() * 2*sizeof(vec3) + sizeof(vec3);
    glBufferData(GL_ARRAY_BUFFER, sz, 0, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    // DEBUG: Dot light
    vao_dot = gen_dot();

    // Some models render incorrectly when this is on:
    // glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    float t = 0.f;
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Animate bones
        animate((*model.bones), context, t, mbs);

        // Compute bone matrices
        int i = 0;
        for(int j : context.skins[0].joints) {
            mbs[i] = compute_mn(&(*model.bones)[j]) * *(*model.bones)[j].ob;
            i++;
        }
        

        m = mat4(1);
        v = lookAt(vec3(0, 0, 0), dir, vec3(0, 1, 0));
        v = translate(v, -position);
        mvp = p * v * m;

        sp.use();
        glUniformMatrix4fv(sp.loc("mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(sp.loc("m"), 1, GL_FALSE, &m[0][0]);
        glUniformMatrix4fv(sp.loc("v"), 1, GL_FALSE, &v[0][0]);


        // DEBUG: Update light point's position
        float light_angle = t;
        vec3 lp = vec3(cos(light_angle), 1.7f, sin(light_angle));

        glUniform3f(sp.loc("light_pos"), lp.x, lp.y, lp.z);

        // Pass bone matrices
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(mat4)*mbs.size(), &mbs[0][0][0]);

        // Draw model with default flags
        glUniform1i(sp.loc("flags"), 0);

        for (auto &vao : model.vaos) {
            glBindVertexArray(vao.index);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);

            glBindTexture(GL_TEXTURE_2D, model.materials[vao.material].tex_id);
            
            glDrawElements(GL_TRIANGLES, vao.count, GL_UNSIGNED_SHORT, (void*)0);
        }

        
        // DEBUG
        //#define SHOW_BONES
        #define SHOW_LIGHT
        draw_debug(sp, model, vbo_bones, vao_bones, lp);


        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double dt = (now - last);
        last = now;
        //this_thread::sleep_for(chrono::milliseconds((int)(1000.f/fps_limit-dt*1000.f)));
        sleep( (int)(1000 * (1.f / fps_limit - dt)) );

        position += (float)(dt * speed) * vel;
        t += 1.f/fps_limit;
    }
    glfwTerminate();

    return 0;
}

void draw_debug(Shader &sp, M &model, GLuint vbo_bones, GLuint vao_bones, vec3 lp) {
    
    #ifdef SHOW_BONES
        vector<GLfloat> vv;
        for (pair<const int, Bone> &b : (*model.bones)) {
            vec4 v = vec4(0, 0, 0, 1);
            vec4 w = vec4(0, 100, 0, 1);
            mat4 tr = compute_mn(&b.second);
            v = tr * v;
            w = tr * w;
            vv.push_back(v.x);
            vv.push_back(v.y);
            vv.push_back(v.z);
            vv.push_back(w.x);
            vv.push_back(w.y);
            vv.push_back(w.z);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_bones);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vv.size() * sizeof(GLfloat), vv.data());

        glUniform1i(sp.loc("flags"), SOLID_COLOR | NO_SHADE | NO_BONES);
        glUniform3f(sp.loc("u_col"), 1, 0, 0);
        
        glBindVertexArray(vao_bones);
        glEnableVertexAttribArray(0);

        glLineWidth(3);
        glDrawArrays(GL_LINES, 0, vv.size());
        glLineSize(1);
        #endif

        #ifdef SHOW_LIGHT
        m = translate(mat4(1), lp);
        mvp = p * v * m;
        glUniformMatrix4fv(sp.loc("mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(sp.loc("m"), 1, GL_FALSE, &m[0][0]);
        glUniform1i(sp.loc("flags"), SOLID_COLOR | NO_SHADE | NO_BONES);
        glUniform3f(sp.loc("u_col"), 1, 1, 1);

        glBindVertexArray(vao_dot);

        glPointSize(10);
        glDrawArrays(GL_POINTS, 0, 1);
        glPointSize(1);
        #endif
}