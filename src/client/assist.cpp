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