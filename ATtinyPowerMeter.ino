/*
 * Derived from https://github.com/moononournation/ATtinyPowerMeter  
 */

#include <TinyWireM.h>
#include "ssd1306.h"
#include "ina219.h"

#define CEILING_BUFFER 0.1F
#define CHART_LEFT 0
#define CHART_RIGHT 78

SSD1306 oled;
INA219 power_mon;
static uint8_t i = CHART_LEFT;
static float voltage = 0;
static float current = 0;
static float power = 0;
static float mWh = 0;
static unsigned long last_millis = 0;
static float max_v = 5.0F;
static float max_ma = 10.0F;
static float max_mw = 5.0F;

void setup() {
  TinyWireM.begin();
  oled_init();
  power_mon.begin();
  oled_splash();
  delay(2000);
  oled_labels();
}

void loop() {
  power_read();
  oled_values();
  oled_chart();
}

void oled_init(){
  oled.begin();
  delay(40);
  oled_clear();
}

void oled_clear(){for (uint8_t i = 0; i < 128; i++){oled.v_line(i, 0x00);}}

void oled_splash(){
  oled.set_invert_color(true);
  oled.set_pos(0, 0);
  oled.print("                          ");
  oled.set_pos(0, 1);
  oled.print(" IOT123 POWER METER BOX  ");
  oled.set_invert_color(false);
  oled.set_pos(0, 3);
  oled.print("  MON- MON+   DEV+ DEV-   ");
  oled.set_pos(0, 4);
  oled.print("     | |         | | |    ");
  oled.set_pos(0, 5);
  oled.print("                BAT- BAT+ ");
  oled.set_invert_color(true);
  oled.set_pos(0, 7);
  oled.print(" DEV #3 #4, DEV BAT #4 #5 ");
  oled.set_invert_color(false);
}

void  oled_labels(){
  oled_clear();
  oled.set_pos(128 - 5, 1);
  oled.print("V");
  oled.set_pos(128 - 10, 3);
  oled.print("mA");
  oled.set_pos(128 - 15, 5);
  oled.print("mWh");
  oled.set_pos(128 - 5, 7);
  oled.print("s");
}

void oled_values(){
  oled.set_pos(CHART_RIGHT + 5, 1);
  oled.print_float(voltage, 8);
  oled.set_pos(CHART_RIGHT + 5, 3);
  oled.print_float(current, 7);
  oled.set_pos(CHART_RIGHT + 5, 5);
  oled.print_float(mWh, 5);
  oled.set_pos(CHART_RIGHT + 9, 7);
  oled.print(last_millis / 250);
}

void oled_chart(){
  // auto adjust graph ceiling
  if (voltage >= max_v) max_v = voltage + CEILING_BUFFER;
  if (current >= max_ma) max_ma = current + CEILING_BUFFER;
  if (power > max_mw) max_mw = power + CEILING_BUFFER;
  // plot graph
  oled.plot_value(i, 0, 2, voltage, 0.0F, max_v);
  oled.plot_value(i, 2, 2, current, 0.0F, max_ma);
  oled.plot_area(i, 4, 4, power, 0.0F, max_mw);
  // loop graph position
  i++;
  if (i >= CHART_RIGHT) {
    i = CHART_LEFT;
  }
  // draw current position line
  oled.draw_pattern(i, 0, 1, 8, 0xFF);
}

void power_read(){
  voltage = power_mon.read_bus_voltage();
  current = power_mon.read_current();
  power = power_mon.read_power();
  mWh += power / 1000 * (millis() - last_millis) / 3600;
  last_millis = millis();
}
