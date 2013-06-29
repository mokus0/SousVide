#include "Logging.h"
#include "Pins.h"

#include <SD.h>

File log_file;

bool init_logging() {
    pinMode(sd_card_ss, OUTPUT);
    
    bool initialized = SD.begin(sd_card_ss);
    if (initialized) {
        log_file = SD.open("temp.log", FILE_WRITE);
    }
    
    return initialized;
}
