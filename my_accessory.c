/*
 * my_accessory.c
 * Define the accessory in C language using the Macro in characteristics.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
}

// Switch (HAP section 8.38)
// required: ON
// optional: NAME

// format: bool; HAP section 9.70; write the .setter function to get the switch-event sent from iOS Home APP.
homekit_characteristic_t cha_switch_1 = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_switch_2 = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_switch_3 = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_switch_4 = HOMEKIT_CHARACTERISTIC_(ON, false);

homekit_characteristic_t cha_programmable_switch_event = HOMEKIT_CHARACTERISTIC_(PROGRAMMABLE_SWITCH_EVENT, 0);

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_bridge, .services=(homekit_service_t*[]) {
    	// HAP section 8.17:
    	// For a bridge accessory, only the primary HAP accessory object must contain this(INFORMATION) service.
    	// But in my test,
    	// the bridged accessories must contain an INFORMATION service,
    	// otherwise the HomeKit will reject to pair.
    	HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Multiple Channel"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Sonoff"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "003100151B"),
            HOMEKIT_CHARACTERISTIC(MODEL, "4CHPRO3"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
        NULL
    }),
	HOMEKIT_ACCESSORY(.id=2, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
    	HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH1"),
			HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
			NULL
		}),
    	HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH1"),
			&cha_switch_1,
			NULL
		}),
		NULL
	}),
	HOMEKIT_ACCESSORY(.id=3, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
    	HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH2"),
			HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
			NULL
		}),
    	HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH2"),
			&cha_switch_2,
			NULL
		}),
		NULL
	}),
	HOMEKIT_ACCESSORY(.id=4, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
    	HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH3"),
			HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
			NULL
		}),
    	HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH3"),
			&cha_switch_3,
			NULL
		}),
		NULL
	}),
	HOMEKIT_ACCESSORY(.id=5, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
    	HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH4"),
			HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
			NULL
		}),
    	HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
			HOMEKIT_CHARACTERISTIC(NAME, "CH4"),
			&cha_switch_4,
			NULL
		}),
		NULL
	}),
  NULL
};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "111-11-111"
};


