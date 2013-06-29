#include "Controller.h"
#include "Temperature.h"
#include "Probes.h"
#include "Relay.h"
#include <EEPROM.h>

void process_serial_input();
void print_status(long now, double heater_level);

const float default_target_temp_f = 140;

unsigned long print_interval = 1000;
unsigned long next_printout;

float EEPROM_readFloat(int x) {
    union {
        float f;
        uint8_t b[sizeof(float)];
    } buf;
    
    for (int i = 0; i < sizeof(float); i++) {
        buf.b[i] = EEPROM.read(x + i);
    }
    
    return buf.f;
}

void EEPROM_writeFloat(int x, float f) {
    union {
        float f;
        uint8_t b[sizeof(float)];
    } buf;
    
    buf.f = f;
    
    for (int i = 0; i < sizeof(float); i++) {
        EEPROM.write(x + i, buf.b[i]);
    }
}

void setup() {
    float target_temp_f = default_target_temp_f;
    
    if (EEPROM.read(0)) {
        target_temp_f = EEPROM_readFloat(1);
        if (!(target_temp_f > 0 && target_temp_f < 212)) target_temp_f = default_target_temp_f;
    }
    
    init_controller(target_temp_f);
    init_heater();
    init_probe();
    
    Serial.begin(9600);
    
    next_printout = millis() + print_interval;
}

void loop() {
    unsigned long now = millis();
    double temp_c = read_ds_probe();
    double temp_f = centigrade_to_fahrenheit(temp_c);
    double heater_level;
    
    heater_level = tick_controller(temp_f);
    set_heater(heater_level);
    tick_heater(now);
    
    if (now >= next_printout) {
        process_serial_input();
        print_status(now, heater_level, temp_c, temp_f);
        
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
                EEPROM.write(0, 1);
                EEPROM_writeFloat(1, get_pid_setpoint());
                break;
            case '-':
            case '_':
                set_pid_setpoint(get_pid_setpoint() - 0.5);
                EEPROM.write(0, 1);
                EEPROM_writeFloat(1, get_pid_setpoint());
                break;
        }
    }
}

void print_status(long now, double heater_level, double temp_c, double temp_f) {
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
    Serial.print(temp_c);
    Serial.print('\t');
    Serial.print(temp_f);
    Serial.print('\t');
    Serial.print(heater_level, 4);
    
    Serial.println();
}
