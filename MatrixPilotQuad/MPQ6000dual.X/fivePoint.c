// 5-point throttle curve: thr_curve[5]
// maps throttle input values of [x0, x1)[x1, x2)[x2, x3)[x3, x4]
// to a piecewise linear output range [y0, y1)[y1, y2)[y2, y3)[y3, y4]
// deltax and deltay are precomputed differences x(i+1)-x(i)

#include "fivePoint.h"

void fivePoint_init(struct fivePoint* p5pt,
                    unsigned int xvals[5],
                    unsigned int yvals[5]) {
    int i;
    for (i = 0; i < 5; i++) {
        p5pt->xseg[i] = xvals[i];
        p5pt->yseg[i] = yvals[i];
    }
    for (i = 0; i < 4; i++) {
        p5pt->dx[i] = p5pt->xseg[i + 1] - p5pt->xseg[i];
        p5pt->dy[i] = p5pt->yseg[i + 1] - p5pt->yseg[i];
    }
}

unsigned int fivePointCurve(struct fivePoint* p5pt, unsigned int val) {
    unsigned int result;
    int i;

    if (val < p5pt->xseg[0]) {
        // out of range low
        result = p5pt->yseg[0];
    } else if (val > p5pt->xseg[4]) {
        // out of range high
        result = p5pt->yseg[4];
    } else {
        for (i = 0; i < 4; i++) {
            if (val < p5pt->xseg[i + 1]) {
                // segment i
                long temp = __builtin_mulus((val - p5pt->xseg[i]), p5pt->dy[i]);
                result = __builtin_divsd(temp, p5pt->dx[i]) + p5pt->yseg[i];
                break;
            }
        }
    }
    return result;
}
