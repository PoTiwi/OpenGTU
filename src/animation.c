#include <string.h>
#include "../include/animation.h"


void anim_init(AnimationSystem *sys) {
    memset(sys, 0, sizeof(*sys));
}

int anim_create(AnimationSystem *sys, int obj_id, bool looping) {
    if (sys->count >= MAX_ANIMATIONS) return -1;
    int id = sys->count++;
    Animation *a = &sys->clips[id];
    memset(a, 0, sizeof(*a));
    a->obj_id  = obj_id;
    a->looping = looping;
    return id;
}

void anim_add_key(AnimationSystem *sys, int anim_id, Keyframe key) {
    if (anim_id < 0 || anim_id >= sys->count) return;
    Animation *a = &sys->clips[anim_id];
    if (a->key_count >= MAX_KEYFRAMES) return;
    a->keys[a->key_count++] = key;
    if (key.time > a->duration) a->duration = key.time;
}

void anim_play(AnimationSystem *sys, int anim_id) {
    if (anim_id < 0 || anim_id >= sys->count) return;
    sys->clips[anim_id].playing = true;
    sys->clips[anim_id].elapsed = 0.0f;
}

void anim_stop(AnimationSystem *sys, int anim_id) {
    if (anim_id < 0 || anim_id >= sys->count) return;
    sys->clips[anim_id].playing = false;
}

void anim_update(AnimationSystem *sys, Scene *s, float dt) {
    for (int ai=0; ai<sys->count; ai++) {
        Animation *a = &sys->clips[ai];
        if (!a->playing || a->key_count < 2) continue;

        a->elapsed += dt;
        if (a->elapsed > a->duration) {
            if (a->looping) a->elapsed = fmodf(a->elapsed, a->duration);
            else { a->playing=false; a->elapsed=a->duration; }
        }

        Keyframe *k0=&a->keys[0], *k1=&a->keys[a->key_count-1];
        for (int i=0; i<a->key_count-1; i++) {
            if (a->elapsed >= a->keys[i].time &&
                a->elapsed <= a->keys[i+1].time) {
                k0=&a->keys[i]; k1=&a->keys[i+1]; break;
            }
        }

        float span=k1->time-k0->time;
        float t=span>0.0f?(a->elapsed-k0->time)/span:0.0f;

        Object *obj=scene_find_object(s, a->obj_id);
        if (!obj) continue;

        obj->position = v3_lerp(k0->position, k1->position, t);
        obj->rotation = v3_lerp(k0->rotation, k1->rotation, t);
        obj->scale    = v3_lerp(k0->scale,    k1->scale,    t);
        object_update_matrix(obj);
    }
}
