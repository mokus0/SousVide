#ifndef ___n_Controller_h__
#define ___n_Controller_h__

enum controller_mode {
    PAUSED,
    RUNNING,
    TUNING,
};

void init_controller(float setpoint);

float get_pid_setpoint();
controller_mode get_pid_mode();
void get_pid_tuning(float *kp, float *ki, float *kd);

void set_pid_setpoint(float setpoint);
bool set_pid_mode(controller_mode mode);
void set_pid_tuning(float kp, float ki, float kd);

float tick_controller(float input);

#endif /* ___n_Controller_h__ */
