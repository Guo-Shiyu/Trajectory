#pragma once

#include <string>
#include <random>


// cast ExMessage.vkcode to log str
std::string as_str(const unsigned char byte_vkcode) noexcept;

// return a random char in hex-digit
char RandHex()	noexcept;