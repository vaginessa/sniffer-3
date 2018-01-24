extern "C" {
#include <user_interface.h>
}

#include "globals.h"
#include "serial_handler.h"
#include "sniffer.h"
#include <TaskScheduler.h>

#include <SPI.h>
#include <SD.h>

Scheduler runner;

const int chip_select = 15;

void blinker() {
	digitalWrite(2, 1);
}

Task serial_handler_task(200, TASK_FOREVER, &serial_handler);

void setup() {
	// set the WiFi chip to "promiscuous" mode aka monitor mode
	Serial.begin(921600);
	Serial.setTimeout(1);
	delay(10);

	initialize_globals();

	runner.init();
	runner.addTask(serial_handler_task);
	serial_handler_task.enable();

	wifi_set_opmode(STATION_MODE);
	wifi_set_channel(1);
	wifi_promiscuous_enable(0);
	delay(10);
	wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
	delay(10);
	wifi_promiscuous_enable(1);
	pinMode(2, OUTPUT);



	Serial.print("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chip_select)) {
		Serial.println("Card failed, or not present");
		// don't do anything more:
		while (1);
	}
	Serial.println("card initialized.");
}

void loop() {
	runner.execute();
	// Serial handler
	if (Serial.available() > 0) {
		serial_intr_handler(NULL);
	}
}
