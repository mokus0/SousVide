#include "Controller.h"
#include "Temperature.h"
#include "Probes.h"
#include "Relay.h"
#include "Logging.h"
#include <EEPROM.h>

void process_serial_input();

const float default_target_temp_f = 140;

unsigned long print_interval = 1000;
unsigned long next_printout;

void print_status(Stream &dest, long now, double heater_level, double temp_c, double temp_f) {
    float kp, ki, kd;
    get_pid_tuning(&kp, &ki, &kd);
    
    dest.print(now);
    dest.print('\t');
    dest.print(get_pid_mode());
    dest.print('\t');
    dest.print(kp);
    dest.print('\t');
    dest.print(ki);
    dest.print('\t');
    dest.print(kd);
    
    dest.print('\t');
    dest.print(get_pid_setpoint());
    dest.print('\t');
    dest.print(temp_c);
    dest.print('\t');
    dest.print(temp_f);
    dest.print('\t');
    dest.print(heater_level, 4);
    
    dest.println();
}

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
    
    if (init_logging()) {
        Serial.println("Initialized logging successfully");
        
        if (log_file) {
            log_file.println("=== reboot ===");
            log_file.flush();
        } else {
            Serial.println("Just kidding... opened the card but not the file");
        }
    } else {
        Serial.println("Failed to initialize logging");
    }
    
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
        if (Serial) print_status(Serial, now, heater_level, temp_c, temp_f);
        if (log_file) {
            print_status(log_file, now, heater_level, temp_c, temp_f);
            log_file.flush();
        }
        
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
