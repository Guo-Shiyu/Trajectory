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
	using RawProcessor	= std::function<void(std::vector<bool>&, size_t, size_t)>;
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
			bitarea_.emplace_back(false);
	}

	bool at(size_t r, size_t c) const
	{
		return bitarea_.at(r * Col + c);
	}

	void set_bit(size_t r, size_t c, bool bit)
	{
		bitarea_[r * Col + c] = bit;
	}

#ifdef _CLIENT
	void display_by(const std::optional<Displayer>& displayer = std::nullopt, int foucusx = 0, int foucusy = 0) const;
	//{
	//	static Displayer default_displayer = [](int x, int y)
	//	{
	//		settextstyle(fontheight, fontwidth, _T("terminal"));
	//		outtextxy(x, y, rand_hexdigit());
	//	};

	//	// cache displayer for the following call
	//	static Displayer display_fn;
	//	display_fn = displayer.value_or(default_displayer);

	//	for (int y = 0; y < Row; y++)
	//		for (int x = 0; x < Col; x++)
	//			if (bitarea_.at(y * Col + (x % Col)) == true)
	//				display_fn(x * unitwidth + foucusx, y * unitheight + foucusy);
	//}
#endif // _CLIENT


	void process_by(const RawProcessor& proc)
	{
		proc(bitarea_, Row, Col);
	}

	void process_by(const Processor& proc)
	{
		proc(*this);
	}

	std::vector<bool>& rawbits()
	{
		return bitarea_;
	}

	std::string zipped_bitstream() const
	{
		std::string ret;
		ret.reserve(bitarea_.size() / 8);
		size_t counter = 0;

		// first flag  
		ret.push_back(bitarea_.front() ? '1' : '0');
		ret.push_back(' ');

		// zip area 
		bool cur = bitarea_.front();
		for (const auto& bit : bitarea_)
		{
			if (bit == cur)
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

	static std::vector<bool> extract_from_bitstream(std::string& bitstream)
	{
		auto& bits = bitstream;
		std::vector<bool> ret;
		
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
	std::vector<bool> bitarea_;
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