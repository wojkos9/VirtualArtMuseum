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

using namespace std;
using namespace glm;

static int win_w = 640, win_h = 480;



vec3 position(0, 0, 3);
vec3 vel(0);
vec3 dir(0, 0, -1);
float cam_angle_x = 0.f, cam_angle_y = 0.f;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    struct key_struct {
        int code;
        vec3 action;
        bool pressed = false;
        key_struct(int code, vec3 action) : code(code), action(action) {}
    };
    static key_struct keys[] = {
    {glfwGetKeyScancode(GLFW_KEY_W), vec3(0, 0, -1)}, 
    {glfwGetKeyScancode(GLFW_KEY_A), vec3(-1, 0, 0)}, 
    {glfwGetKeyScancode(GLFW_KEY_S), vec3(0, 0, 1)}, 
    {glfwGetKeyScancode(GLFW_KEY_D), vec3(1, 0, 0)},
    {glfwGetKeyScancode(GLFW_KEY_SPACE), vec3(0, 1, 0)},
    {0x32, vec3(0, -1, 0)},
    };
    vec3 vel_tmp(0);
    for (key_struct &k : keys) {
        if (scancode == k.code) {
            k.pressed = (action != GLFW_RELEASE);
            
        }
        if (k.pressed) {
            vel_tmp += k.action;
        }
        
    }
    if (mods & GLFW_MOD_CONTROL) {
        vel_tmp *= 0.1;
    }

    vel = -vel_tmp.z * dir + vel_tmp.x * cross(dir, vec3(0, 1, 0)) + vec3(0, vel_tmp.y, 0);
}

void mouse_callback(GLFWwindow *win, double xpos, double ypos) {
    float sc = 0.001;
    cam_angle_x += sc*(xpos-win_w/2);
    cam_angle_y -= sc*(ypos-win_h/2);
    glfwSetCursorPos(win, win_w/2, win_h/2);
    dir = vec3(sin(cam_angle_x)*cos(cam_angle_y), sin(cam_angle_y), -cos(cam_angle_x)*cos(cam_angle_y));
}



int main() {
    
    glewExperimental = 1;
    if (! glfwInit()) {
        printf("GLFW fail\n");
    }

    GLFWwindow* win;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    win = glfwCreateWindow(win_w, win_h, "My Window", NULL, NULL);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPos(win, win_w/2, win_h/2);
    glfwSetCursorPosCallback(win, mouse_callback);
    

    
    glfwMakeContextCurrent(win);

    GLint maj, min;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);

    printf("Version %d.%d\n", maj, min);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    printf("Vendor: %s\n", vendor);

    if (glewInit() != GLEW_OK) {
        printf("GLEW fail\n");
    }

    GLuint sp = make_program("shaders/shaded.vsh", "shaders/shaded.fsh");

    
    glClearColor(0.6f, 0.5f, 8, 1);
    float fps_limit = 60.f;
    
    Model context;
    
    M model = loadModel("/home/wojtek/walk.glb", context);
    int x = 1;
    for (pair<const int, Bone> &b : *model.bones) {
        b.second.mn = b.second.ms ;//* rotate(mat4(1), radians(45.f), vec3(0, 0, 1));
        x--;
    }
    //(*model.bones)[1].mn = (*model.bones)[1].ms * rotate(mat4(1), radians(30.f), vec3(0, 0, 1));
    int n_bones = (*model.bones).size();
    vector<mat4> mbs(n_bones, mat4());

    mat4 m(1);//= scale(mat4(1), vec3(0.01f, 0.01f, 0.01f));
    mat4 p = perspective(radians(110.f), (float)win_w/win_h, 0.1f, 500.f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    float speed = 1.f;

    float t = 0.f;
    glPointSize(5);

    float xy[] = {0.45f, 0.25f, 0.65f, 0.45f};

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
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {

        //animate((*model.bones), context, t, mbs);

        int i = 0;
        for(int j : context.skins[0].joints) {
            mbs[i] = compute_mn(&(*model.bones)[j]) * *(*model.bones)[j].ob;
            i++;
        }

        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 v = lookAt(vec3(0, 0, 0), dir, vec3(0, 1, 0));
        //m = translate(mat4(1), -position);
        v = translate(v, -position);

        mat4 mvp = p * v * m;

        float an = t;
        vec3 lp = vec3(cos(an), 3, sin(an));

        glUseProgram(sp);

        glUniformMatrix4fv(glGetUniformLocation(sp, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sp, "m"), 1, GL_FALSE, &m[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sp, "v"), 1, GL_FALSE, &v[0][0]);
        glUniform3f(glGetUniformLocation(sp, "light_pos"), lp.x, lp.y, lp.z);
        
        glUniform1i(glGetUniformLocation(sp, "use_bones"), 1);
        glUniform3f(glGetUniformLocation(sp, "u_col"), 1, 1, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(mat4)*mbs.size(), &mbs[0][0][0], GL_STREAM_DRAW);

        for (auto &vao : model.vaos) {
            // if (vao.index != 3) // eyelashes
            //     continue;
            glBindVertexArray(vao.index);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);

            glBindTexture(GL_TEXTURE_2D, model.materials[vao.material].tex_id);
            //printf("%d\n", vao.material);
            

            glDrawElements(
                GL_TRIANGLES,      // mode
                vao.count,    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );
        }
        
        glUniform1i(glGetUniformLocation(sp, "use_bones"), 0);
        glUniform3f(glGetUniformLocation(sp, "u_col"), 1, 0, 0);

        glBindVertexArray(vao);
        

        vector<GLfloat> vv;
        vec4 lv = vec4(0, 0, 0, 1);
        i = 0;
        // for (pair<const int, Bone> &b : (*model.bones)) {
        //     vec4 v = vec4(0, 0, 0, 1);
        //     vec4 w = vec4(0, 100, 0, 1);
        //     mat4 tr = compute_mn(&b.second);
        //     v = tr * v;
        //     w = tr * w;
        //     vv.push_back(v.x);
        //     vv.push_back(v.y);
        //     vv.push_back(v.z);
        //     vv.push_back(w.x);
        //     vv.push_back(w.y);
        //     vv.push_back(w.z);
        //     i++;
        // }
        vv.push_back(0);
        vv.push_back(0);
        vv.push_back(0);
        vv.push_back(lp.x);
        vv.push_back(lp.y);
        vv.push_back(lp.z);


        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vv.size() * sizeof(GLfloat), vv.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_LINES, 0, vv.size());

        // glBindVertexArray(quad);
        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(3);
        // glBindTexture(GL_TEXTURE_2D, model.materials[0].tex_id);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);


        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double dt = (now - last);
        last = now;
        this_thread::sleep_for(chrono::milliseconds((int)(1000.f/fps_limit-dt*1000.f)));

        position += (float)(dt * speed) * vel;
        t += 1.f/fps_limit;
    }
    glfwTerminate();

    return 0;
}