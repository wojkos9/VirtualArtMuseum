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

#include "renderer.hpp"
#include "museum.hpp"
#include "human.hpp"

#include "model_instance.hpp"

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

static GLuint vao_dot;

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
    
    glClearColor(0.6f, 0.5f, 8, 1);
    
    Model context;
    
    // TODO: in a different thread
    const char *model_fname = argc < 2 ? "models/walk.glb" : argv[1];
    //M model = loadModel(model_fname, context);

    AnimatedModel amodel;
    amodel.load(model_fname);
    amodel.set_scale(0.4f);

    Renderer r(player);
    Museum museum;
    
    Human npc(amodel);

    ModelInstance mi(amodel), mi2(amodel);

    // // DEBUG: for visualizing bones
    // GLuint vao_bones;
    // glGenVertexArrays(1, &vao_bones);
    // glBindVertexArray(vao_bones);

    // GLuint vbo_bones;
    // glGenBuffers(1, &vbo_bones);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_bones);
    // int sz = (*model.bones).size() * 2*sizeof(vec3) + sizeof(vec3);
    // glBufferData(GL_ARRAY_BUFFER, sz, 0, GL_STREAM_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    
    // DEBUG: Dot light
    vao_dot = gen_dot();

    // Some models render incorrectly when this is on:
    // glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    
    double last = glfwGetTime();
    float dt = 1.f/fps_limit;
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DEBUG
        //#define SHOW_BONES
        #define SHOW_LIGHT
        //rend.draw_debug(sp, model, vbo_bones, vao_bones, lp);
        player.update(dt);
        mi.update(dt);
        mi2.update(1.3f*dt);

      
        r.use_shader(Static);
        museum.draw(r);
   

        r.use_shader(Lights);
        museum.drawLight(r);
        

        r.use_shader(Character);
        r.i();
        r.tr(vec3(0, -0.5f, 0));
        mi.draw(r);
        r.tr(vec3(1, 0, 0));
        mi2.draw(r);


        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double dt = (now - last);
        last = now;
        //this_thread::sleep_for(chrono::milliseconds((int)(1000.f/fps_limit-dt*1000.f)));
        sleep( (int)(1000 * (1.f / fps_limit - dt)) );

        position += (float)(dt * speed) * vel;
        //t += 1.f/fps_limit;
    }
    glfwTerminate();

    return 0;
}

