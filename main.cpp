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
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
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
    const char *model_fname = argc < 2 ? "models/wssi.glb" : argv[1];

    AnimatedModel amodel;
    amodel.load(model_fname);
    amodel.set_scale(0.4f);

    Renderer r(player);
    Museum museum;

    vector<ModelInstance> npcs;
    npcs.push_back(ModelInstance(amodel, 0));
    npcs.push_back(ModelInstance(amodel, 3));
    npcs.push_back(ModelInstance(amodel, 6));
    npcs.push_back(ModelInstance(amodel, 8));

    npcs[0].debug = true;

    
    // DEBUG: Dot light
    vao_dot = gen_dot();

    // Some models render incorrectly when this is on:
    // glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    //vec2 dest[dlugosTrasy] ={vec2(1,1),vec2(2,-0.7),vec2(4,1),vec2(4.5,-1),vec2(6.5,1),vec2(4.5,-1)};
    int i=0;
    bool startedToWatch=false;
    float watchPaintingTimer = glfwGetTime();
    
    double last = glfwGetTime();
    float dt = 1.f/fps_limit;
    float dx = 0;
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DEBUG
        //#define SHOW_BONES
        #define SHOW_LIGHT


        player.update(dt);
        for (ModelInstance &mi : npcs) {
            mi.update(dt);
        }

      
        r.use_shader(Static);
        museum.draw(r);
   

        r.use_shader(Lights);
        museum.drawLight(r);
        

        r.use_shader(Character);

        for (ModelInstance &mi : npcs) {
            r.i();
            r.tr(vec3(-10.5,0,0));
            mi.draw(r);
        }
        


        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double dt = (now - last);
        last = now;

        sleep( (int)(1000 * (1.f / fps_limit - dt)) );


    }
    glfwTerminate();

    return 0;
}

