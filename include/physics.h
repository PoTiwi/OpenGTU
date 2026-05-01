#ifndef OPENGTU_PHYSICS_H
#define OPENGTU_PHYSICS_H

#include "object.h"
#include "config.h"

void physics_update(Scene *s, float dt);

bool physics_aabb_collide(const Object *a, const Object *b,
                           Vec3 *normal, float *penetration);

void physics_resolve_collisions(Scene *s);

void physics_apply_impulse(Object *obj, Vec3 impulse);

#endif /* OPENGTU_PHYSICS_H */
