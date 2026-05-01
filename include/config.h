#ifndef OPENGTU_CONFIG_H
#define OPENGTU_CONFIG_H

#define FPS           60        /* Target render framerate      */
#define __GTR         100       /* Global tick / simulation rate*/

#define __physicsEnabled  1     /* 1 = enabled, 0 = disabled    */
#define GRAVITY           9.81f /* Gravitational acceleration    */
#define PHYSICS_SUBSTEPS  4     /* Integration sub-steps/tick   */

/* 0 = fixed / orbit (centred on scene origin)                  */
/* 1 = free-fly  (WASD to move, arrow keys to look)            */
#define camera  0

#define SCREEN_WIDTH   120      /* Terminal columns             */
#define SCREEN_HEIGHT   40      /* Terminal rows                */
#define FOV            60.0f    /* Field of view (degrees)      */
#define RENDER_DIST    30.0f    /* Far-clip distance            */

#define DOUBLE_BUFFER   1       /* Reduce flicker               */
#define WIREFRAME_MODE  0       /* 1 = edges only               */
#define FILL_MODE       1       /* 1 = filled faces             */
#define DEBUG_OVERLAY   1       /* Show FPS / object count      */

#define MAX_OBJECTS   256
#define MAX_LIGHTS      8
#define MAX_VERTS    4096
#define MAX_FACES    2048

#define INPUT_MOVE_SPEED  0.15f
#define INPUT_LOOK_SPEED  0.04f

#endif /* OPENGTU_CONFIG_H */
