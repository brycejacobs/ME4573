
#include "Plane.h"
#include <assert.h>
#include <stdlib.h>



vert *Plane::add_vert(vert *v, GLfloat x, GLfloat z)
{
    v->v[0] = x;
    v->v[1] = 0;
    v->v[2] = z;

    v->n[0] = 0;
    v->n[1] = 1;
    v->n[2] = 0;

    return v + 1;
}

GLushort *Plane::add_rect(GLushort *e, GLushort i0, GLushort i1,
                                       GLushort i2, GLushort i3)
{
    e[0] = i0;
    e[1] = i2;
    e[2] = i1;
    e[3] = i3;
    e[4] = i1;
    e[5] = i2;

    return e + 6;
}

void Plane::init_verts(GLuint vbo[1], GLushort n)
{
    GLsizei nv = 4 * (n + 1) * (n + 1), sv = nv * sizeof (vert);
    vert *v;
    vert *w;

    /* Initialize the vertex array. */

    if ((w = v = (vert *) malloc(sv)))
    {
        const GLfloat d = 0.50f * n;
        const GLfloat g = 0.05f;

        GLushort i, j;

        for     (i = 0; i <= n; ++i)
            for (j = 0; j <= n; ++j)
            {
                w = add_vert(w, j - d - g, i - d - g);
                w = add_vert(w, j - d + g, i - d - g);
                w = add_vert(w, j - d - g, i - d + g);
                w = add_vert(w, j - d + g, i - d + g);
            }
    }

    /* Copy the vertex data to the buffer object. */

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sv, v, GL_STATIC_DRAW);

    free(v);
}

void Plane::init_elems(GLuint ebo[2], GLushort n)
{
    GLsizei nf = 6 * (n    ) * (    n    ), sf = nf * sizeof (GLushort);
    GLsizei nl = 6 * (n + 1) * (3 * n + 1), sl = nl * sizeof (GLushort);

    GLushort *f = 0;
    GLushort *p = 0;
    GLushort *l = 0;
    GLushort *q = 0;
    GLushort  i,  j;

    /* Initialize the element arrays. */

    if ((f = p = (GLushort *) malloc(sf)) &&
        (l = q = (GLushort *) malloc(sl)))

        for     (i = 0; i <= n; ++i)
            for (j = 0; j <= n; ++j)
            {
                const GLushort a = 4 * ((n + 1) * (i    ) + j);
                const GLushort b = 4 * ((n + 1) * (i + 1) + j);

                                    q = add_rect(q, a + 0, a + 1, a + 2, a + 3);
                if (j < n)          q = add_rect(q, a + 1, a + 4, a + 3, a + 6);
                if (i < n)          q = add_rect(q, a + 2, a + 3, b + 0, b + 1);
                if (i < n && j < n) p = add_rect(p, a + 3, a + 6, b + 1, b + 4);
            }

    /* Copy the element data to the buffer object. */

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sf, f, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sl, l, GL_STATIC_DRAW);

    free(f);
    free(l);
}

/*----------------------------------------------------------------------------*/

/* Allocate and initialize a new plane object. There must be a current OpenGL */
/* context at the time.                                                       */

planeobject *Plane::plane_create(int n)
{
    planeobject *P;

    if ((P = (planeobject *) malloc(sizeof (planeobject))))
    {
        glGenBuffers(1, P->vbo);
        glGenBuffers(2, P->ebo);

        init_verts(P->vbo, n);
		init_elems(P->ebo, n);
		
        P->n = n;
    }
    return P;
}

/* Release the given plane structure and all resources held by it.            */

void Plane::plane_delete(planeobject *P)
{
    assert(P);

    glDeleteBuffers(2, P->ebo);
    glDeleteBuffers(1, P->vbo);

    free(P);
}

/*----------------------------------------------------------------------------*/

/* Render the given plane structure.                                          */

void Plane::plane_render(planeobject *P)
{
    const GLsizei nf = 6 * (P->n    ) * (    P->n    );
    const GLsizei nl = 6 * (P->n + 1) * (3 * P->n + 1);

    const size_t sz = sizeof (GLfloat);

    glEnable(GL_COLOR_MATERIAL);
    {
        /* Enable the necessary array pointers. */

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        {
            /* Bind the array pointers to the array buffer object. */

            glBindBuffer(GL_ARRAY_BUFFER, P->vbo[0]);
            {
                glVertexPointer(3, GL_FLOAT, sz * 6, (GLvoid *) (     0));
                glNormalPointer(   GL_FLOAT, sz * 6, (GLvoid *) (sz * 3));

                /* Render the squares. */

                glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, P->ebo[0]);
                glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_SHORT, 0);

                /* Render the lines */

                glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, P->ebo[1]);
                glDrawElements(GL_TRIANGLES, nl, GL_UNSIGNED_SHORT, 0);

                /* Revert all state. */

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    glDisable(GL_COLOR_MATERIAL);
}