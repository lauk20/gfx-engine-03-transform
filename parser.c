#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char line[256];
  clear_screen(s);
  color c;
  c.red = 255;
  c.green = 255;
  c.blue = 255;

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);

    if (strcmp(line, "line") == 0){
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      double x0, y0, z0, x1, y1, z1;
      x0 = 0;
      y0 = 0;
      z0 = 0;
      x1 = 0;
      y1 = 0;
      z1 = 0;

      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);

      add_edge(edges, x0, y0, z0, x1, y1, z1);
    } else if (strcmp(line, "ident") == 0){
      ident(transform);
    } else if (strcmp(line, "scale") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      double sx = 0, sy = 0, sz = 0;

      sscanf(line, "%lf %lf %lf", &sx, &sy, &sz);

      matrix_mult(make_scale(sx, sy, sz), transform);
    } else if (strcmp(line, "translate") == 0 || strcmp(line, "move") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      double tx = 0, ty = 0, tz = 0;

      sscanf(line, "%lf %lf %lf", &tx, &ty, &tz);

      matrix_mult(make_translate(tx, ty, tz), transform);
    } else if (strcmp(line, "rotate") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char axis;
      double theta = 0;

      sscanf(line, "%c %lf", &axis, &theta);

      if (axis == 'x'){
        matrix_mult(make_rotX(theta), transform);
      } else if (axis == 'y'){
        matrix_mult(make_rotY(theta), transform);
      } else if (axis == 'z'){
        matrix_mult(make_rotZ(theta), transform);
      }
    } else if (strcmp(line, "apply") == 0){
      matrix_mult(transform, edges);
    } else if (strcmp(line, "display") == 0){
      clear_screen(s);
      draw_lines(edges, s, c);
      display(s);
    } else if (strcmp(line, "save") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      clear_screen(s);
      draw_lines(edges, s, c);

      char name[256];

      sscanf(line, "%s", name);

      save_extension(s, name);
    } else if (strcmp(line, "quit") == 0){
      break;
    }
  }
}
