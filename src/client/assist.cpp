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

std::optional<std::string> read_to_string(std::string filename) noexcept
{
    std::ifstream in{ filename, std::ios::in };
    if (in.is_open())
        return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    else
        return std::nullopt;
}

void load_all_mod(sol::state* lua, const std::filesystem::path& directory) noexcept
{
    for (auto& it : std::filesystem::directory_iterator{ directory })
    {
        auto path = it.path();
        auto modname = path.filename().string();
        modname.erase(modname.begin() + modname.find('.'), modname.end());  // abc.lua -> abc 
        modname[0] = std::toupper(modname[0]); // abc -> Abc 
        lua->require_file(modname, path.string());
        std::cout << std::format("\t Has Load Mod: {:8} in Path: {}", modname, path.string()) << std::endl;
    }
}