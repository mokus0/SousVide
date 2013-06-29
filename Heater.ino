#include "Heater.h"
#include "Pins.h"

relay_t heater = {
    window_size:    5000,
};

void init_heater() {
    init_relay(&heater, 0.8);
    pinMode(heater_pin, OUTPUT);
}

void set_heater(float val) {
    set_relay(&heater, val);
}

void tick_heater(long now) {
    bool on = tick_relay(&heater, now);
//    analogWrite(heater_pin, on ? 2 : 0);
    digitalWrite(heater_pin, on ? HIGH : LOW);
}
