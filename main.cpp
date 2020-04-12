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



vec3 position(0, 0, 3);
float cam_angle_x = 0.f, cam_angle_y = 0.f;
vec3 vel(0);
vec3 dir(0, 0, -1);


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

    vel = -vel_tmp.z * dir + vel_tmp.x * cross(dir, vec3(0, 1, 0)) + vec3(0, vel_tmp.y, 0);
}

void mouse_callback(GLFWwindow *win, double xpos, double ypos) {
    float sc = 0.001;
    cam_angle_x += sc*(xpos-320);
    cam_angle_y -= sc*(ypos-240);
    glfwSetCursorPos(win, 320, 240);
    dir = vec3(sin(cam_angle_x)*cos(cam_angle_y), sin(cam_angle_y), -cos(cam_angle_x)*cos(cam_angle_y));
}

mat4 compute_inv_ob(Bone *b) {
    mat4 ret = b->parent==nullptr ? b->ms : compute_mn(b->parent) * b->ms;
    return ret;
}


int main() {
    
    glewExperimental = 1;
    if (! glfwInit()) {
        printf("GLFW fail\n");
    }

    GLFWwindow* win;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    win = glfwCreateWindow(640, 480, "My Window", NULL, NULL);

    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPos(win, 320, 240);
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

    GLuint vs = make_shader("basic.vsh", GL_VERTEX_SHADER);
    GLuint fs = make_shader("basic.fsh", GL_FRAGMENT_SHADER);

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
    glClearColor(0, 0, 0, 1);
    float fps_limit = 60;
    
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
    // vector<mat4> obs(n_bones, mat4());
    // for (pair<const int, Bone> &b : *model.bones) {
    //     if (b.first == 74)
    //         continue;
    //     mat4 ob = inverse(compute_inv_ob(&b.second));
    //     printf("OB BONE %d\n", b.first);
    //     printm(ob);
    //     printm(*b.second.ob);
    //     *b.second.ob = ob;
    // }

    mat4 m(1);//= scale(mat4(1), vec3(0.01f, 0.01f, 0.01f));
    mat4 p = perspective(radians(110.f), 640.f/480, 0.1f, 10.f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    float speed = 1.f;

    float t = 0.f;
    glPointSize(5);
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {

        animate((*model.bones), context, t, mbs);
        // for (auto &b : (*model.bones)) {
        //  b.second.mn = b.second.ms * rotate(mat4(1), (float)sin(t)/16, vec3(0, 1, 1));
        // }
        int i = 0;
        for(int j : context.skins[0].joints) {
            mbs[i] = compute_mn(&(*model.bones)[j]) * *(*model.bones)[j].ob;
            i++;
            //printf("MAT%d:\n", j);
            //printm(mbs[j]);
        }

        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 v = lookAt(vec3(0, 0, 0), 
        dir, 
        vec3(0, 1, 0));
        v = translate(v, -position);
        mat4 mvp = p * v * m;

        glUseProgram(sp);

        glUniformMatrix4fv(glGetUniformLocation(sp, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniform1i(glGetUniformLocation(sp, "use_bones"), 1);
        glUniform3f(glGetUniformLocation(sp, "u_col"), 1, 1, 0);
        //glUniform1f(glGetUniformLocation(sp, "a"), 1.f - t / 5);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(mat4)*mbs.size(), &mbs[0][0][0], GL_STREAM_DRAW);

        glBindVertexArray(model.vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        
        
        glDrawElements(
            GL_TRIANGLES,      // mode
            model.count,    // count
            GL_UNSIGNED_SHORT,   // type
            (void*)0           // element array buffer offset
        );
        glUniform1i(glGetUniformLocation(sp, "use_bones"), 0);
        glUniform3f(glGetUniformLocation(sp, "u_col"), 1, 0, 0);

        glBindVertexArray(vao);
        

        vector<GLfloat> vv;
        vec4 lv = vec4(0, 0, 0, 1);
        i = 0;
        for (pair<const int, Bone> &b : (*model.bones)) {
            vec4 v = vec4(0, 0, 0, 1);
            vec4 w = vec4(0, 100, 0, 1);
            mat4 tr = compute_mn(&b.second);
            v = tr * v;
            w = tr * w;
            //printf("%d: %f %f %f\n", i, v.x, v.y, v.z);
            vv.push_back(v.x);
            vv.push_back(v.y);
            vv.push_back(v.z);
            vv.push_back(w.x);
            vv.push_back(w.y);
            vv.push_back(w.z);
            i++;
        }
        vv.push_back(0);
        vv.push_back(0);
        vv.push_back(0);


        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vv.size() * sizeof(GLfloat), vv.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_LINES, 0, vv.size());

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

int main2() {
    glewExperimental = 1;
    if (! glfwInit()) {
        printf("GLFW fail\n");
    }

    GLFWwindow* win;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    win = glfwCreateWindow(640, 480, "My Window", NULL, NULL);

    
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

    GLuint vs = make_shader("basic_bones.vsh", GL_VERTEX_SHADER);
    GLuint fs = make_shader("basic_bones.fsh", GL_FRAGMENT_SHADER);

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

    

    static const GLfloat verts[] = {
        -.75f, -.25f, 0, 1, 0,
        0, .25f, 0, .5f, .5f,
        -.75f, .25f, 0, 1, 0,

        -.75f, -.25f, 0, 1, 0,
        0, -.25f, 0, .5f, .5f,
        0, .25f, 0, .5f, .5f,

        0.f, -.25f, 0, .5f, .5f,
        0.75f, .25f, 0, 1, 0,
        0.f, .25f, 0, .5f, .5f,

        0.f, -.25f, 0, .5f, .5f,
        0.75f, -.25f, 0, 1, 0,
        0.75f, .25f, 0, 1, 0,

        -.75, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        .75, 0, 0, 1, 0,
    };

    static const GLint bone_idx[] {
        0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1,

        1, 0, 1, 0, 1, 0,
        1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 1, 0
    };

    static const GLfloat g_vertex_buffer_data[] = {
        -.5f, -.5f, 0.0f, 1, 0, 0,
        .5f, -.5f, 0.0f, 0, 1, 0,
        0.0f,  .5f, 0.0f, 0, 0, 1
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo[2];
    glGenBuffers(2, &vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bone_idx), bone_idx, GL_STATIC_DRAW);

    glVertexAttribIPointer(2, 2, GL_INT, 2*sizeof(GLint), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    

    printf("%d %d\n", vao, vbo[0]);

    
    
    glPointSize(5);
    //glClearColor(0.3f, 0.7f, 0.5f, 1.0f);
    glClearColor(0, 0, 0, 1);
    mat4 bone(1);

    mat4 r1 = rotate(mat4(1), radians(10.f), vec3(0, 0, 1));
    mat4 r2 = rotate(mat4(1), radians(35.f), vec3(0, 0, 1));
    

    mat4 ms1 = translate(mat4(1), vec3(-0.75f, 0, 0));
    mat4 ms2 = translate(mat4(1), vec3(0.75f, 0, 0));

    mat4 ob1 = inverse(ms1);
    mat4 ob2 = inverse(ms2) * ob1;

    mat4 mn1 = ms1 * r1;
    mat4 mn2 = ms2 * r2;

    mat4 mb1 = mn1 * ob1;
    mat4 mb2 = ms1 * r1 * ms2 * r2 * ob2;

    mat4 bones[2] = {mb1, mb2};

    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint bone1= glGetUniformLocation(sp, "bone1");
    GLuint bone2= glGetUniformLocation(sp, "bone2");
    GLuint bones_loc= glGetUniformLocation(sp, "bones");

    


    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(bones), 0, GL_STREAM_DRAW);
    

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

    
    glBindVertexArray(0);

    GLuint printBuffer = createPrintBuffer();
    bindPrintBuffer(sp, printBuffer);

    float t = 0;

    float fps_limit = 60;

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        t += 0.05f;

        r1 = rotate(mat4(1), (float)sin(t)*3.14159f/4, vec3(0, 0, 1));
        r2 = rotate(mat4(1), (float)sin(t)*3.14159f/2, vec3(0, 0, 1));
        mat4 t2 = translate(mat4(1), vec3((float)sin(t/3), 0, 0));

        mat4 mn1 = ms1 * r1;
        mat4 mn2 = ms2 * t2 * r2;

        mat4 mb1 = mn1 * ob1;
        mat4 mb2 = mn1 * mn2 * ob2;

        mat4 bones[] = {mb1, mb2};

        glUseProgram(sp);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(vao);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(bones), bones);
        
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(win);
        glfwPollEvents();

        double now = glfwGetTime();
        double elapsed = (now - last)*1000;
        last = now;
        this_thread::sleep_for(chrono::milliseconds((int)(1000.f/fps_limit-elapsed)));
    }
    glfwTerminate();

    return 0;
}