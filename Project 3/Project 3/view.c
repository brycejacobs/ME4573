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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "view.h"
#include "math3d.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

/*----------------------------------------------------------------------------*/

struct state
{
    double p[3];                /* Position                         */
    double e[3];                /* Orientation                      */
    double z;                   /* Zoom coefficient                 */
};

typedef struct state state;

struct view
{
    state  curr;                /* Current view state               */
    state  mark;                /* Marked view state                */
    state *list;                /* State list buffer                */

    int    mode;                /* Interaction mode [global, local] */
    int    n;                   /* Number of states in the list     */
    double t;                   /* Current playback time in [0,n)   */
    double v[3];                /* Eye-space velocity vector        */
    double m[3];                /* Eye-space mark vector            */
};

/*----------------------------------------------------------------------------*/

/* Return n modulo m in [0,m).                                                */

static int mmod(int n, int m)
{
    const int d = n % m;
    return (d < 0) ? d + m : d;
}

/* Return f modulo m in [0,m).                                                */

static double fmmod(double f, double m)
{
    const double d = fmod(f, m);
    return (d < 0.0) ? d + m : d;
}

/*----------------------------------------------------------------------------*/
/* Interpolate.                                                               */

static double mix(double a, double b, double c, double d, double t)
{
    const double w = d - c - a + b;
    const double x = a - b - w;
    const double y = c - a;
    const double z = b;

    return (w * t * t * t) + (x * t * t) + (y * t) + (z);
}

/*----------------------------------------------------------------------------*/
/* Array management procedures.                                               */

static void *array_insert(void *p, int i, int n, size_t s)
{
    if ((p = realloc(p, (n + 1) * s)))
    {
        unsigned char *p0 = (unsigned char *) p + (i    ) * s;
        unsigned char *p1 = (unsigned char *) p + (i + 1) * s;

        memmove(p1, p0, (n - i) * s);

        return p;
    }
    return NULL;
}

static void *array_remove(void *p, int i, int n, size_t s)
{
    unsigned char *p0 = (unsigned char *) p + (i    ) * s;
    unsigned char *p1 = (unsigned char *) p + (i + 1) * s;

    memmove(p0, p1, (n - i - 1) * s);

    return realloc(p, (n - 1) * s);
}

/*----------------------------------------------------------------------------*/
/* State and state list management procedures.                                */

/* Insert state curr into the state list at index i.  Append when i = n.      */

static void state_insert(view *V, int i, state *s)
{
    state *p;

    if ((p = (state *) array_insert(V->list, i, V->n, sizeof (state))))
    {
        V->list = p;
        V->n   += 1;

        memcpy(V->list + i, s, sizeof (state));
    }
}

/* Remove the state from the list at index i.                                 */

static void state_remove(view *V, int i)
{
    state *p;

    if ((p = (state *) array_remove(V->list, i, V->n, sizeof (state))))
    {
        V->list = p;
        V->n   -= 1;
    }
}

/* Interpolate between two states with k in [0,1].                            */

static void state_mix(state *s,
                const state *a,
                const state *b,
                const state *c,
                const state *d, double k)
{
    double q[4], qa[4], qb[4], qc[4], qd[4];

    s->p[0] = mix(a->p[0], b->p[0], c->p[0], d->p[0], k);
    s->p[1] = mix(a->p[1], b->p[1], c->p[1], d->p[1], k);
    s->p[2] = mix(a->p[2], b->p[2], c->p[2], d->p[2], k);

    qeuler(qa, a->e);
    qeuler(qb, b->e);
    qeuler(qc, c->e);
    qeuler(qd, d->e);

    qsquad(q, qa, qb, qc, qd, k);

    equaternion(s->e, q);

    s->z = mix(a->z, b->z, c->z, d->z, k);
}

/* Compute a 4x4 transform matrix for the given state.                        */

static void state_matrix(double *M, const state *s)
{
    double T[16], R[16], p[3];

    vneg(p, s->p);

    mtranslate(T, p);
    meuler    (R, s->e);
    mmultiply (M, R, T);
}

/* Compute a 4x4 inverse matrix for the given state.                          */

static void state_inverse(double *I, const state *s)
{
    double T[16], A[16], R[16];

    mtranslate(T, s->p);
    meuler    (A, s->e);
    mtranspose(R, A);
    mmultiply (I, T, R);
}

/* Write state s to the given file pointer.                                  */

static void state_write(FILE *fp, const state *s)
{
    fprintf(fp, "%+.2e %+.2e %+.2e %+.2e %+.2e %+.2e %+.2e\n",
            s->p[0], s->p[1], s->p[2],
            s->e[0], s->e[1], s->e[2], s->z);
}

/* Read state s from the given file pointer, returning success.              */

static int state_read(FILE *fp, state *s)
{
    return (fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf",
                   &s->p[0], &s->p[1], &s->p[2],
                   &s->e[0], &s->e[1], &s->e[2], &s->z) == 7);
}

