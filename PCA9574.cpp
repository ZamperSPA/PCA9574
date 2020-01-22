// 
// 
// 

#include "PCA9574.h"
#ifdef DEBUG_PCA9574
#define PCA9574_DEBUG(x) if (_debugPort != NULL) { _debugPort->print(x);}
#define PCA9574_DEBUGLN(x) if (_debugPort != NULL) { _debugPort->println(x);}
#define STORAGE(x) (x)
#else
#define PCA9574_DEBUG(x)
#define PCA9574_DEBUGLN(x)
#define STORAGE(x) (x)
#endif


PCA9574::PCA9574()
{
	_i2cPort = NULL;
	_debugPort = NULL;
	_deviceAddress = PCA9574_ADDRESS_INVALID;
	_register = PCA9574_REGISTER_CONFIGURATION;
}

// NB!: Only address 0 or 1
PCA9574_error_t PCA9574::begin(TwoWire& wirePort)
{

	_deviceAddress = PCA9574_DEFAULT_ADDRESS;
	_i2cPort = &wirePort;

	_i2cPort->begin();

	return PCA9574_ERROR_SUCCESS;
}

void PCA9574::setDebugStream(Stream& debugPort)
{
	_debugPort = &debugPort;
}

PCA9574_error_t PCA9574::pinMode(uint8_t pin, uint8_t mode, uint16_t custom_address)
{
	PCA9574_error_t err;
	uint8_t cfgRegister = 0;

	if (pin > PCA9574_BITS) return PCA9574_ERROR_UNDEFINED;

	pin = getBitReg(pin, 3);
	_deviceAddress = PCA9574_DEFAULT_ADDRESS | custom_address; //by Manux

	err = readI2CRegister(&cfgRegister, _register);
	if (err != PCA9574_ERROR_SUCCESS)
	{
		return err;
	}
	cfgRegister &= ~(1 << pin); // Clear pin bit
	if (mode == INPUT) // Set the bit if it's being set to INPUT (opposite of Arduino)
	{
		cfgRegister |= (1 << pin);
	}
	return writeI2CRegister(cfgRegister, _register);
}

PCA9574_error_t PCA9574::portMode(uint8_t mode, uint16_t custom_address)
{
	Serial.println("portmode");
	if (mode == INPUT)
	{
		return writeI2CRegister(0xfF, PCA9574_REGISTER_CONFIGURATION);//todo como entrada
	}
	return writeI2CRegister(0x00, PCA9574_REGISTER_CONFIGURATION);//todos como salida
}

PCA9574_error_t PCA9574::write(uint8_t pin, uint8_t value, uint16_t custom_address)
{
	PCA9574_error_t err;
	uint8_t outputRegister = 0;

	if (pin > PCA9574_BITS) return PCA9574_ERROR_UNDEFINED;

	pin = getBitReg(pin, 1);
	_deviceAddress = PCA9574_DEFAULT_ADDRESS | custom_address; //by Manux
	err = readI2CRegister(&outputRegister, _register);
	if (err != PCA9574_ERROR_SUCCESS)
	{
		return err;
	}
	// TODO: Break out of here if it's already set correctly
	outputRegister &= ~(1 << pin); // Clear pin bit
	if (value == HIGH) // Set the bit if it's being set to HIGH (opposite of Arduino)
	{
		outputRegister |= (1 << pin);
	}
	return writeI2CRegister(outputRegister, _register);
}

PCA9574_error_t PCA9574::digitalPortWrite(uint8_t value, uint16_t custom_address)
{
	if (value == HIGH)
	{
		return writeI2CRegister(0xfF, PCA9574_REGISTER_OUTPUT_PORT);//todo como entrada
	}
	return writeI2CRegister(0x00, PCA9574_REGISTER_OUTPUT_PORT);//todos como salida
}

PCA9574_error_t PCA9574::digitalWrite(uint8_t pin, uint8_t value, uint16_t custom_address)
{
	return write(pin, value, custom_address);
}

uint8_t PCA9574::readReg()
{
	PCA9574_error_t err;
	uint8_t inputRegister = 0;

	err = readI2CRegister(&inputRegister, PCA9574_REGISTER_INPUT_PORT_0);
	if (err != PCA9574_ERROR_SUCCESS)
	{
		return err;
	}
	return (inputRegister & (0x0f));
}

uint8_t PCA9574::read(uint8_t pin, uint16_t custom_address)
{
	PCA9574_error_t err;
	uint8_t inputRegister = 0;

	if (pin > PCA9574_BITS) return PCA9574_ERROR_UNDEFINED;

	pin = getBitReg(pin, 0);
	_deviceAddress = PCA9574_DEFAULT_ADDRESS | custom_address; //by Manux
	err = readI2CRegister(&inputRegister, _register);
	if (err != PCA9574_ERROR_SUCCESS)
	{
		return err;
	}

	return (inputRegister & (1 << pin)) >> pin;
}


