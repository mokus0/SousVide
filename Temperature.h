#ifndef ___n_Temperature_h__
#define ___n_Temperature_h__

inline float kelvin_to_centigrade(float k) {
    return k - 273.15;
}

inline float centigrade_to_kelvin(float c) {
    return c + 273.15;
}

inline float centigrade_to_fahrenheit(float c) {
    return 32 + 1.8 * c;
}

inline float fahrenheit_to_centigrade(float f) {
    return (f - 32) / 1.8;
}

#endif /* ___n_Temperature_h__ */
