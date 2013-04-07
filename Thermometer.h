#ifndef ___n_Thermometer_h__
#define ___n_Thermometer_h__

#include "Temperature.h"
#include <math.h>

struct probe_t {
    const int pin;
    const float decay;
    const float r_divider;
    const float a, b, c;
    float pin_value;
    float r_probe;
};

typedef struct probe_t probe_t;

void init_probe(struct probe_t *probe);
void read_probe(struct probe_t *probe);

inline float probe_ohms(const probe_t probe) {
    return probe.r_probe;
}

inline float probe_temp_k(const probe_t probe) {
    float ln_r = log(probe.r_probe);
    return 1 / (probe.a + probe.b * ln_r + probe.c * ln_r * ln_r * ln_r);
}

inline float probe_temp_c(const probe_t probe) {
    return kelvin_to_centigrade(probe_temp_k(probe));
}

inline float probe_temp_f(const probe_t probe) {
    return centigrade_to_fahrenheit(probe_temp_c(probe));
}

#endif /* ___n_Thermometer_h__ */