/*----------------------------------------------------------------------------*/
/* Create and destroy heap-allocated view structures.                        */

view *view_create()
{
    view *V;

    if ((V = (view *) calloc(1, sizeof (view))))
        view_home(V);

    return V;
}

void view_delete(view *V)
{
    assert(V);

    if (V->list) free(V->list);

    free(V);
}

/*----------------------------------------------------------------------------*/
/* Compute the state s of view V at time t.                                  */

void view_time(const view *V, double t, state *s)
{
    int i = (int) floor(t);
    int j = (int)  ceil(t);

    if (i < j)
        state_mix(s, V->list + mmod(i - 1, V->n),
                     V->list + mmod(i + 0, V->n),
                     V->list + mmod(i + 1, V->n),
                     V->list + mmod(i + 2, V->n), t - i);
    else
           memcpy(s, V->list + mmod(i, V->n), sizeof (state));
}

/* Return the "current" modifiable state.  This is the curr state in global  */
/* mode, but most recent enlisted state if in local mode.  This allows the   */
/* state list to be tweaked remotely by the same mechanisms as the view.     */

state *view_curr(view *V)
{
    if (V->mode && V->n)
        return  V->list + (int) floor(V->t);
    else
        return &V->curr;
}

/*----------------------------------------------------------------------------*/

/* Reset the view state to the origin.                                       */

void view_home(view *V)
{
    state *s = view_curr(V);

    s->p[0] = 0.0;
    s->p[1] = 0.0;
    s->p[2] = 0.0;

    s->e[0] = 0.0;
    s->e[1] = 0.0;
    s->e[2] = 0.0;

    s->z    = 0.5;

    V->v[0] = 0.0;
    V->v[1] = 0.0;
    V->v[2] = 0.0;
}

/* Set the view interaction mode.                                            */

void view_mode(view *V, int m)
{
    assert(V);
    V->mode = m;
}

/* Note the velocity vector.                                                 */

void view_move(view *V, const double *v)
{
    assert(V);
    vcpy(V->v, v);
}

/* Note the pointer vector and view state to mark the beginning of a look.   */

void view_mark(view *V, const double *m)
{
    assert(V);
    vnormalize(V->m, m);

    memcpy(&V->mark, view_curr(V), sizeof (state));
}

/*----------------------------------------------------------------------------*/
/* Pan the view up/down and left/right.                                      */

void view_turn(view *V, const double *w)
{
#if 0
    double M[16], T[16], q[4], a[3], b[3], c[3], t[3];

    assert(V);

    /* Transform the mark and drag vectors from eye space into the world     */
    /* space current at the time of the mark.                                */

    state_matrix(M, &V->mark);

    mtranspose(T, M);
    vnormalize(t, V->a);
    vtransform(a, T, t);
    vnormalize(t,    w);
    vtransform(b, T, t);

    /* These determine the axis and angle of rotation.  Find the quaternion. */

    vcrs(t, a, b);
    vnormalize(c, t);
    qrotate(q, c, acos(vdot(a, b)));

    /* Accumulate this quaternion with the view state, rotating the view.    */

    qmultiply(view_curr(V)->q, V->mark.q, q);
#endif
}

/*----------------------------------------------------------------------------*/

/* Look up/down and left/right in the style of a first-person game.          */

void view_look(view *V, const double *v)
{
    double /* M[16],*/ e[2];

    double d0;
    double d1;

    assert(V);

    d0 = +asin(v[1] - V->m[1]);
    d1 = -asin(v[0] - V->m[0]);

    e[0] = V->mark.e[0] + d0;
    e[1] = V->mark.e[1] + d1;

    /* Clamp or wrap the angles. */

    if      (e[0] >  3.14 * 0.5) view_curr(V)->e[0] =  3.14 * 0.5;
    else if (e[0] < -3.14 * 0.5) view_curr(V)->e[0] = -3.14 * 0.5;
    else                         view_curr(V)->e[0] = e[0];

    if      (e[1] >  3.14)       view_curr(V)->e[1] = e[1] - 3.14 * 2.0;
    else if (e[1] < -3.14)       view_curr(V)->e[1] = e[1] + 3.14 * 2.0;
    else                         view_curr(V)->e[1] = e[1];
}

/*----------------------------------------------------------------------------*/
/* Roll the view about the view direction.                                   */

void view_roll(view *V, const double *w)
{
    /* The mark and drag vectors determine the angle of rotation. */

    const double a = atan2(V->v[1], V->v[0]);
    const double b = atan2(   w[1],    w[0]);

    view_curr(V)->e[2] = V->mark.e[2] + b - a;
}

/*----------------------------------------------------------------------------*/
/* Zoom the view.                                                            */

