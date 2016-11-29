
// STL includes
#include <cstring>
#include <iostream>

// hyperion local includes
#include "LedDeviceKarate.h"

LedDeviceKarate::LedDeviceKarate(const std::string& outputDevice, const unsigned baudrate) :
	LedRs232Device(outputDevice, baudrate),
	_ledBuffer(4 + 8*3) // 4-byte header, 8 BGR values
{
	_ledBuffer[0] = 0xAA; // Hard-coded..
	_ledBuffer[1] = 0x12; // .. header
	_ledBuffer[2] = 0x00; // CRC goes here
	_ledBuffer[3] = 24; // Number of Databytes sent, always 24 (DECIMAL)
}

int LedDeviceKarate::write(const std::vector<ColorRgb> &ledValues)
{
	// Protocol has a 4-byte header (xAA|x12|??|24, with CRC at byte 2) and exactly 8 channels BGR (28 bytes total)
	if (ledValues.size() != 8)
	{
		std::cerr << "KarateLight: " << ledValues.size() << " channels configured. This should always be 8!" << std::endl;
		return 0;
	}

	memcpy(4 + _ledBuffer.data(), ledValues.data(), ledValues.size() * sizeof(ColorRgb));
	finalizeBuffer(_ledBuffer);
	return writeBytes(_ledBuffer.size(), _ledBuffer.data());
}

int LedDeviceKarate::switchOff()
{
	memset(4 + _ledBuffer.data(), 0, _ledBuffer.size() - 4);
	finalizeBuffer(_ledBuffer);
	return writeBytes(_ledBuffer.size(), _ledBuffer.data());
}

void LedDeviceKarate::finalizeBuffer(std::vector<uint8_t> &buffer)
{
	uint8_t crc = 0;
	for (size_t i = 0; i != buffer.size(); ++i)
	{
		if (i != 2) // crc itself is not part of the calculation
		{
			crc ^= buffer[i];
		}
	}
	buffer[2] = crc;
}
