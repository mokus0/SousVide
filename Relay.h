#ifndef ___n_Relay_h__
#define ___n_Relay_h__

struct relay {
    const long window_size;
    float set_point; // 0 to 1
    long cur_window;
    long off_time;
    long next_window;
};

typedef struct relay relay_t;

void init_relay(struct relay *relay, float set_point);
bool tick_relay(struct relay *relay, long now);
void set_relay(struct relay *relay, float set_point);

#endif /* ___n_Relay_h__ */
