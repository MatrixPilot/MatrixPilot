/* 
 * File:   fivePoint.h
 * Author: markw
 *
 * Created on October 11, 2012, 3:58 PM
 */

#ifndef FIVEPOINT_H
#define	FIVEPOINT_H

#ifdef	__cplusplus
extern "C" {
#endif

struct fivePoint {
    unsigned int xseg[5], yseg[5];
    int dx[4], dy[4];
};
void fivePoint_init(struct fivePoint* p5pt,
                    unsigned int xvals[5],
                    unsigned int yvals[5]);
unsigned int fivePointCurve(struct fivePoint* p5pt, unsigned int val);

#ifdef	__cplusplus
}
#endif

#endif	/* FIVEPOINT_H */

