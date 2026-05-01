# OpenGTU — Open Graphics Terminal Unit

A pseudo-3D ASCII rendering engine for the terminal, written in C.

## Quick start

```bash
make run
```

## Controls

| Key | Action |
|-----|--------|
| W / S | Move forward / backward (free-fly) or zoom (orbit) |
| A / D | Strafe left / right |
| Q / E | Move up / down |
| Arrow keys | Look around (free-fly) or orbit (fixed) |
| F1 | Toggle wireframe mode |
| F2 | Toggle debug overlay |
| F3 | Cycle shading profile |
| ESC | Quit |

## Configuration (`include/config.h`)

*(everything here is set by defualt)*
| Macro | Default | Description |
|-------|---------|-------------|
| `FPS` | 60 | Target render framerate |
| `__GTR` | 100 | Simulation tick rate |
| `__physicsEnabled` | 1 | Toggle physics |
| `camera` | 1 | 0=fixed orbit, 1=free-fly |
| `SCREEN_WIDTH/HEIGHT` | 120×40 | Viewport size |
| `WIREFRAME_MODE` | 0 | Global wireframe |
| `DEBUG_OVERLAY` | 1 | FPS/info bar |

## Shading profiles

Cycle with **F3** at runtime, or call directly in code:

```c
obj->shading = __retu_gst();         // default
obj->shading = __retu_gst_dense();
obj->shading = __retu_gst_minimal();
obj->shading = __retu_gst_classic();
obj->shading = __retu_gst_smooth();
```

## Custom voxel shapes

```c
static const int my_shape[] = {
    1,1,1,
    1,0,1,
    1,1,1,
};
Object *obj = scene_add_object(&scene, SHAPE_CUSTOM, pos, rot, scl);
obj->vox_w = 3; obj->vox_h = 3; obj->vox_d = 1;
memcpy(obj->voxels, my_shape, sizeof(my_shape));
object_build_mesh(obj);
```

## Scene save / load

```bash
# The running engine saves to a .gtus file; load it:
./opengtu my_scene.gtus
```

## File layout

```
OpenGTU/
├── include/
│   ├── config.h       Engine configuration macros
│   ├── math3d.h       Vector & matrix math
│   ├── graphics.h     Shading profiles & framebuffer
│   ├── object.h       Object, mesh & scene system
│   ├── camera.h       Camera (fixed / free-fly)
│   ├── physics.h      Physics (gravity, collision)
│   ├── input.h        Non-blocking keyboard input
│   ├── renderer.h     Rasteriser & renderer
│   ├── animation.h    Keyframe animation system
│   └── scene_io.h     Scene save / load (.gtus)
└── src/
    ├── main.c         Engine loop & demo scene
    ├── math3d.c
    ├── graphics.c
    ├── object.c
    ├── camera.c
    ├── physics.c
    ├── input.c
    ├── renderer.c
    ├── animation.c
    └── scene_io.c
```
