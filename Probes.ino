#include "Probes.h"
#include "Pins.h"

// I think r_divider is actually a fairly significant
// potential source of error - even fairly minor thermal
// changes may have a fairly big impact - would be good to
// have an extra thermistor on the board next to them
// to get a coarse measurement of ambient temp.

probe_t red_probe = {
    pin:        red_probe_pin,
    decay:      0.999,
    r_divider:  2.163e3,
    
    a:          9.90977226567047e-04,
    b:          2.33837156017689e-04,
    c:          2.49996859812746e-07,
};

probe_t blk_probe = {
    pin:        blk_probe_pin,
    decay:      0.999,
    r_divider:  2.154e3,
    
    a:          8.75766232946504e-04,
    b:          2.54007469940907e-04,
    c:          1.57355406012639e-07,
};

void init_probes() {
    init_probe(&red_probe);
    init_probe(&blk_probe);
}

void read_probes() {
    read_probe(&red_probe);
    read_probe(&blk_probe);
}
