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

#ifndef UTIL3D_VIEW_H
#define UTIL3D_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/

typedef struct view view;

view *view_create();
void  view_delete(view *);

/*----------------------------------------------------------------------------*/

void view_home(view *);
void view_mode(view *, int);
void view_move(view *, const double *);
void view_mark(view *, const double *);
void view_turn(view *, const double *);
void view_look(view *, const double *);
void view_roll(view *, const double *);
void view_zoom(view *, const double *);

/*----------------------------------------------------------------------------*/

void view_step(view *, double);
void view_play(view *, double);

/*----------------------------------------------------------------------------*/

void view_next(view *);
void view_prev(view *);
void view_jump(view *);

void view_insert(view *);
void view_remove(view *);
void view_clear (view *);

/*----------------------------------------------------------------------------*/

#define VIEW_DAT "view.dat"

void view_save(view *, const char *);
void view_load(view *, const char *);

/*----------------------------------------------------------------------------*/

double view_matrix(const view *, double *, double *);

const double *view_get_p(const view *);
const double *view_get_e(const view *);
double        view_get_t(const view *);
int           view_get_n(const view *);

/*----------------------------------------------------------------------------*/

typedef void (*view_render_fn)(const double *, double, double, int, void *);

void view_render(const view *, view_render_fn, void *);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif
