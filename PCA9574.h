// PCA9574.h

#ifndef _PCA9574_h
#define _PCA9574_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Wire.h>

//#define DEBUG_PCA9574

#define PCA9574_BITS 8
#define PCA9574_DEFAULT_ADDRESS 0x20
#define PCA9574_ADDRESS_INVALID 0xFF

// PCA9574 error code returns:
typedef enum {
	PCA9574_ERROR_READ = -4,
	PCA9574_ERROR_WRITE = -3,
	PCA9574_ERROR_INVALID_ADDRESS = -2,
	PCA9574_ERROR_UNDEFINED = -1,
	PCA9574_ERROR_SUCCESS = 1
} PCA9574_error_t;
const PCA9574_error_t PCA9574_SUCCESS = PCA9574_ERROR_SUCCESS;

typedef enum {
	PCA9574_REGISTER_INPUT_PORT_0 = 0x00, //0
	PCA9574_REGISTER_POLARITY_INVERSION = 0x01, //2
	PCA9574_REGISTER_BUS_HOLD = 0X02,
	PCA9574_REGISTER_PULL_UP_DOWN = 0X03,
	PCA9574_REGISTER_CONFIGURATION_0 = 0x04, //3
	PCA9574_REGISTER_OUTPUT_PORT = 0x02, //1
	PCA9574_REGISTER_INTERRUPT_MASK = 0X06,
	PCA9574_REGISTER_INTERRUPT_STATUS=0X07,
	PCA9574_REGISTER_INVALID
} PCA9574_REGISTER_t;

// PCA9574 invert/normal values:
typedef enum {
	PCA9574_RETAIN,
	PCA9574_INVERT,
	PCA9574_INVERT_END
} PCA9574_invert_t;
/*
 TODO: If I understood the manual correctly, the command only needs to be sent to change active register
 so for performance I want to store the last used register, and if you are for example reading multiple times,
 you would not need to resend the command the second time to read two more bytes. And the same way about writing.
 The registers are paired, so each byte writes to register (REGISTER | (i % 2)) indefinitely.
 */

class PCA9574 {
public:
	PCA9574();

	// begin initializes the Wire port and I/O expander
	boolean begin(void);
	// give begin a TwoWire port to specify the I2C port
	PCA9574_error_t begin(TwoWire& wirePort);

	// setDebugStream to enable library debug statements
	void setDebugStream(Stream& debugPort = Serial);

	// pinMode can set a pin (0-3) to INPUT or OUTPUT
	PCA9574_error_t pinMode(uint8_t pin, uint8_t mode, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);

	PCA9574_error_t portMode(uint8_t mode, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);

	// digitalWrite and write can be used to set a pin HIGH or LOW
	PCA9574_error_t digitalWrite(uint8_t pin, uint8_t value, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);
	PCA9574_error_t write(uint8_t pin, uint8_t value, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);


	PCA9574_error_t digitalPortWrite(uint8_t value, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);

	// readReg can be used to read the whole input register (4 bits)
	uint8_t readReg();

	// digitalRead and read can be used to read a pin (0-3)
	uint8_t digitalRead(uint8_t pin, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);
	uint8_t read(uint8_t pin, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);

	// invert and revert can be used to invert (or not) the I/O logic during a read
	PCA9574_error_t invert(uint8_t pin, PCA9574_invert_t inversion = PCA9574_INVERT, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);
	PCA9574_error_t revert(uint8_t pin, uint16_t custom_address = PCA9574_DEFAULT_ADDRESS);

private:
	TwoWire* _i2cPort;
	Stream* _debugPort;
	uint16_t _deviceAddress;
	PCA9574_REGISTER_t _register;
	//
	uint8_t getBitReg(uint8_t pin, uint8_t conf);
	// I2C Read/Write
	PCA9574_error_t readI2CBuffer(uint8_t* dest, PCA9574_REGISTER_t startRegister, uint16_t len);
	PCA9574_error_t writeI2CBuffer(uint8_t* src, PCA9574_REGISTER_t startRegister, uint16_t len);
	PCA9574_error_t readI2CRegister(uint8_t* dest, PCA9574_REGISTER_t registerAddress);
	PCA9574_error_t writeI2CRegister(uint8_t data, PCA9574_REGISTER_t registerAddress);
};

#endif

