# 1 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.c"
# 1 "/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/educational/fractionalArithmetic//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.c"
# 21 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.c"
# 1 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.h" 1
# 24 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.h"
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 1 3 4




# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 1 3 4
# 86 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4
typedef void *va_list;
# 102 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4

# 135 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4
typedef long long _Longlong;
typedef unsigned long long _ULonglong;
# 149 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4
typedef short unsigned int _Wchart;
typedef unsigned int _Wintt;
# 161 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4
typedef int _Ptrdifft;
typedef unsigned int _Sizet;



int _Setjmp(int *);


typedef va_list _Va_list;





void _Atexit(void (*)(void));


typedef struct _Mbstatet
 {
 _Wchart _Wchar;
 unsigned short _Byte, _State;
 } _Mbstatet;
# 191 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4
typedef struct _Fpost
 {
 long _Off;
 _Mbstatet _Wstate;
 } _Fpost;
# 212 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/yvals.h" 3 4

# 6 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 2 3 4


# 36 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 3 4
typedef _Sizet size_t;




typedef _Wchart wchar_t;


typedef struct
 {
 int quot;
 int rem;
 } div_t;

typedef struct
 {
 long quot;
 long rem;
 } ldiv_t;


typedef struct
 {
 _Longlong quot;
 _Longlong rem;
 } _Lldiv_t;




void _Exit(int) ;
void exit(int) ;
char * getenv(const char *);
int system(const char *);



void abort(void) ;
int abs(int);
void * calloc(size_t, size_t);
div_t div(int, int);
void free(void *);
long labs(long);
ldiv_t ldiv(long, long);
void * malloc(size_t);
int mblen(const char *, size_t);
size_t mbstowcs(wchar_t *, const char *, size_t);
int mbtowc(wchar_t *, const char *, size_t);
int rand(void);
void srand(unsigned int);
void * realloc(void *, size_t);
long strtol(const char *, char **, int);
size_t wcstombs(char *, const wchar_t *, size_t);
int wctomb(char *, wchar_t);
# 102 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 3 4
unsigned long _Stoul(const char *, char **, int);
float _Stof(const char *, char **, long);






long double _Stold(const char *, char **, long);
_Longlong _Stoll(const char *, char **, int);
_ULonglong _Stoull(const char *, char **, int);

extern size_t _Getmbcurmax(void);

# 191 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 3 4


typedef int _Cmpfun(const void *, const void *);

int atexit(void (*)(void));
void * bsearch(const void *, const void *, size_t, size_t, _Cmpfun *);
void qsort(void *, size_t, size_t, _Cmpfun *);

double atof(const char *);
double strtod(const char *, char **);
int atoi(const char *);
long atol(const char *);
unsigned long strtoul(const char *, char **, int);
# 216 "/opt/microchip/xc16/v1.10/bin/bin/../../include/lega-c/stdlib.h" 3 4








# 25 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.h" 2



union int32_w2 { long longword; struct {int low; int high;} word; } ;
inline int lp2(int input, union int32_w2 *state, unsigned int lpcb);
# 46 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.h"
struct boxCarState {
    int len;
    int N;
    int* buff;
    long* sum;
    int index;
};

void boxcar(int *value, struct boxCarState* state, int* result);
void init_boxCarState(int len, int N, int* buff, long* sum, struct boxCarState* state);
# 22 "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libUDB/filters.c" 2

inline int lp2(int input, union int32_w2 *state, unsigned int lpcb) {
    state->longword -= __builtin_mulus(lpcb, state->word.high);
    state->longword += __builtin_mulus(lpcb, input);
    return (state->word.high);
}

void init_boxCarState(int len, int N, int* buff, long* sum, struct boxCarState* state)
{
    int i;
    state->N = N;
    state->len = len;
    state->index = 0;
    state->buff = buff;
    for (i=0; i<N*len; i++) buff[i] = 0;
    state->buff = buff;
    state->sum = sum;
    for (i=0; i<N; i++) sum[i] = 0;
}

void boxcar(int* value, struct boxCarState* state, int* result)
{
    int i, offset;
    state->index++;
    if (state->index >= state->len)
        state->index = 0;
    for (i = 0; i < state->N; i++)
    {
        offset = i * state->len + state->index;
        state->sum[i] -= state->buff[offset];
        state->buff[offset] = value[i];
        state->sum[i] += value[i];
        result[i] = state->sum[i] / state->len;
    }
}
