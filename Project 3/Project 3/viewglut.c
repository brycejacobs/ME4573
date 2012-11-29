/* Copyright (c) 2009 Robert Kooima                                           */
/*                                                                            */
/* Permission is hereby granted, free of charge, to any person obtaining a    */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,   */
/* and/or sell copies of the Software, and to permit persons to whom the      */
/* Software is furnished to do so, subject to the following conditions:       */
/*                                                                            */
/* The above copyright notice and this permission notice shall be included in */
/* all copies or substantial portions of the Software.                        */
/*                                                                            */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        */
/* DEALINGS IN THE SOFTWARE.                                                  */

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "gl.h"

/*---------------------------------------------------------------------------*/

struct viewglut;

#include "viewglut.h"
#include "image.h"

/*---------------------------------------------------------------------------*/

typedef enum { st_idle,
               st_look,
               st_roll,
               st_zoom,
               st_play_fwd,
               st_play_rev } state_t;

struct viewglut
{
    state_t state;
    view   *V;

    double  d[3];
    double  n;
    double  f;

    GLuint  rect;
    GLuint  axes;
};

/*---------------------------------------------------------------------------*/

/* A rectangle outlining the view port centered at (0, 0, -1).               */

static GLfloat rect_v[4][3] = {
    { -1.0, -1.0, -1.0 },
    {  1.0, -1.0, -1.0 },
    {  1.0,  1.0, -1.0 },
    { -1.0,  1.0, -1.0 }
};

/* Three lines defining axes coordinate axes centered at (0, 0, -1);         */

static GLfloat axes_v[6][3] = {
    {  0.0,  0.0,  0.0 },
    {  0.0,  0.0, -1.0 },
    {  1.0,  0.0, -1.0 },
    {  0.0,  0.0, -1.0 },
    {  0.0,  1.0, -1.0 },
    {  0.0,  0.0, -1.0 }
};

/*---------------------------------------------------------------------------*/

