#include "display.h"
#include "triangle.h"


static void int_swap(int* a, int* b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}



static void fill_flat_bottom_triangle(int x0,int y0,int x1,int y1,int x2,int y2,uint32_t color){
    // slope inverse -> change in x since y always increments by 1 for this line angle.
    float inv_slope1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope2 = (float)(x2 - x0) / (y2 - y0);
    // start x_start and x_end from top vertex x0,y0
    // loop all scanlines from top to bottom
    float x_start = x0;
    float x_end = x0;
    for (int y = y0; y <= y2; y++){
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope1;
        x_end += inv_slope2;
    }
}
static void fill_flat_top_triangle(int x0,int y0,int x1,int y1,int x2,int y2, uint32_t color){
    // slope inverse -> change in x since y always increments by 1 for this line angle.
    float inv_slope1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope2 = (float)(x2 - x1) / (y2 - y1);
    // start x_start and x_end from bottom vertex x2,y2
    // loop all scanlines from bottom to top
    float x_start = x2;
    float x_end = x2;
    for (int y = y2; y >= y0; y--){
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope1;
        x_end -= inv_slope2;
    }
}


void draw_filled_triangle(int x0, int y0,int x1, int y1,int x2, int y2,uint32_t color){
    // sort vertices by y-coordinate ascending
    if (y0 > y1){
        int_swap(&y0,&y1);
        int_swap(&x0,&x1);
    }
    if (y1 > y2){
        int_swap(&y1,&y2);
        int_swap(&x1,&x2);
    }
    if (y0 > y1){
        int_swap(&y0,&y1);
        int_swap(&x0,&x1);
    }
    // avoid divison by zero errors
    if (y1 == y2){
        // draw flat botoom triangle
        fill_flat_bottom_triangle(x0,y0,x1,y1,x2,y2,color);
    } else if (y0 == y1){
        // draw flat top triangle
        fill_flat_top_triangle(x0,y0,x1,y1,x2,y2,color);
    } else {
        int My = y1;
        int Mx = ((float)(x2 - x0) * (y1 - y0) / (float)(y2 - y0)) + x0;
        fill_flat_bottom_triangle(x0,y0,x1,y1,Mx,My,color);
        fill_flat_top_triangle(x1,y1,Mx,My,x2,y2,color);
    }

}

