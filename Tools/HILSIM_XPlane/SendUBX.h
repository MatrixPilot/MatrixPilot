#pragma once

void Send_NAV_KEYSTROKE(int inFlags, char inVirtualKey);

void Send_NAV_BODYRATES(float P_plane, float Q_plane, float R_plane,
                        float X_accel, float Y_accel, float Z_accel);

void Send_NAV_VELNED(int LocalSecsInt,
                     double local_vx, double local_vy, double local_vz,
                     double air_speed, double speed_over_ground,
                     double course_over_ground);

void Send_NAV_POSLLH(int LocalSecsInt,
                     double latitude, double longitude, double elevation);

void Send_NAV_SOL(int LocalSecsInt, float LocalSecsFloat, int Week, 
                  double local_x,  double local_y,  double local_z, 
                  double local_vx, double local_vy, double local_vz,
                  float mag_x, float mag_y, float mag_z);

