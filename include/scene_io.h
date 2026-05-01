#ifndef OPENGTU_SCENE_IO_H
#define OPENGTU_SCENE_IO_H

#include "object.h"

int scene_save(const Scene *s, const char *path);
int scene_load(Scene *s,       const char *path);

#endif /* OPENGTU_SCENE_IO_H */
