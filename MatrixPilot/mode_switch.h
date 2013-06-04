#ifndef FLIGHT_MODE_SWITCH_H
#define FLIGHT_MODE_SWITCH_H


inline int16_t flight_mode_switch_manual(void);
inline int16_t flight_mode_switch_stabilize(void);
inline int16_t flight_mode_switch_waypoints(void);

void flight_mode_switch_2pos_poll(void); // this is called at 40 hertz and scans the two postion switch option.
void flight_mode_switch_check_set(void); // this is called at 2 hertz and changes the flight mode if req.


#endif // FLIGHT_MODE_SWITCH_H
