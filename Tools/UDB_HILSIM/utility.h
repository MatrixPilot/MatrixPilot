inline int signum(float a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }
inline int signum(double a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }
double GetHeading(double lat1, double lat2, double lon1, double lon2);
double GetDistance(double lat1, double lat2, double lon1, double lon2);
double GetCTE(double current_lat, double current_long, double dest_lat, double dest_lon, double track_angle);
double LimitValue(double &input_val, double limit_mag);
float LimitValue(float &input_val, float limit_mag);
void GetPosWHdngDist(double lat1, double lon1, double hdng, double dist, double &new_lat, double &new_long);
void LLAtoECEF(double lat, double lon, double alt, double &x, double &y, double &z);
void BCBFtoOGL(float &x, float &y, float &z, float phi, float theta, float psi);
void FLIGHTtoBCBF(float &x, float &y, float &z, float alpha, float beta);
void OGLtoBCBF(float &x, float &y, float &z, float phi, float theta, float psi);

struct ww { int W0 ; int W1 ; } ;
union longww { long  WW ; struct ww _ ; } ;

struct bbbb { unsigned char B0 ; unsigned char B1 ; unsigned char B2 ; unsigned char B3 ; } ;
struct bb { unsigned char B0 ; unsigned char B1 ; } ;

union intbb { short int BB ; struct bb _ ; } ;
union longbbbb { int WW ; struct ww _ ; struct bbbb __ ; } ;