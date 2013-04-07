#include "Relay.h"

void init_relay(struct relay *relay, float set_point) {
    long now = millis();
    
    relay->next_window = now;
    relay->set_point = set_point;
    
    tick_relay(relay, now);
}

bool tick_relay(struct relay *relay, long now) {
    while (now >= relay->next_window) {
        relay->cur_window   = relay->next_window;
        relay->next_window  = relay->cur_window + relay->window_size;
        
        relay->off_time     = relay->cur_window 
                            + relay->window_size * relay->set_point;
    }
    
    return (now < relay->off_time);
}

void set_relay(struct relay *relay, float set_point) {
    relay->set_point    = set_point;
    relay->off_time     = relay->cur_window 
                        + relay->window_size * relay->set_point;
}
