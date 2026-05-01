#include <stdio.h>
#include "../include/scene_io.h"
#include <stdlib.h>

#define GTUS_MAGIC   0x47545553U
#define GTUS_VERSION 1

typedef struct {
    unsigned int magic;
    unsigned int version;
    int          obj_count;
    int          light_count;
} GtusHeader;

int scene_save(const Scene *s, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;

    GtusHeader hdr = {GTUS_MAGIC, GTUS_VERSION, s->obj_count, s->light_count};
    fwrite(&hdr,       sizeof(hdr),       1, f);
    fwrite(s->objects, sizeof(Object),    s->obj_count,   f);
    fwrite(s->lights,  sizeof(Light),     s->light_count, f);
    fwrite(&s->ambient,sizeof(Vec3),      1, f);
    fwrite(&s->gravity,sizeof(Vec3),      1, f);

    fclose(f);
    return 0;
}

int scene_load(Scene *s, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    GtusHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1 ||
        hdr.magic != GTUS_MAGIC || hdr.version != GTUS_VERSION) {
        fclose(f); return -1;
    }

    s->obj_count   = hdr.obj_count;
    s->light_count = hdr.light_count;
    size_t count = s->obj_count;
    if (fread(s->objects, sizeof(Object), count, f) != count)
    if (fread(s->lights, sizeof(Light), s->light_count, f) != (size_t)s->light_count) {
        perror("Failed to read lights");
        exit(EXIT_FAILURE);
    }

    if (fread(&s->ambient, sizeof(Vec3), 1, f) != 1) {
        perror("Failed to read ambient");
        exit(EXIT_FAILURE);
    }

    if (fread(&s->gravity, sizeof(Vec3), 1, f) != 1) {
        perror("Failed to read gravity");
        exit(EXIT_FAILURE);
    }

    if (fread(s->objects, sizeof(Object), s->obj_count, f) != (size_t)s->obj_count)
    
    fclose(f);
    return 0;
}
