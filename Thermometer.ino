#include "Thermometer.h"

inline float ohms(float analog, float r_divider) {
    float alpha = analog / 1023.0;
    return (1 - alpha) / alpha * r_divider;
}

void init_probe(struct probe_t *probe) {
    probe ->pin_value = (float) analogRead(probe->pin);
    read_probe(probe);
}

void read_probe(struct probe_t *probe) {
    float pin_value = (float) analogRead(probe->pin);
    
    probe->pin_value = probe->pin_value * probe->decay 
                     + pin_value * (1 - probe->decay);
    probe->r_probe = ohms(probe->pin_value, probe->r_divider);
}
