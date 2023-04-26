#ifndef POWER_MANAGER_H_
#define POWER_MANAGER_H_

class PowerManager {
 public:
  PowerManager();
  ~PowerManager();

  void set_power_pozyx(bool pwr);
  void set_power_led(bool pwr);

  bool get_power_pozyx(void);
  bool get_power_led(void);

  float get_battery_voltage(void);

 private:
};

#endif