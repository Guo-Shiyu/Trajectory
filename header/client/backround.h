#include <vector>
#include <functional>
#include <optional>
#include <graphics.h>

class BackRound
{
public:
	using Displayer = std::function<void(size_t, size_t)>;
	using RawProcessor = std::function<void(std::vector<bool>&, size_t, size_t)>;
	using Processor = std::function<void(BackRound&)>;
	constexpr static size_t fontwidth = 7, fontheight = 7;
	constexpr static size_t unitwidth = 9, unitheight = 9;


	BackRound() = delete;
	~BackRound() = default;
	BackRound(size_t wid, size_t hei) : bitarea_(), realw_(wid), realh_(hei), col_(wid / unitwidth), row_(hei / unitheight)
	{
		size_t len = col_ * row_;
		bitarea_.reserve(len);
		for (size_t i = 0; i < len; i++)
			bitarea_.emplace_back(false);
	}

	size_t colume() const { return col_; }
	size_t row() const { return row_; }

	bool at(size_t r, size_t c) const
	{
		return bitarea_.at(r * col_ + c);
	}

	void set_bit(size_t r, size_t c, bool bit)
	{
		bitarea_[r * col_ + c] = bit;
	}

	void display_by(const std::optional<Displayer>& displayer = std::nullopt, int foucusx = 0, int foucusy = 0) const
	{
		static Displayer default_displayer = [](auto x, auto y) 
		{	
			settextstyle(fontheight, fontwidth, _T("terminal"));
			outtextxy(x, y, RandHex()); 
		};

		// cache displayer for the following call
		static Displayer display_fn;
		display_fn = displayer.value_or(default_displayer);

		for (int y = 0; y < row_; y++)
			for (int x = 0; x < col_; x++)
				if (bitarea_.at(y * col_ + (x % col_)) == true)
					display_fn(x * unitwidth + foucusx, y * unitheight + foucusy);
	}

	void process_by(const RawProcessor& proc)
	{
		proc(bitarea_, row_, col_);
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
		ret.reserve(bitarea_.size());
		for (const auto& bit : bitarea_)
			ret.push_back(bit ? '1' : '0');
		return ret;
	}

	void extract_from_bitstream()
	{
		// todo 
	}


private:
	std::vector<bool> bitarea_;
	size_t realw_, realh_;
	size_t col_, row_;
};