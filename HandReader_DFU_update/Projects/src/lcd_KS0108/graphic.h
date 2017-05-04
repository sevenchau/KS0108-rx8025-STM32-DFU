#ifndef __GRAPHC_H
#define __GRAPHC_H

void GLCD_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void GLCD_RectangleAntiColor(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void GLCD_RectangleDotted(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void GLCD_Circle(unsigned char cx, unsigned char cy ,unsigned char radius);
void GLCD_Line(int X1, int Y1,int X2,int Y2);

#endif