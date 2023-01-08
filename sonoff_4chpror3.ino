/*
 Homekit for SONOFF 4CHPRO R3 by Aurelien DELAMARRE
 Based on examples of lib developed by Mixiaoxiao (Wang Bin)
 
*/

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"
#include <AceButton.h>
#include <EEPROM.h>

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

#define LED_PIN 13
#define EEPROM_SIZE 2048

using namespace ace_button;

// LED states. Some microcontrollers wire their built-in LED the reverse.
const int LED_ON = HIGH;
const int LED_OFF = LOW;

// Number of buttons and LEDs.
const uint8_t NUM_LEDS = 4;

struct Info {
  const uint8_t buttonPin;
  const uint8_t ledPin;
  bool ledState;
};

Info INFOS[NUM_LEDS] = {
  {0, 12, LED_OFF},
  {9, 5, LED_OFF},
  {10, 4, LED_OFF},
  {14, 15, LED_OFF},
};

AceButton buttons[NUM_LEDS];

void setup() {
  Serial.begin(115200);
	wifi_connect(); // in wifi_info.h
	//homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
	my_homekit_setup();
}

void loop() {
	my_homekit_loop();
	delay(5);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;

extern "C" homekit_characteristic_t cha_switch_1;
extern "C" homekit_characteristic_t cha_switch_2;
extern "C" homekit_characteristic_t cha_switch_3;
extern "C" homekit_characteristic_t cha_switch_4;

static uint32_t next_heap_millis = 0;

void my_homekit_setup() {
  pinMode(LED_PIN, OUTPUT);
  EEPROM.begin(EEPROM_SIZE);

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    // initialize built-in LED as an output
    pinMode(INFOS[i].ledPin, OUTPUT);

    // Button uses the built-in pull up register.
    pinMode(INFOS[i].buttonPin, INPUT_PULLUP);

    // Get stored 
    bool storedState;
    EEPROM.get(i+1408, storedState);
    INFOS[i].ledState = storedState;
    digitalWrite(INFOS[i].ledPin, storedState);

    // initialize the corresponding AceButton
    buttons[i].init(INFOS[i].buttonPin, storedState, i);

    switch (i) {
      case 0:
        cha_switch_1.value.bool_value = storedState;
        break;
      case 1:
        cha_switch_2.value.bool_value = storedState;
        break;
      case 2:
        cha_switch_3.value.bool_value = storedState;
        break;
      case 3:
        cha_switch_4.value.bool_value = storedState;
        break;
    }

  }

  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  // buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  // buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  // buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

	cha_switch_1.setter = cha_switch_on_setter_1;
	cha_switch_2.setter = cha_switch_on_setter_2;
	cha_switch_3.setter = cha_switch_on_setter_3;
	cha_switch_4.setter = cha_switch_on_setter_4;

	homekit_characteristic_notify(&cha_switch_1, cha_switch_1.value);
	homekit_characteristic_notify(&cha_switch_1, cha_switch_2.value);
	homekit_characteristic_notify(&cha_switch_1, cha_switch_3.value);
	homekit_characteristic_notify(&cha_switch_1, cha_switch_4.value);

	arduino_homekit_setup(&config);

}

void my_homekit_loop() {


  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    buttons[i].check();
  }

	arduino_homekit_loop();
	const uint32_t t = millis();
	if (t > next_heap_millis) {
		// show heap info every 5 seconds
		next_heap_millis = t + 5 * 1000;
		LOG_D("Free heap: %d, HomeKit clients: %d",
				ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

	}

}

void cha_switch_on_setter_1(const homekit_value_t value) {
  Serial.print(F("Homekit: "));
  handleTrigger(0);
}

void cha_switch_on_setter_2(const homekit_value_t value) {
  Serial.print(F("Homekit: "));
  handleTrigger(1);
}

void cha_switch_on_setter_3(const homekit_value_t value) {
  Serial.print(F("Homekit: "));
  handleTrigger(2);
}

void cha_switch_on_setter_4(const homekit_value_t value) {
  Serial.print(F("Homekit: "));
  handleTrigger(3);
}

void setRelayStateStorred(uint8_t id, bool state) {
  EEPROM.write(id+1408, state);
  EEPROM.commit();
}

void handleTrigger(uint8_t id) {
  uint8_t ledPin = INFOS[id].ledPin;
  bool ledState = !INFOS[id].ledState;

  Serial.print(F("ID: "));
  Serial.print(id);
  Serial.print(F("; LED: "));
  Serial.println(ledState);

  digitalWrite(ledPin, ledState);
  INFOS[id].ledState = ledState;
  setRelayStateStorred(id, ledState);

  switch (id){
    case 0:
      cha_switch_1.value.bool_value = ledState;
      homekit_characteristic_notify(&cha_switch_1, cha_switch_1.value);
      break;
    case 1:
      cha_switch_2.value.bool_value = ledState;
      homekit_characteristic_notify(&cha_switch_2, cha_switch_2.value);
      break;
    case 2:
      cha_switch_3.value.bool_value = ledState;
      homekit_characteristic_notify(&cha_switch_3, cha_switch_3.value);
      break;
    case 3:
      cha_switch_4.value.bool_value = ledState;
      homekit_characteristic_notify(&cha_switch_4, cha_switch_4.value);
      break;
  }

}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.print(F("Button: "));

  // Get the LED pin
  uint8_t id = button->getId();

  // Control the LED only for the Pressed and Released events.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  switch (eventType) {
    case AceButton::kEventPressed:
      handleTrigger(id);      
      break;
  }
}
