#include "SousVide.h"

#include "Controller.h"
#include "Probes.h"
#include "Relay.h"
#include "Thermometer.h"

void process_serial_input();
void print_status(long now, double heater_level);

const float target_temp_f = 140;

unsigned long print_interval = 1000;
unsigned long next_printout;

void setup() {
    init_controller(target_temp_f);
    init_heater();
    init_probes();
    
    Serial.begin(9600);
    
    next_printout = millis() + print_interval;
}

void loop() {
    unsigned long now = millis();
    double heater_level;
    
    read_probes();
    heater_level = tick_controller(probe_temp_f(red_probe));
    set_heater(heater_level);
    tick_heater(now);
    
    if (now >= next_printout) {
        process_serial_input();
        print_status(now, heater_level);
        
        next_printout += print_interval;
    }
}

void process_serial_input() {
    while (Serial.available()) {
        char c = Serial.read(); 
        
        switch(c) {
            case '0':
                set_pid_mode(PAUSED);
                break;
            case '1':
                set_pid_mode(RUNNING);
                break;
            case '2':
                set_pid_mode(TUNING);
                break;
            case '+':
            case '=':
                set_pid_setpoint(get_pid_setpoint() + 0.5);
                break;
            case '-':
            case '_':
                set_pid_setpoint(get_pid_setpoint() - 0.5);
                break;
        }
    }
}

void print_status(long now, double heater_level) {
    float kp, ki, kd;
    get_pid_tuning(&kp, &ki, &kd);
    
    Serial.print(now);
    Serial.print('\t');
    Serial.print(get_pid_mode());
    Serial.print('\t');
    Serial.print(kp);
    Serial.print('\t');
    Serial.print(ki);
    Serial.print('\t');
    Serial.print(kd);
    
    Serial.print('\t');
    Serial.print(get_pid_setpoint());
    Serial.print('\t');
    Serial.print(probe_temp_f(red_probe));
    Serial.print('\t');
    Serial.print(probe_temp_f(blk_probe));
    Serial.print('\t');
    Serial.print(heater_level, 4);
    
    Serial.println();
}