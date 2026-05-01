#ifndef OPENGTU_INPUT_H
#define OPENGTU_INPUT_H

#include <stdbool.h>

#define KEY_NONE      0
#define KEY_W         'w'
#define KEY_A         'a'
#define KEY_S         's'
#define KEY_D         'd'
#define KEY_Q         'q'
#define KEY_E         'e'
#define KEY_ESC       27
#define KEY_UP        128
#define KEY_DOWN      129
#define KEY_LEFT      130
#define KEY_RIGHT     131
#define KEY_SPACE     ' '
#define KEY_F1        200   /* Toggle wireframe               */
#define KEY_F2        201   /* Toggle debug overlay           */
#define KEY_F3        202   /* Cycle shading profile          */

void input_init(void);

void input_cleanup(void);

int  input_poll(void);

bool input_held(int key);

#endif /* OPENGTU_INPUT_H */
