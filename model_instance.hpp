#pragma once
#include "model.hpp"
#include "animate.hpp"
#include <queue>
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

enum AnimType {
    Idle=0, Start, Stop, Walk, Nothing, Rotate
};
enum TaskType {
    WalkTo, Wait
};

class Task {
    public:
    TaskType tt;
    vec2 dst;
    float time;
    Task(TaskType tt, vec2 dst, float time) : tt(tt), dst(dst), time(time) {}
};


class ModelInstance {

public:
    float t = 0;
    map<int, Bone> *bones;
    Model &ctx;
    vector<mat4> mbs;
    AnimatedModel &amodel;
    bool do_animation = true;
    bool working = true;
    AnimType curr_anim = Nothing;
    int cycles = 1;
    vec3 pos;
    float d_since = 0.f;
    float last_d = 0.f;
    queue<AnimType> animations;
    queue<Task> tasks;
    bool walking = false;

    vector<vec2> path = {vec2(0, 0), vec2(1,1),vec2(2,-0.7), vec2(3.8,0.9), vec2(4.5,-0.7),
    vec2(6.5,1),vec2(8.5,-0.7),vec2(11.5,0.8),vec2(11.5,-0.7),
    vec2(13.3, 0.7),vec2(13.5, -0.6), vec2(13.5,0.0)};
    
    int ipath = 0;
    bool walking_path = true;
    int watch_iter = 1;

    float rot = 0.f;
    float target_rot = 0.f;
    bool rotating = false;
    float rot_speed = 1.f;
    bool clockwise = true;
    bool reached = true;

    vec2 dir = vec2(0, 1);
    vec2 target = vec2(0);

    int hips_node = 64;

    void stop() {
        if (walking) {
            animations.push(Stop);
            animations.push(Idle);
            cout << "stop" << endl;
            walking = false;
        }
        
    }

    bool reachedGoal() {
        return reached;
    }

    void rotateTo(float rad) {
        cout << "ROT: " << rad << endl;
        target_rot = rad;
        clockwise = target_rot > rot;
        animations.push(Rotate);
    }
    int anim_counter = 0;
    bool debug = false;
    void nextAnimation() {
        if (animations.size() > 0) {
            curr_anim = animations.front();
            if (curr_anim == Idle) {
                if (debug)
                    cout << "POS" << pos.x << " " << pos.z << endl;
            }
            anim_counter++;
            last_d = 0;
            t = 0;
            animations.pop();
            cout << "anim " << curr_anim << "/" << animations.size() << endl;
        }
    }

    void update(float dt) {

        if (curr_anim == Rotate) {
            rot += rot_speed * (2*clockwise-1) * dt;
            if (rot >= target_rot && clockwise || rot <= target_rot && (!clockwise)) {
                rot = target_rot;
                nextAnimation();
            }
        } else {
                      
            // Animate bones
            if (curr_anim != Nothing) {
                t += dt;
                working = animate(*bones, ctx, t, &d_since, curr_anim, hips_node);

                if (curr_anim != Idle) {
                    float dr = (d_since>last_d)?(d_since-last_d):d_since;
                    dr = dr*0.005;
                    pos += rotateY(vec3(0, 0, dr), rot);
                    if (distance(vec2(pos.x, pos.z), target) <= 0.5f) {
                        stop();
                        reached = true;
                    }
                } else if (reached) {
                    if (!working) {
                        cout << "iter" << watch_iter << endl;
                        watch_iter--;
                    }
                        
                    if (watch_iter == 0) {
                        nextDestination();
                    }
                }
                
                
                last_d = d_since;
            } else {
                working = false;
                t = 0;
                last_d = 0;
            }
            if (!working) {
                nextAnimation();
            }
        }
        

        // Compute bone matrices
        int i = 0;
        for(int j : ctx.skins[0].joints) {
            mbs[i] = compute_mn(&(*bones)[j]) * *(*bones)[j].ob;
            i++;
        }
    }

    void start() {
        if (!walking) {
            animations.push(Start);
            animations.push(Walk);
            cout << "start" << endl;
            walking = true;
        }
    }

    void goTo(vec2 dst) {
        vec2 p = vec2(pos.x, pos.z);
        target = dst;
        dir = normalize(dst-p);
        float new_rot = acos(dot(dir, vec2(0, 1))) * sign(dir.x);
        cout << "ROT" << new_rot << endl;
        rotateTo(new_rot);
        start();
        reached = false;
    }

    void setLocation(vec2 p) {
        pos.x = p.x;
        pos.z = p.y;
    }

    void nextDestination() {
        ipath = (ipath+1)%path.size();

        // correct position when not visible (behind the door)
        if (ipath == 1)
            setLocation(path[0]);

        goTo(path[ipath]);
    }
    void startFrom(int i) {
        ipath = i<path.size()?i:0;
        pos = vec3(path[ipath].x, -0.5f, path[ipath].y);
        nextDestination();
    }
    
    ModelInstance(AnimatedModel &am, int i=0) : amodel(am), ctx(am.ctx) {
        startFrom(i);
        bones = copyBones(am.bones);
        prepareBones();
        
    }
    void prepareBones() {

        // CPU buffer for bone matrices
        int n_bones = bones->size();
        mbs = vector<mat4>(n_bones, mat4());
    }
    void draw(Renderer &r) {
        r.tr(pos);
        r.ro(rot/3.14f*180, Y);
        r.passBoneMatrices(mbs);
        r.renderModel(amodel);
    }
};