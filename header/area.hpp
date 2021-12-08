#pragma once 

#include <vector>
#include <functional>

#ifdef _CLIENT
#include <optional>
#include <graphics.h>
#include "../header/client/assist.h"
#endif // _CLIENT

class BattleArea
{
public:
	using BitsContainer	= std::vector<bool>;
	using RawProcessor	= std::function<void(BitsContainer&, size_t, size_t)>;
	using Processor		= std::function<void(BattleArea&)>;

#ifdef _CLIENT
	using Displayer		= std::function<void(size_t, size_t)>;
#endif // _CLIENT

public:
	constexpr static size_t fontwidth = 7, fontheight = 7;
	constexpr static size_t unitwidth = 9, unitheight = 9;

public:
	BattleArea() = delete;
	~BattleArea() = default;
	BattleArea(size_t wid, size_t hei) : bitarea_(), RWidth(wid), RHeight(hei), Col(wid / unitwidth), Row(hei / unitheight)
	{
		size_t len = Col * Row;
		bitarea_.reserve(len);
		for (size_t i = 0; i < len; i++)
			bitarea_[i] = false;
	}

	bool at(size_t r, size_t c) const
	{
		return bitarea_[r * Col + c];
	}

	void set_bit(size_t r, size_t c, bool bit)
	{
		bitarea_[r * Col + c] = bit;
	}

#ifdef _CLIENT

#endif // _CLIENT


	void process_by(const RawProcessor& proc)
	{
		proc(bitarea_, Row, Col);
	}

	void process_by(const Processor& proc)
	{
		proc(*this);
	}

	BitsContainer& rawbits()
	{
		return bitarea_;
	}

	std::string zipped_bitstream() const
	{
		std::string ret;
		ret.reserve(Col * Row / 8);
		size_t counter = 0;

		// first flag  
		ret.push_back(bitarea_[0] ? '1' : '0');
		ret.push_back(' ');

		// zip area 
		bool cur = bitarea_[0];
		for (int i = 0; i < Col * Row; i++)
		{
			if (bitarea_[i] == cur)
				counter++;
			else
			{
				ret.append(std::to_string(counter)).push_back(' ');
				counter = 1;
				cur = !cur;
			}
		}

		if (counter != 0)
			ret.append(std::to_string(counter)).push_back(' ');
		
		return ret;
	}

	static BitsContainer extract_from_bitstream(std::string& bitstream)
	{
		auto& bits = bitstream;
		BitsContainer ret;
		
		// first flag 
		bool first = std::stoull(bits) == 0 ? false : true;
		bits.erase(0, 2);

		// others 
		bool cur = first;
		while (bits.size() != 0)
		{
			auto ops = bits.find_first_of(' ');
			if (ops != std::string::npos)
			{
				auto num = std::stoull(bits);
				for (size_t i = 0; i < num; i++)
					ret.push_back(cur);
				bits.erase(0, ops + 1);
				cur = !cur;
			}
		}
		return ret;
	}

public:
	const size_t RWidth, RHeight;
	const size_t Col, Row;

private:
	BitsContainer bitarea_;
};


//BackRound back(scnWidth, scnHeight);
//BackRound::Processor testproc = [](BackRound& bits)
//{
//	for (size_t y = bits.row() / 2 + 20; y < bits.row(); y++)
//		for (size_t x = 0; x < bits.colume(); x++)
//			bits.set_bit(y, x, true);
// 
//	size_t centerx = 50, centery = 50, r = 30;
//	for (size_t y = 0; y < bits.row() / 2; y++)
//		for (size_t x = 0; x < bits.colume(); x++)
//		{
//			auto offsetbetween = [](auto a, auto b) { return std::llabs(b - a); };
//			size_t xoffset = offsetbetween(x, centerx), yoffset = offsetbetween(y, centery);
//			if (xoffset * xoffset + yoffset * yoffset < r * (r - 1))
//				bits.set_bit(x, y, true);
//		}
//};
// 
//back.process_by(testproc);