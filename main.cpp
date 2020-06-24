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
    const char *model_fname = argc < 2 ? "models/wssi.glb" : argv[1];
    //M model = loadModel(model_fname, context);

    AnimatedModel amodel;
    amodel.load(model_fname);
    amodel.set_scale(0.4f);

    Renderer r(player);
    Museum museum;

    ModelInstance mi(amodel, 0), mi2(amodel, 2);

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
    //vec2 dest[dlugosTrasy] ={vec2(1,1),vec2(2,-0.7),vec2(4,1),vec2(4.5,-1),vec2(6.5,1),vec2(4.5,-1)};
    static const int dlugosTrasy=12;

    vec2 dest[dlugosTrasy] ={vec2(1,1),vec2(2,-0.7),vec2(3.8,0.9),vec2(4.5,-0.7),vec2(6.5,1),vec2(8.5,-0.7),vec2(11.5,0.8),vec2(11.5,-0.7),vec2(13.3, 0.7),vec2(13.5, -0.6), vec2(13.5,0.0),vec2(0.0,0.0)};
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
        //rend.draw_debug(sp, model, vbo_bones, vao_bones, lp);

        if (cmd_go)
            mi.goTo(vec2(5, 0));
        else if (cmd_stop)
            mi.goTo(vec2(0, 0));


        player.update(dt);
        mi.update(dt);
        mi2.update(dt);

      
        r.use_shader(Static);
        museum.draw(r);
   

        r.use_shader(Lights);
        museum.drawLight(r);
        

        r.use_shader(Character);
        r.i();
        r.tr(vec3(-10.5,0,0));
        r.tr(mi.pos);
        r.ro(mi.rot/3.14f*180, Y);
        mi.draw(r);

        r.i();
        r.tr(vec3(-10.5,0,0));
        r.tr(mi2.pos);
        r.ro(mi2.rot/3.14f*180, Y);
        mi2.draw(r);
        


        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double dt = (now - last);
        last = now;
        //this_thread::sleep_for(chrono::milliseconds((int)(1000.f/fps_limit-dt*1000.f)));
        sleep( (int)(1000 * (1.f / fps_limit - dt)) );

        position += (float)(dt * speed) * vel;
    //        if(mi.reachedGoal()){
    //     if(!startedToWatch){
    //     watchPaintingTimer=glfwGetTime();
    //     startedToWatch=true;
    //     }
    //     if(glfwGetTime()- watchPaintingTimer >= 1.0){
    //         startedToWatch=false;
    //         mi.goTo(dest[i]);
    //         cout<<"ide do "<<i<<endl;
    //         i++;
    //         if(i>=dlugosTrasy)
    //             i=0;
    //     }
        
    // }
        //t += 1.f/fps_limit;
    }
    glfwTerminate();

    return 0;
}