void view_zoom(view *V, const double *v)
{
    double k0, k1, z;

    assert(V);

    /* Dragging vertically determines the zoom scale. */

    k0 = (V->m[1] + 1.0) / 2.0;
    k1 = (   v[1] + 1.0) / 2.0;

    /* Compute and clamp the new zoom value. */

    z = V->mark.z * pow(k1 / k0, 4.0);

    view_curr(V)->z = MIN(MAX(z, 1.0 / 32.0), 4.0);
}

/*----------------------------------------------------------------------------*/
/* Apply the passage of time in either interaction or playback mode.         */

void view_step(view *V, double dt)
{
    double I[16], v[3];

    assert(V);

    /* Compute the world-space velocity vector. */

    state_inverse(I, view_curr(V));
    vtransform(v, I, V->v);

    /* Integrate the new position along this vector. */

    vmad(view_curr(V)->p, view_curr(V)->p, v, dt);
}

void view_play(view *V, double dt)
{
    assert(V);

    if (V->n)
    {
        V->t += dt;

        if (V->t >= V->n) V->t -= V->n;
        if (V->t <   0.0) V->t += V->n;

        view_time(V, V->t, &V->curr);
    }
}

/*----------------------------------------------------------------------------*/
/* Save the view state list to the named file.                               */

void view_save(view *V, const char *name)
{
    FILE *fp;

    if ((fp = fopen(name, "w")))
    {
        int i;

        for (i = 0; i < V->n; ++i)
            state_write(fp, V->list + i);

        fclose(fp);
    }
}

/* Load the view state list from the named file.                             */

void view_load(view *V, const char *name)
{
    FILE *fp;

    if ((fp = fopen(name, "r")))
    {
        state s;

        while (state_read(fp, &s))
            state_insert(V, V->n, &s);

        fclose(fp);
    }
}

/*----------------------------------------------------------------------------*/
/* Move forward or backward in the state sequence.                           */

void view_next(view *V)
{
    assert(V);

    if (V->t < ceil(V->t))
        V->t = ceil(V->t);
    else
        V->t = V->t + 1.0;

    V->t = fmmod(V->t, V->n);
}

void view_prev(view *V)
{
    assert(V);

    if (V->t > floor(V->t))
        V->t = floor(V->t);
    else
        V->t = V->t - 1.0;

    V->t = fmmod(V->t, V->n);
}

void view_jump(view *V)
{
    assert(V);

    if (V->n > 0)
        view_time(V, V->t, &V->curr);
}

/*----------------------------------------------------------------------------*/
/* Insert or delete a state from the sequence.                               */

void view_insert(view *V)
{
    int i;

    assert(V);

    i = (int) (floor(V->t) + 1)
            % (      V->n  + 1);

    state_insert(V, i, &V->curr);

    V->t = i;
}

void view_remove(view *V)
{
    int i;

    assert(V);

    i = (int) floor(V->t);

    if (V->n)
        state_remove(V, i);

    if (V->n)
        V->t = fmmod(i - 1, V->n);
}

void view_clear(view *V)
{
    assert(V);

    if (V->list) free(V->list);

    V->list = NULL;
    V->n    = 0;
    V->t    = 0.0;
}

/*----------------------------------------------------------------------------*/
/* Compute a view matrix and/or inverse for the current view state.          */

double view_matrix(const view *V, double *M, double *I)
{
    assert(V);

    if (M) state_matrix (M, &V->curr);
    if (I) state_inverse(I, &V->curr);

    return V->curr.z;
}

/*----------------------------------------------------------------------------*/
/* Accessors                                                                 */

const double *view_get_p(const view *V)
{
    assert(V);
    return V->curr.p;
}

const double *view_get_e(const view *V)
{
    assert(V);
    return V->curr.e;
}

double view_get_t(const view *V)
{
    assert(V);
    return V->t;
}

int view_get_n(const view *V)
{
    assert(V);
    return V->n;
}

/*----------------------------------------------------------------------------*/
/* Rendering                                                                 */

static void render_state(const view *V,
                               view_render_fn fn, double d, double t, void *p)
{
    state c;
    double M[16];

    view_time(V, t, &c);
    state_inverse(M, &c);

    if      (t > V->t) fn(M, c.z, d, +1, p);
    else if (t < V->t) fn(M, c.z, d, -1, p);
    else               fn(M, c.z, d,  0, p);
}

void view_render(const view *V, view_render_fn fn, void *p)
{
    int i, j, n = 10;

    assert(V);

    /* Draw the key states. */

    for (i = 0; i < V->n; ++i)
        render_state(V, fn, 0.0, (double) i, p);

    /* Draw the interpolated states. */

    if (V->n > 1)
        for (i = 0; i < V->n; ++i)
            for (j = 1; j < n; ++j)
                render_state(V, fn, (double) j / n,
                                    (double) j / n + (double) i, p);

    /* Draw the playback time state. */

    if (V->n > 0)
        render_state(V, fn, 0.0, V->t, p);
}

/*----------------------------------------------------------------------------*/
