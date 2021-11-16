#include "../../header/client/assist.h"

std::string as_str(const unsigned char byte_vkcode) noexcept
{
	auto code = toascii(byte_vkcode);
	switch (code)
	{
	case ' ':	return "space";
	case '\t':	return "tab";
	default:	return { static_cast<char>(code) };
	}
}

char RandHex()	noexcept
{
	static std::string hexs = { "0123456789abcdef" };
	// static size_t buf{ 0 }, cnt{ 0 };
	static std::default_random_engine rd{};
	static std::uniform_int<size_t> form{ 0U, 15U };

	return hexs.at(form(rd));
}