viewglut *viewglut_create(double n, double f)
{
    viewglut *P;

    if ((P = (viewglut *) calloc(1, sizeof (viewglut))))
    {
        /* Initialize the view object. */

        P->V = view_create();
        P->n = n;
        P->f = f;

        /* Initialize the OpenGL state. */

        glGenBuffers(1, &P->rect);
        glBindBuffer(GL_ARRAY_BUFFER, P->rect);
        glBufferData(GL_ARRAY_BUFFER, sizeof (rect_v), rect_v, GL_STATIC_DRAW);

        glGenBuffers(1, &P->axes);
        glBindBuffer(GL_ARRAY_BUFFER, P->axes);
        glBufferData(GL_ARRAY_BUFFER, sizeof (axes_v), axes_v, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return P;
}

void viewglut_delete(viewglut *P)
{
    assert(P);

    glDeleteBuffers(1, &P->axes);
    glDeleteBuffers(1, &P->rect);

    view_delete(P->V);
}

/*---------------------------------------------------------------------------*/

/* Compute and return the eye-space pointer vector.                          */

static void pointer(const viewglut *P, int x, int y, int zoom, double v[3])
{
    const double W = (double) glutGet(GLUT_WINDOW_WIDTH);
    const double H = (double) glutGet(GLUT_WINDOW_HEIGHT);

    const double z = zoom ? 0.5 : view_matrix(P->V, NULL, NULL);

    v[0] =  P->n * z * (2.0 * x / W - 1.0);
    v[1] = -P->n * z * (2.0 * y / H - 1.0) * H / W;
    v[2] = -P->n;
}

/*---------------------------------------------------------------------------*/

/* Specify the color for a given sign and transparency.                      */

static void color_state(int s, double a)
{
    if      (s > 0) glColor4d(0.0, 1.0, 0.0, a);
    else if (s < 0) glColor4d(1.0, 0.0, 0.0, a);
    else            glColor4d(1.0, 1.0, 0.0, a);
}

static void render_key_state(GLuint rect, GLuint axes, int s)
{
    glLineWidth(3.0);

    /* Draw the view rectangle and its outline. */

    glBindBuffer(GL_ARRAY_BUFFER, rect);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    color_state(s, 0.2);
    glDrawArrays(GL_QUADS,     0, 4);
    color_state(s, 1.0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    /* Draw the view point and axes. */

    glBindBuffer(GL_ARRAY_BUFFER, axes);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glDrawArrays(GL_POINTS, 0, 1);
    glDrawArrays(GL_LINES,  0, 6);
}

static void render_sub_state(GLuint rect, GLuint axes, int s, double d)
{
    glLineWidth(1.0);
    color_state(s, 0.6 * d);

    /* Draw the view rectangle and its outline. */

    glBindBuffer(GL_ARRAY_BUFFER, rect);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    /* Draw the view point and axes. */

    glBindBuffer(GL_ARRAY_BUFFER, axes);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_POINTS, 0, 1);
}

/* Draw one state of a view path.                                            */

void viewglut_render_state(const double *M, double z, double d, int s, void *p)
{
    const double W = (double) glutGet(GLUT_WINDOW_WIDTH);
    const double H = (double) glutGet(GLUT_WINDOW_HEIGHT);

    assert(p);

    glPushMatrix();
    {
        const viewglut *P = (const viewglut *) p;

        /* Apply the location and scale to fit the zoom. */

        glMultMatrixd(M);
        glScaled(P->n * z, P->n * z * H / W, P->n);

        /* Draw the state marker. */

        if (d)
            render_sub_state(P->rect, P->axes, s, d);
        else
            render_key_state(P->rect, P->axes, s);
    }
    glPopMatrix();
}

/*---------------------------------------------------------------------------*/

/* Apply the projection and view matrices to the OpenGL matrix stack.        */

void viewglut_apply(const viewglut *P)
{
    double v[3], M[16];

    assert(P);

    /* Get the view matrix and compute the view frustum parameters. */

    view_matrix(P->V, M, NULL);

    pointer(P, 0, 0, 0, v);

    /* Set the projection and view matrices. */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(+v[0], -v[0], -v[1], +v[1], P->n, P->f);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(M);
}

/* Render the view path.                                                     */

void viewglut_render(const viewglut *P)
{
    /* Don't render the path during play mode. */

    if (P->state != st_play_fwd &&
        P->state != st_play_rev)
    {
        /* Preserve the current GL state. */

        glPushAttrib(GL_LINE_BIT         |
                     GL_POINT_BIT        |
                     GL_CURRENT_BIT      |
                     GL_LIGHTING_BIT     |
                     GL_COLOR_BUFFER_BIT |
                     GL_DEPTH_BUFFER_BIT);
        {
            /* Get set up for unlit transparent rendering. */

            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glPointSize(7.0);

            /* We'll need vertex arrays in the callback. */

            glEnableClientState(GL_VERTEX_ARRAY);
            {
                view_render(P->V, viewglut_render_state, (void *) P);
            }
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        glPopAttrib();
    }
}

/*---------------------------------------------------------------------------*/

static void viewglut_grab()
{
    const int w = glutGet(GLUT_WINDOW_WIDTH);
    const int h = glutGet(GLUT_WINDOW_HEIGHT);

    void *p;

    if ((p = malloc(w * h * 3)))
    {
        glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, p);

        image_flip (              w, h, 3, 1, p);
        image_write(VIEWGLUT_IMG, w, h, 3, 1, p);

        free(p);
    }
}

static void viewglut_fwd(viewglut *P)
{
    if (P->state != st_play_fwd)
        P->state  = st_play_fwd;
    else
        P->state  = st_idle;
}

static void viewglut_rev(viewglut *P)
{
    if (P->state != st_play_rev)
        P->state  = st_play_rev;
    else
        P->state  = st_idle;
}

/*---------------------------------------------------------------------------*/

int viewglut_idle(viewglut *P)
{
    const double dt = 1.0 / 60.0;

    assert(P);

    if      (P->state == st_play_fwd) view_play(P->V, +dt);
    else if (P->state == st_play_rev) view_play(P->V, -dt);
    else                              view_step(P->V, +dt);

    glutPostRedisplay();

    return 0;
}

int viewglut_click(viewglut *P, int x, int y, int d)
{
    assert(P);

    if (d)
    {
        const int m = glutGetModifiers();

        double v[3];

        /* Convert the mouse position to a pointer vector. */

        pointer(P, x, y, (m & GLUT_ACTIVE_CTRL), v);

        /* Apply the pointer vector. */

        view_mark(P->V, v);

        /* Select a viewing state based on modifier keys. */

        if      (m & GLUT_ACTIVE_SHIFT) P->state = st_roll;
        else if (m & GLUT_ACTIVE_CTRL)  P->state = st_zoom;
        else                            P->state = st_look;
    }
    else P->state = st_idle;

    glutPostRedisplay();
    return 1;
}

int viewglut_point(viewglut *P, int x, int y)
{
    double v[3];

    assert(P);

    /* Convert the mouse position to a pointer vector. */

    pointer(P, x, y, (P->state == st_zoom), v);

    /* Apply the new pointer vector. */

    switch (P->state)
    {
        case st_look: view_look(P->V, v); break;
        case st_roll: view_roll(P->V, v); break;
        case st_zoom: view_zoom(P->V, v); break;
        default: return 0;
    }

    glutPostRedisplay();
    return 1;
}

int viewglut_key(viewglut *P, int k, int d)
{
    double s = 5;
    int mine = 1;

    assert(P);

    if (d)
    {
        /* Interpret a key press. */

        switch (tolower(k))
        {
        case 27: exit(EXIT_SUCCESS);        break;

        case  7: viewglut_grab();           break;
        case  6: viewglut_fwd(P);           break;
        case  2: viewglut_rev(P);           break;

        case 16: view_prev(P->V);           break;
        case 14: view_next(P->V);           break;
        case  8: view_home(P->V);           break;
        case 10: view_jump(P->V);           break;

        case 19: view_save(P->V, VIEW_DAT); break;
        case 12: view_load(P->V, VIEW_DAT); break;

        case 18: view_remove(P->V);         break;
        case  9: view_insert(P->V);         break;
        case  3: view_clear (P->V);         break;

        case 'd': case 'e': P->d[0] += s;   break;
        case 'a':           P->d[0] -= s;   break;
        case 'r': case 'p': P->d[1] += s;   break;
        case 'f': case 'u': P->d[1] -= s;   break;
        case 's': case 'o': P->d[2] += s;   break;
        case 'w': case ',': P->d[2] -= s;   break;

        default: mine = 0; /* Not a viewglut key */
        }
    }
    else
    {
        /* Interpret a key release. */

        switch (tolower(k))
        {
        case 'd': case 'e': P->d[0] -= s;   break;
        case 'a':           P->d[0] += s;   break;
        case 'r': case 'p': P->d[1] -= s;   break;
        case 'f': case 'u': P->d[1] += s;   break;
        case 's': case 'o': P->d[2] -= s;   break;
        case 'w': case ',': P->d[2] += s;   break;

        default: mine = 0; /* Not a viewglut key */
        }
    }

    if (mine)
    {
        view_move(P->V, P->d);
        glutPostRedisplay();
    }
    return mine;
}

/*---------------------------------------------------------------------------*/
/* Accessors                                                                 */

void viewglut_set_near(viewglut *P, double n)
{
    assert(P);
    P->n = n;
}

void viewglut_set_far(viewglut *P, double f)
{
    assert(P);
    P->f = f;
}

view *viewglut_get_view(viewglut *P)
{
    assert(P);
    return P->V;
}

/*---------------------------------------------------------------------------*/
