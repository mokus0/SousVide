#include "Controller.h"

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

void begin_tuning();
void end_tuning();

// 1st tuning run:
//double kp=17.63,ki=0.15,kd=0.00;
// 2nd tuning run:
//double kp=5.67, ki=0.07, kd=0.00;
// 3rd run (undamped inputs):
//double kp = 8.94,
//       ki = 0.16,
//       kd = 0.00;

double  kp = 35.25,
        ki = 0.63,
        kd = 0.00;

// TODO: find out if there's any reason not to use, say, 1e6 here
const float pid_range = 1023;

controller_mode pid_mode = PAUSED;
controller_mode saved_mode = PAUSED; // mode to return to after tuning

double pid_input, pid_output, pid_setpoint;
PID controller(&pid_input, &pid_output, &pid_setpoint, kp, ki, kd, DIRECT);

const double autotune_base  = 40;
const double autotune_step  = 40;
const double autotune_noise = 0.25;
const int autotune_lookback = 20;
PID_ATune tuner(&pid_input, &pid_output);

void init_controller(float setpoint) {
    pid_input = 0;
    pid_setpoint = setpoint;
    
    controller.SetOutputLimits(0, pid_range);
    set_pid_mode(RUNNING);
}

float get_pid_setpoint() {
    return pid_setpoint;
}

controller_mode get_pid_mode() {
    return pid_mode;
}

void get_pid_tuning(float *out_kp, float *out_ki, float *out_kd) {
    *out_kp = kp;
    *out_ki = ki;
    *out_kd = kd;
}

void set_pid_setpoint(float setpoint) {
    pid_setpoint = setpoint;
}

bool set_pid_mode(controller_mode mode) {
    bool mode_accepted = true;
    
    switch(mode) {
        case PAUSED:
            controller.SetMode(MANUAL);
            break;
        case RUNNING:
            controller.SetMode(AUTOMATIC);
            break;
        case TUNING:
            if (pid_mode != TUNING) {
                begin_tuning();
            }
            
            break;
        default:
            mode_accepted = false;
            break;
    }
    
    if (mode_accepted && pid_mode != mode) {
        if (pid_mode == TUNING) {
            tuner.Cancel();
        }
        
        pid_mode = mode;
    }
    
    return mode_accepted;
}

void begin_tuning() {
    saved_mode = pid_mode;
    
    // if using a specified baseline output,
    // set it before starting the tuner.
    if (autotune_base > 0) pid_output = autotune_base;
    
    // step size is as given, but clamp it if necessary so output stays in range
    double step = autotune_step;
    if (pid_output < step) step = pid_output;
    if (pid_range - pid_output < step) step = pid_range - pid_output;
    
    tuner.SetNoiseBand(autotune_noise);
    tuner.SetOutputStep(step);
    tuner.SetLookbackSec(autotune_lookback);
}

void end_tuning() {
    kp = tuner.GetKp();
    ki = tuner.GetKi();
    kd = tuner.GetKd();
    controller.SetTunings(kp,ki,kd);
    
    // this shouldn't happen, but just in case...
    if (saved_mode == TUNING) saved_mode = RUNNING;
    
    set_pid_mode(saved_mode);
}

float tick_controller(float input) {
    pid_input = input;
    
    switch (pid_mode) {
        case RUNNING:
        case PAUSED:
            controller.Compute();
            break;
        
        case TUNING:
            if (tuner.Runtime()) {
                end_tuning();
            }
            break;
    }
    
    return pid_output / pid_range;
}
