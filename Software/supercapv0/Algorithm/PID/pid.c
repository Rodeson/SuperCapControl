#include "pid.h"

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }


void PID_Init(PID_Regulator_t *pid, uint8_t mode,float maxout, float max_iout, float kp, float ki, float kd)
{
	if (pid == NULL )
    {
        return;
    }
	
	pid->mode = mode;
	pid->max_iout = max_iout;
	pid->max_out = maxout;
	pid->kp = kp;
	pid->kd = kd;
	pid->ki = ki;
	pid->err[0] = pid->err[1] = pid->err[2] = 0.0f;
	
}


float PID_Calculate(PID_Regulator_t *pid, float fdb, float set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];

	pid->set = set;
    pid->fdb = fdb;
    pid->err[0] = set - fdb;
    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->kp * pid->err[0];
        pid->Iout += pid->ki * pid->err[0];
        pid->Dout = pid->kd *(pid->err[0] - pid->err[1]);
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->kp * (pid->err[0] - pid->err[1]);
        pid->Iout = pid->ki * pid->err[0];
        pid->Dout = pid->kd * (pid->err[0] - 2.0f * pid->err[1] + pid->err[2]);
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}


