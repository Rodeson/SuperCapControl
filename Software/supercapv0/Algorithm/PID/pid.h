#ifndef _PID_H
#define _PID_H

#define SGN(x) ((x >= 0) ? 1 : -1)
#define ABS(x) (((x) >= 0)? (x) : -(x))

#include "main.h"
#include "math.h"

#define PID_POSITION 0
#define PID_DELTA 1

typedef struct
{
	uint8_t mode;
	
	float kp;
	float ki;
	float kd;
	
	float set;//reffer,set
	float fdb;//feedback
	
	float err[3];
	
	float Pout;
    float Iout;
    float Dout;

	float out;
	float max_iout;
	float max_out;
	
}PID_Regulator_t;

struct pid_param
{
    float p;
    float i;
    float d;
    float input_max_err;

    float max_out;
    float integral_limit;
};

struct pid
{
    struct pid_param param;

    uint8_t enable;

    float set;
    float get;

    float err;
    float last_err;

    float pout;
    float iout;
    float dout;
    float out;

    void (*f_param_init)(struct pid* pid,
                         float max_output,
                         float integral_limit,
                         float p,
                         float i,
                         float d);
    void (*f_pid_reset)(struct pid* pid, float p, float i, float d);
};

extern float PID_Calculate(PID_Regulator_t *pid, float fdb, float set);
extern void PID_Init(PID_Regulator_t *pid, uint8_t mode,float maxout, float max_iout, float kp, float ki, float kd);




#endif
