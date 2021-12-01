#pragma once

#include <string>
#include <random>
#include <fstream>
#include <sstream>
#include <optional>
#include <filesystem>

#include "iluavm.h"

// cast ExMessage.vkcode to log str
std::string as_str(const unsigned char byte_vkcode) noexcept;

// return a random char in hex-digit
char rand_hexdigit()	noexcept;

std::optional<std::string> read_to_string(std::string filename) noexcept;

void load_all_mod(sol::state* lua, const std::filesystem::path& directory) noexcept;
 