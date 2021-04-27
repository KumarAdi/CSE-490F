class Blinker {
  public:
  Blinker(int pin_number, int delay_ms) {
    this->pin_number = pin_number;
    this->delay_ms = delay_ms;
    this->timestamp_of_last_switch = delay_ms;
    this->state = HIGH;
  }

  void loop() {
    unsigned long time_since_last_switch = millis() - this->timestamp_of_last_switch;

    if (time_since_last_switch > this->delay_ms) {
      this->state = !state;
      this->timestamp_of_last_switch += time_since_last_switch;
    }

    digitalWrite(pin_number, state);
  }
  
  private:
  int pin_number;
  int delay_ms;
  int timestamp_of_last_switch;
  int state;
};

Blinker _led1Blinker(2, 200);  // specify pin and blink interval (200ms)
Blinker _led2Blinker(5, 333);  // specify pin and blink interval (333ms)
Blinker _led3Blinker(9, 1111); // specify pin and blink interval (1111ms)

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  _led1Blinker.loop();
  _led2Blinker.loop();
  _led3Blinker.loop();

}
