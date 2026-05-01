#include <math.h>
#include "../include/physics.h"

#if __physicsEnabled

static void integrate(Object *obj, float dt) {
    if (obj->anchored) return;

    /* Gravity */
    obj->body.acceleration = (Vec3){0, -GRAVITY, 0};

    obj->body.velocity = v3_add(obj->body.velocity,
                         v3_scale(obj->body.acceleration, dt));
    obj->position      = v3_add(obj->position,
                         v3_scale(obj->body.velocity, dt));

    if (obj->position.y < 0.5f) {
        obj->position.y   = 0.5f;
        obj->body.velocity.y = -obj->body.velocity.y * 0.4f;  /* bounce */
        obj->body.on_ground  = true;
    } else {
        obj->body.on_ground = false;
    }

    object_update_matrix(obj);
}

void physics_update(Scene *s, float dt) {
    float sub_dt = dt / (float)PHYSICS_SUBSTEPS;
    for (int step=0; step<PHYSICS_SUBSTEPS; step++) {
        for (int i=0; i<s->obj_count; i++)
            if (s->objects[i].active)
                integrate(&s->objects[i], sub_dt);
        physics_resolve_collisions(s);
    }
}

bool physics_aabb_collide(const Object *a, const Object *b,
                           Vec3 *normal, float *penetration)
{
    Vec3 diff = v3_sub(b->position, a->position);
    Vec3 sum  = (Vec3){a->scale.x+b->scale.x,
                       a->scale.y+b->scale.y,
                       a->scale.z+b->scale.z};
    if (fabsf(diff.x)>sum.x || fabsf(diff.y)>sum.y || fabsf(diff.z)>sum.z)
        return false;

    float px=sum.x-fabsf(diff.x), py=sum.y-fabsf(diff.y), pz=sum.z-fabsf(diff.z);
    if (px<py && px<pz){ *normal=(Vec3){diff.x>0?1:-1,0,0}; *penetration=px; }
    else if(py<pz)      { *normal=(Vec3){0,diff.y>0?1:-1,0}; *penetration=py; }
    else                { *normal=(Vec3){0,0,diff.z>0?1:-1}; *penetration=pz; }
    return true;
}

void physics_resolve_collisions(Scene *s) {
    for (int i=0; i<s->obj_count; i++)
    for (int j=i+1; j<s->obj_count; j++) {
        Object *a=&s->objects[i], *b=&s->objects[j];
        if (!a->active||!b->active||!a->collision||!b->collision) continue;
        Vec3 n; float pen;
        if (!physics_aabb_collide(a,b,&n,&pen)) continue;

        float total_mass=a->body.mass+b->body.mass;
        if (!a->anchored) a->position=v3_sub(a->position, v3_scale(n, pen*(b->body.mass/total_mass)));
        if (!b->anchored) b->position=v3_add(b->position, v3_scale(n, pen*(a->body.mass/total_mass)));

        Vec3 rv=v3_sub(b->body.velocity, a->body.velocity);
        float vel_along=v3_dot(rv,n);
        if (vel_along>0) continue;
        float e=0.4f;
        float j_val=-(1+e)*vel_along/total_mass;
        Vec3 impulse=v3_scale(n,j_val);
        if (!a->anchored) a->body.velocity=v3_sub(a->body.velocity, v3_scale(impulse,1.0f/a->body.mass));
        if (!b->anchored) b->body.velocity=v3_add(b->body.velocity, v3_scale(impulse,1.0f/b->body.mass));

        object_update_matrix(a);
        object_update_matrix(b);
    }
}

void physics_apply_impulse(Object *obj, Vec3 impulse) {
    if (obj->anchored) return;
    obj->body.velocity = v3_add(obj->body.velocity,
                         v3_scale(impulse, 1.0f/obj->body.mass));
}

#else  /* physics disabled stubs */
void physics_update           (Scene *s, float dt)                           { (void)s;(void)dt; }
bool physics_aabb_collide     (const Object *a, const Object *b, Vec3 *n, float *p) { (void)a;(void)b;(void)n;(void)p; return false; }
void physics_resolve_collisions(Scene *s)                                     { (void)s; }
void physics_apply_impulse    (Object *obj, Vec3 impulse)                     { (void)obj;(void)impulse; }
#endif
