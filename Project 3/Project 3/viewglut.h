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

#ifndef UTIL3D_VIEWGLUT_H
#define UTIL3D_VIEWGLUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "view.h"

/*---------------------------------------------------------------------------*/

#define VIEWGLUT_IMG "grab.png"

/*---------------------------------------------------------------------------*/

typedef struct viewglut viewglut;

viewglut *viewglut_create(double, double);
void      viewglut_delete(viewglut *);

/*---------------------------------------------------------------------------*/

void  viewglut_apply (const viewglut *);
void  viewglut_render(const viewglut *);

/*---------------------------------------------------------------------------*/

int   viewglut_idle (viewglut *);
int   viewglut_point(viewglut *, int, int);
int   viewglut_click(viewglut *, int, int, int);
int   viewglut_key  (viewglut *, int, int);

/*---------------------------------------------------------------------------*/

void  viewglut_set_near(viewglut *, double);
void  viewglut_set_far (viewglut *, double);
view *viewglut_get_view(viewglut *);

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif
