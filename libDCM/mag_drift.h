



extern union intbb dcm_declination_angle;

extern fractional magFieldEarth[3];
extern fractional rmatPrevious[9];
extern fractional magFieldEarthNormalizedPrevious[3];
extern fractional magAlignment[4];
extern fractional magFieldBodyMagnitudePrevious;
extern fractional magFieldBodyPrevious[3];


void mag_drift_init(void);
void mag_drift(fractional errorYawplane[]);
void mag_drift_callback(void);
