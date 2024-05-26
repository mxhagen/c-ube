#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define f32 float
#define i32 int
#define u32 unsigned

#define ROWS 30
#define COLS 60


char buf[COLS * ROWS];         // screen buffer
f32 zbuf[COLS * ROWS];         // depth-buffer

const char bg = ' ';           // background char
const u32 dist = 100;          // distance from the camera
const f32 k_proj = 40;         // projection constant

const u32 timeout = 1e4;       // timeout between frames

const f32 d_angle_a = 0.02,    // rotation speeds
          d_angle_b = 0.02,
          d_angle_c = 0.007;

const f32 x_stretch = 1.70;    // stretch along axes
const f32 y_stretch = 0.75;



f32 x, y, z;                   // 3d coordinates
f32 a = 0.0, b = 0.0, c = 0.0; // rotation angles for the cube
f32 z_recip;                   // reciprocal of z (1/z)
i32 x_proj, y_proj;            // screen coordinates
i32 i;                         // buffer index


void render_surface(f32 cx, f32 cy, f32 cz, char ch) {
    x = cy * sin(a) * sin(b) * cos(c)
      - cz * cos(a) * sin(b) * cos(c)
      + cy * cos(a) * sin(c)
      + cz * sin(a) * sin(c)
      + cx * cos(b) * cos(c);

    y = cy * cos(a) * cos(c)
      + cz * sin(a) * cos(c)
      - cy * sin(a) * sin(b) * sin(c)
      + cz * cos(a) * sin(b) * sin(c)
      - cx * cos(b) * sin(c);

    z = dist
      + cz * cos(a) * cos(b)
      - cy * sin(a) * cos(b)
      + cx * sin(b);

    if (z == 0) z = 1e-7; // avoid div by 0
    z_recip = 1 / z;

    x_proj = (i32)((f32) COLS / 2 + k_proj * z_recip * x * x_stretch);
    y_proj = (i32)((f32) ROWS / 2 + k_proj * z_recip * y * y_stretch);

    i = x_proj + y_proj * COLS;
    if (0 <= i && i < COLS * ROWS) {
        if (z_recip > zbuf[i]) {
            zbuf[i] = z_recip;
            buf[i] = ch;
        }
    }
}

i32 main() {
    printf("\033[2J\033[?25l"); // clear and make cursor invisible
    for (;;) {
        memset( buf, bg, COLS * ROWS * sizeof(char));
        memset(zbuf,  0, COLS * ROWS * sizeof( f32));

        a += d_angle_a, b += d_angle_b, c += d_angle_c;

        // render to buffer
        for (f32 cx = -20; cx < 20; cx += 0.5) {
            for (f32 cy = -20; cy < 20; cy += 0.5) {
                render_surface( cx,  cy, -20, '@');
                render_surface( 20,  cy,  cx, '$');
                render_surface(-20,  cy, -cx, '#');
                render_surface( -cx, cy,  20, '=');
                render_surface( cx, -20, -cy, '+');
                render_surface( cx,  20,  cy, '~');
            }
        }
        printf("\033[H"); // reset cursor position

        // draw buffer contents
        for (i32 k = 0; k < COLS * ROWS; k++) {
            putchar(buf[k]);
            if (k % COLS == COLS - 1) putchar('\n');
        }

        usleep(timeout);
    }
    return 0;
}
