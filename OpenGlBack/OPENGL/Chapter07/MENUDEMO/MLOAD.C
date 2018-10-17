/*=========================================================================
  Model Loading

  Romka Graphics 
  Author: Roman Podobedov
  Email: romka@ut.ee
  WEB: www.ut.ee/~romka
=========================================================================*/

/* Model file load */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mload.h"

void ParseInt(char *s, int *a, int *b, int *c)
{
  int i, j;
  int x, y, z;
  
  x = 0; y = -1; z = -1;
  j = (int)strlen(s);
  for (i=0; i<j; i++)
  {
    if ((s[i] == ' ') || (s[i] == '\n'))
    {
      s[i] = 0;
      if (y == -1) y = i+1;
      else
      if (z == -1) z = i+1;
    }
  }
  *a = atoi(&s[x]);
  *b = atoi(&s[y]);
  *c = atoi(&s[z]);
}

void ParseFloat(char *s, float *a, float *b, float *c)
{
  int i, j;
  int x, y, z;
  
  x = 0; y = -1; z = -1;
  j = (int)strlen(s);
  for (i=0; i<j; i++)
  {
    if ((s[i] == ' ') || (s[i] == '\n'))
    {
      s[i] = 0;
      if (y == -1) y = i+1;
      else
      if (z == -1) z = i+1;
    }
  }
  *a = (float)atof(&s[x]);
  *b = (float)atof(&s[y]);
  *c = (float)atof(&s[z]);
}

int isEmpty(char *s)
{
  int i;

  for (i=0; i<(int)strlen(s); i++)
    if (s[i] > 32) return 0;
 
  return 1;
}

void LoadMFile(char *filename, struct model *m)
{
  FILE *f;
  char buf[128];
  int i, j, a;
  float x, y, z;
  int kk, ll, mm;

  f = fopen(filename, "r");
  
  /* Get info */
  fgets(buf, 128, f);
  ParseInt(buf, &m->vn, &m->nn, &m->pn);
  /* Get vertexs */
  m->v = (float *)malloc(sizeof(float)*3*m->vn);
  for (i=0; i<m->vn; i++)
  {
    do
    {
      fgets(buf, 128, f);
    } while (isEmpty(buf));
    
    ParseFloat(buf, &x, &y, &z);
    m->v[i*3] = x;;
    m->v[i*3+1] = y;;
    m->v[i*3+2] = z;;
  }

  /* Get normals */
  m->n = (float *)malloc(sizeof(float)*3*m->nn);
  for (i=0; i<m->nn; i++)
  {
    do
    {
      fgets(buf, 128, f);
    } while (isEmpty(buf));
    
    ParseFloat(buf, &x, &y, &z);
    m->n[i*3] = x;;
    m->n[i*3+1] = y;;
    m->n[i*3+2] = z;;
  }

  /* Get polygons */
  m->p = (int *)malloc(sizeof(int)*6*m->pn);
  for (i=0; i<m->pn; i++)
  {
    do
    {
      fgets(buf, 128, f);
    } while (isEmpty(buf));
 
    for (j=0; j<(int)strlen(buf); j++)
    {
      if (buf[j] == 'v') a = j+2;
      if (buf[j] == 'n')
      {
        buf[j-1] = 0;
        ParseInt(&buf[a], &kk, &ll, &mm);
        m->p[i*6] = kk;
        m->p[i*6+1] = ll;
        m->p[i*6+2] = mm;
        ParseInt(&buf[j+2], &kk, &ll, &mm);
        m->p[i*6+3] = kk;
        m->p[i*6+4] = ll;
        m->p[i*6+5] = mm;
        break;
      }
    }
  }
  fclose(f);
}
