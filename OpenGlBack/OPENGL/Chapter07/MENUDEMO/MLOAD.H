struct model
{
  int vn; /* Vertex number */
  int nn; /* Normal number */
  int pn; /* Polygon number */

  float *v; /* Vertexs */
  float *n; /* Normals */
  int   *p; /* Polygons */
};

void LoadMFile(char *filename, struct model *m);
