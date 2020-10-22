g++ -Wall -D RPI -shared -o libDeviceLib.so -fPIC \
	ArduinoString.cpp \
	ArduinoCore.cpp \
	SPI.cpp \
	Wire.cpp \
	device.cpp \
	actuator.cpp \
	sensor.cpp \
	a4988.cpp \
	beeper.cpp \
	buzzer.cpp \
	dcmotor.cpp \
	dht.cpp \
	ds18.cpp \
	hcsr04.cpp \
	hcsr501.cpp \
	ibutton.cpp \
	led.cpp \
	mfrc522.cpp \
	pn532.cpp \
	rgbled.cpp \
	sound.cpp \
	sqlclient.cpp \
	switch.cpp \
	unistepper.cpp \
	usbcamera.cpp