uint8_t PCA9574::digitalRead(uint8_t pin, uint16_t custom_address)
{
	return read(pin, custom_address);
}

PCA9574_error_t PCA9574::invert(uint8_t pin, PCA9574_invert_t inversion, uint16_t custom_address)
{
	PCA9574_error_t err;
	uint8_t invertRegister = 0;

	if (pin > PCA9574_BITS) return PCA9574_ERROR_UNDEFINED;

	pin = getBitReg(pin, 2);
	_deviceAddress = custom_address; //by Manux
	err = readI2CRegister(&invertRegister, _register);
	if (err != PCA9574_ERROR_SUCCESS)
	{
		return err;
	}
	// TODO: Break out of here if it's already set correctly
	invertRegister &= ~(1 << pin); // Clear pin bit
	if (inversion == PCA9574_INVERT) // Set the bit if it's being set to inverted
	{
		invertRegister |= (1 << pin);
	}
	return writeI2CRegister(invertRegister, _register);
}

PCA9574_error_t PCA9574::revert(uint8_t pin, uint16_t custom_address)
{
	return invert(pin, PCA9574_RETAIN, custom_address);
}

uint8_t PCA9574::getBitReg(uint8_t pin, uint8_t conf)
{
	if (conf == 0)
		_register = PCA9574_REGISTER_INPUT_PORT_0;
	else if (conf == 1)
		_register = PCA9574_REGISTER_OUTPUT_PORT;
	else if (conf == 2)
		_register = PCA9574_REGISTER_POLARITY_INVERSION_0;
	else if (conf == 3)
		_register = PCA9574_REGISTER_CONFIGURATION;

	if (pin > 7) {
		pin = pin - 8;
		if (conf == 0)
			_register = PCA9574_REGISTER_INPUT_PORT_1;
		else if (conf == 1)
			_register = PCA9574_REGISTER_OUTPUT_PORT_1;
		else if (conf == 2)
			_register = PCA9574_REGISTER_POLARITY_INVERSION_1;
		else if (conf == 3)
			_register = PCA9574_REGISTER_CONFIGURATION_1;
	}
	return pin;
}

PCA9574_error_t PCA9574::readI2CBuffer(uint8_t* dest, PCA9574_REGISTER_t startRegister, uint16_t len)
{
	PCA9574_DEBUGLN((STORAGE("(readI2CBuffer): read ") + String(len) +
		STORAGE(" @ 0x") + String(startRegister, HEX)));
	//Serial.println((STORAGE("(readI2CBuffer): read ") + String(len) +STORAGE(" @ 0x") + String(startRegister, HEX)));
	if (_deviceAddress == PCA9574_ADDRESS_INVALID)
	{
		PCA9574_DEBUGLN(STORAGE("    ERR (readI2CBuffer): Invalid address"));
		return PCA9574_ERROR_INVALID_ADDRESS;
	}
	_i2cPort->beginTransmission((uint8_t)_deviceAddress);
	_i2cPort->write(startRegister);
	if (_i2cPort->endTransmission(false) != 0)
	{
		PCA9574_DEBUGLN(STORAGE("    ERR (readI2CBuffer): End transmission"));
		return PCA9574_ERROR_READ;
	}

	_i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)len);
	for (int i = 0; i < len; i++)
	{
		dest[i] = _i2cPort->read();
		PCA9574_DEBUGLN((STORAGE("    ") + String(i) + STORAGE(": 0x") + String(dest[i], HEX)));
	}

	return PCA9574_ERROR_SUCCESS;
}

PCA9574_error_t PCA9574::writeI2CBuffer(uint8_t* src, PCA9574_REGISTER_t startRegister, uint16_t len)
{
	if (_deviceAddress == PCA9574_ADDRESS_INVALID)
	{
		PCA9574_DEBUGLN(STORAGE("ERR (readI2CBuffer): Invalid address"));
		return PCA9574_ERROR_INVALID_ADDRESS;
	}
	_i2cPort->beginTransmission((uint8_t)_deviceAddress);
	_i2cPort->write(startRegister);
	for (int i = 0; i < len; i++)
	{
		_i2cPort->write(src[i]);
	}

	if (_i2cPort->endTransmission(true) != 0)
	{
		return PCA9574_ERROR_WRITE;
	}
	return PCA9574_ERROR_SUCCESS;
}

PCA9574_error_t PCA9574::readI2CRegister(uint8_t* dest, PCA9574_REGISTER_t registerAddress)
{
	return readI2CBuffer(dest, registerAddress, 1);
}

PCA9574_error_t PCA9574::writeI2CRegister(uint8_t data, PCA9574_REGISTER_t registerAddress)
{
	return writeI2CBuffer(&data, registerAddress, 1);
}

