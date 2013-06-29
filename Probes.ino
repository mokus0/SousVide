#include "Probes.h"
#include "Pins.h"
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(thermometer_pin);
DallasTemperature thermometer(&oneWire);
DeviceAddress probe_addr = { 0x28, 0x6C, 0xC8, 0x60, 0x04, 0x00, 0x00, 0xB2 };

void init_probe() {
  // 9 bit = 0.5C resolution (0.9F), 12 bit = 0.0625C resolution (0.1125F)
  thermometer.setResolution(probe_addr, 12);
}

double read_ds_probe() {
  thermometer.requestTemperatures();
  return thermometer.getTempC(probe_addr) - 0.1875;
}
