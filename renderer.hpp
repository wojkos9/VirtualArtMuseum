#pragma once
#include "model.hpp"
#include "shader.hpp"
#include "camera.hpp"

enum ShaderType {
    Character = 0, Static = 1, Lights = 2
};

enum Axis {
    X, Y, Z
};



class Renderer {
    
    mat4 mvp, m;

    GLuint ssbo;
    vector<Shader> shaders;

    float t = 0.f;
    Shader *current_shader;

    bool m_dirty = true;
    
    vec3 world_axes[3] = {vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)};
    
    public:

    

    MCamera &camera;
    
    Renderer(MCamera &camera) : camera(camera), shaders(sizeof(ShaderType)) {

        add_shader(Character, "shaders/shaded.vsh", "shaders/shaded.fsh");
        add_shader(Static, "shaders/vertexShaderMuseum.vs", "shaders/fragmentShaderMuseum.fs");
        add_shader(Lights, "shaders/lightCubeShader.vs", "shaders/lightCubeShader.fs");
        // GPU buffer for bone matrices

        int n_bones = 128;
        
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, n_bones * sizeof(mat4), NULL, GL_STREAM_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        
    }

    void add_shader(ShaderType st, const char *vertex_fname, const char *fragment_fname) {
        shaders[st] = Shader(vertex_fname, fragment_fname);
    }

    void use_shader(ShaderType st) {
        i();
        m_dirty = true;
        (current_shader = &shaders[st])->use();
    }

    void passMatrices() {
        mat4& v = camera.get_v();
        mat4& p = camera.get_p();
        mvp = p * v * m;

        glUniformMatrix4fv(current_shader->loc("mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(current_shader->loc("m"), 1, GL_FALSE, &m[0][0]);
        glUniformMatrix4fv(current_shader->loc("v"), 1, GL_FALSE, &v[0][0]);
        //glUniform3f (current_shader->loc("lightPos"),  2.5f,  0.99f,  0.0f);

        m_dirty = false;
    }

    void setLightPositions(vec3 lp[4]){
        glUniform3f (current_shader->loc("lightPos[0]"),  lp[0].x, lp[0].y, lp[0].z);
        glUniform3f (current_shader->loc("lightPos[1]"),  lp[1].x, lp[1].y, lp[1].z);

        glUniform3f (current_shader->loc("lightPos[2]"),  lp[2].x, lp[2].y, lp[2].z);

        glUniform3f (current_shader->loc("lightPos[3]"),  lp[3].x, lp[3].y, lp[3].z);

    }
    void i() {
        m = mat4(1);
        m_dirty = true;
    }
    void tr(vec3 t) {
        m = translate(m, t);
        m_dirty = true;
    }
    void ro(float alpha, Axis ax) {
        m = rotate(m, radians(alpha), world_axes[ax]);
        m_dirty = true;
    }
    void sc(vec3 s) {
        m = scale(m, s);
        m_dirty = true;
    }
    void sc(float s) {
        m = scale(m, vec3(s, s, s));
        m_dirty = true;
    }

    void updateLight(float dt) {
        t += dt;
      
        // DEBUG: Update light point's position
        float light_angle = t;
        vec3 lp = vec3(cos(light_angle), 1.7f, sin(light_angle));

        glUniform3f(current_shader->loc("light_pos"), lp.x, lp.y, lp.z);
    }

    void setCameraPosition(){
         glUniform3fv(current_shader->loc("viewPos"),1, &camera.position[0]);

    }
    bool dirty() {
        return m_dirty || camera.m_dirty;
    }

    void passBoneMatrices(vector<mat4> &mbs) {
        // Pass bone matrices
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(mat4)*mbs.size(), &mbs[0][0][0]);
    }

    void renderModel(AnimatedModel &model, unsigned int flags=0) {
        sc(model.scale);
        if (dirty())
            passMatrices();
         

        // Draw model with default flags
        glUniform1i(current_shader->loc("flags"), flags);

        for (auto &vao : model.vaos) {
            // if (vao.name=="Head")
            //     continue;
            glBindVertexArray(vao.index);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);

            glBindTexture(GL_TEXTURE_2D, model.materials[vao.material].tex_id);
            setCameraPosition();
            glm::vec3 lightPositions []={
            glm::vec3( 7.5f,  0.5f,  0.0f),
            glm::vec3( 2.5f,  0.5f,  0.0f),
            glm::vec3( -2.5f,  0.5f,  0.0f),
            glm::vec3( -7.5f,  0.5f,  0.0f)
        };
                
            setLightPositions(lightPositions);
            glDrawElements(GL_TRIANGLES, vao.count, GL_UNSIGNED_SHORT, (void*)0);
        }
        sc(1/model.scale);
    }


    void renderTriangles(int n) {
        if (m_dirty)
            passMatrices();
        glDrawArrays(GL_TRIANGLES, 0, n);
    }

    // void draw_debug(Shader &sp, M &model, GLuint vbo_bones, GLuint vao_bones, vec3 lp) {
    
    //     #ifdef SHOW_BONES
    //     vector<GLfloat> vv;
    //     for (pair<const int, Bone> &b : (*model.bones)) {
    //         vec4 v = vec4(0, 0, 0, 1);
    //         vec4 w = vec4(0, 100, 0, 1);
    //         mat4 tr = compute_mn(&b.second);
    //         v = tr * v;
    //         w = tr * w;
    //         vv.push_back(v.x);
    //         vv.push_back(v.y);
    //         vv.push_back(v.z);
    //         vv.push_back(w.x);
    //         vv.push_back(w.y);
    //         vv.push_back(w.z);
    //     }

    //     glBindBuffer(GL_ARRAY_BUFFER, vbo_bones);
    //     glBufferSubData(GL_ARRAY_BUFFER, 0, vv.size() * sizeof(GLfloat), vv.data());

    //     glUniform1i(sp.loc("flags"), SOLID_COLOR | NO_SHADE | NO_BONES);
    //     glUniform3f(sp.loc("u_col"), 1, 0, 0);
        
    //     glBindVertexArray(vao_bones);
    //     glEnableVertexAttribArray(0);

    //     glLineWidth(3);
    //     glDrawArrays(GL_LINES, 0, vv.size());
    //     glLineSize(1);
    //     #endif

    //     #ifdef SHOW_LIGHT
    //     m = translate(mat4(1), lp);
    //     mvp = p * v * m;
    //     glUniformMatrix4fv(sp.loc("mvp"), 1, GL_FALSE, &mvp[0][0]);
    //     glUniformMatrix4fv(sp.loc("m"), 1, GL_FALSE, &m[0][0]);
    //     glUniform1i(sp.loc("flags"), SOLID_COLOR | NO_SHADE | NO_BONES);
    //     glUniform3f(sp.loc("u_col"), 1, 1, 1);

    //     glBindVertexArray(vao_dot);

    //     glPointSize(10);
    //     glDrawArrays(GL_POINTS, 0, 1);
    //     glPointSize(1);
    //     #endif
    // }
};