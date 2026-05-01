#ifndef OPENGTU_ANIMATION_H
#define OPENGTU_ANIMATION_H

#include "object.h"

#define MAX_KEYFRAMES  64
#define MAX_ANIMATIONS 64

typedef struct {
    float time;
    Vec3  position;
    Vec3  rotation;
    Vec3  scale;
} Keyframe;

typedef struct {
    int       obj_id;
    Keyframe  keys[MAX_KEYFRAMES];
    int       key_count;
    float     duration;
    bool      looping;
    float     elapsed;
    bool      playing;
} Animation;

typedef struct {
    Animation clips[MAX_ANIMATIONS];
    int       count;
} AnimationSystem;

void anim_init   (AnimationSystem *sys);
int  anim_create (AnimationSystem *sys, int obj_id,
                  bool looping);
void anim_add_key(AnimationSystem *sys, int anim_id,
                  Keyframe key);
void anim_play   (AnimationSystem *sys, int anim_id);
void anim_stop   (AnimationSystem *sys, int anim_id);
void anim_update (AnimationSystem *sys, Scene *s, float dt);

#endif /* OPENGTU_ANIMATION_H */
