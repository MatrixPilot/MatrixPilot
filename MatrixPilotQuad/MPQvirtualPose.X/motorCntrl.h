/* 
 * File:   motorCntrl.h
 * Author: markw
 *
 * Created on February 10, 2013, 1:12 PM
 */

#ifndef MOTORCNTRL_H
#define	MOTORCNTRL_H

#ifdef	__cplusplus
extern "C" {
#endif

struct int_RPY {
    int roll, pad1;
    int pitch, pad2;
    int yaw, pad3;
};
extern struct int_RPY cmd_RPY;

extern int pwManual[];
extern int poscmd_north, poscmd_east;
extern int roll_error, pitch_error, yaw_error, yaw_rate_error;
extern int rotz, roty, rotx;
extern int rate_error[3], rate_error_dot[2];
extern int rate_des_damping[3], rate_desired[3];
extern int rolladvanced, pitchadvanced;
extern signed char lagBC, precessBC;
extern unsigned int desired_heading, earth_yaw;
extern int roll_control, pitch_control, yaw_control, accel_feedback;
extern int pos_error[], pos_perr[], pos_derr[];
extern union longww roll_error_integral, pitch_error_integral, yaw_error_integral;
extern union longww rrate_error_integral, prate_error_integral, yrate_error_integral;
extern unsigned int pid_gains[PID_GAINS_N];
extern unsigned int throttle_limit;

#ifdef	__cplusplus
}
#endif

#endif	/* MOTORCNTRL_H */

