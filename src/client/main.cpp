

#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"


#else



#endif // CASE1

#ifdef CASE1



using God = Client;

[[nodiscard("hiahia")]]
int main()
{

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


	God::prepare_for_light();
	auto light = God::i_say_there_would_be_light();
	return light->shine();
}


#else

#include <set>
#include <iostream>
#include <iostream>
#include <graphics.h>
#include <vector>
#include <string>
#include <format>
// enum class HitMsgTag
// {
//     Mouse,
//     Key
// };

/*abstract*/ class DisplayTrait
{
public:
	virtual void display() const = 0;
};

#define FormatCtrl constexpr static const char *
/*abstract*/ class DumpTrait
{
public:
	FormatCtrl Point = "{X = {}, Y = {}}";
	FormatCtrl Line = "{Start = {}, Stop = {}}";
	FormatCtrl Circle = "{Center = {}, Radius = {}}";
	FormatCtrl Bezier = "{Order = {}, Points = {}}";

	virtual std::string dump() const = 0;
};

// /*abstract*/ class GlobalData
// {
// public: 
//     static std::unordered_map<_Point, size_t> ShapeMap;

//     virtual void regist() = 0;
// };

class _Point : public POINT, public DisplayTrait, public DumpTrait
{
public:
	friend struct std::hash<_Point>;
	constexpr static int Around = 30;
	constexpr static int MoveRate = 10;
	static std::set<_Point> AllPoints;

	_Point() = delete;
	_Point(long _x, long _y)
	{
		x = _x, y = _y;
		AllPoints.insert(*this);
	}

	void up() { this->y -= MoveRate; }
	void down() { this->y += MoveRate; }
	void left() { this->x -= MoveRate; }
	void right() { this->x += MoveRate; }
	void display() const { circle(x, y, Around); }
	std::string dump() const { return std::format(DumpTrait::Point, x, y); }
	bool operator < (const _Point& p) const
	{
		return x < p.x;
	}

	~_Point()
	{
		AllPoints.erase(*this);
	}


};

//namespace std
//{
//    template<>
//    struct std::hash<_Point>
//    {
//        size_t operator ()(const _Point& p) const noexcept
//        {
//            return (std::hash<long>()(p.x) << 2) ^ 0xfac4193d6U ^ (std::hash<long>()(p.y) >> 6);
//        }
//    };
//}

class BezierCurve : public DisplayTrait, public DumpTrait
{
private:
	std::vector<_Point> points_;

public:
	constexpr static int DefaultLength = 200;
	constexpr static int DefaultHeight = 100;

	BezierCurve() = delete;
	BezierCurve(int x, int y) : points_()
	{
		points_.reserve(16);
		this->points_.emplace_back(x - DefaultLength / 2, y);
		this->points_.emplace_back(x - DefaultLength / 4, y - DefaultHeight / 2);
		this->points_.emplace_back(x + DefaultLength / 4, y + DefaultHeight / 2);
		this->points_.emplace_back(x + DefaultLength / 2, y);
	}

	~BezierCurve() = default;

	void take(BezierCurve& other)
	{
		for (const auto& p : other.points_)
			this->points_.push_back(p);
	}

	size_t len()
	{
		return this->points_.size();
	}

	_Point front()
	{
		return this->points_.front();
	}

	_Point back()
	{
		return this->points_.back();
	}

	void display() const { polybezier(this->points_.data(), this->points_.size()); }

	std::string dump() const
	{
		std::string pre; pre.reserve(this->points_.size() * 32);
		for (const auto& p : this->points_)
			pre.append(p.dump()).push_back(',');
		return std::format(DumpTrait::Bezier, this->points_.size(), pre);
	}
};

class StraightLine : public DisplayTrait, public DumpTrait
{
public:
	_Point start_;
	_Point stop_;
	StraightLine() = delete;
	StraightLine(long x, long y) : start_(x + 50, y), stop_(x - 50, y) {}
	~StraightLine() = default;

	void display() const
	{
		line(start_.x, start_.y, stop_.x, stop_.y);
		start_.display(), stop_.display();
	}

	std::string dump() const
	{
		return std::format(DumpTrait::Line, start_.dump(), stop_.dump());
	}
};

class Circle : public DisplayTrait, DumpTrait
{
public:
	_Point  center_;
	int     rad_;

	Circle() = delete;
	Circle(long x, long y, int rad = 100) :center_(x, y), rad_(rad) {}
	~Circle() = default;

	void display() const
	{
		circle(center_.x, center_.y, rad_);
	}
	std::string dump() const
	{
		return std::format(DumpTrait::Circle, center_.dump(), rad_);
	}
};

enum class ShapeEnum { Point, Circle, Line, Bezier };
struct Shape
{
	Shape() {};
	Shape(const Shape& s)
	{
		this->kind = s.kind;
		switch (s.kind)
		{
		case  ShapeEnum::Bezier:
			this->bzr = s.bzr;
			break;
		case ShapeEnum::Circle:
			this->crl = s.crl;
			break;
		case ShapeEnum::Line:
			this->sln = s.sln;
			break;
		case ShapeEnum::Point:
			this->pnt = s.pnt;
			break;
		default:
			break;
		}
	}

	ShapeEnum kind;
	union
	{
		_Point pnt;
		Circle crl;
		BezierCurve bzr;
		StraightLine sln;
	};
	~Shape() {};
};

using ASCII = char;
class Program
{
	constexpr static int CanvasLen = 1500;
	constexpr static int CanvasWid = 900;

	constexpr static int DefaultX = CanvasLen / 2;
	constexpr static int DefaultY = CanvasWid / 2;

public:
	Program* start()
	{
		initgraph(CanvasLen, CanvasWid, SHOWCONSOLE);
		while (true)
		{
			auto exmsg = getmessage();
			if (exmsg.message == WM_LBUTTONDOWN)
			{
				_Point pot = { DefaultX, DefaultY };
				Shape sp;
				sp.pnt = pot;
				sp.kind = ShapeEnum::Point;
				this->shapes_.push_back(sp);
				if (this->cur_ == nullptr)
					cur_ = const_cast<_Point*>(&pot.AllPoints.begin().operator*());
			}
			else if (exmsg.message == WM_KEYDOWN)
			{
				ASCII key = toascii(exmsg.vkcode);
				if (key == 'e' || key == 'E')
					break;
				switch (key)
				{
					// 3-order bezier
				case 'b':
				case 'B':
				{
					BezierCurve b(DefaultX, DefaultY);
					Shape sp;
					sp.kind = ShapeEnum::Bezier;
					sp.bzr = b;
					this->shapes_.push_back(sp);
				}
				break;

				//line   
				case 'l':
				case 'L':
				{
					StraightLine l(DefaultX, DefaultY);
					Shape sp;
					sp.kind = ShapeEnum::Line;
					sp.sln = l;
					this->shapes_.push_back(sp);
				}
				break;

				case 'c':
				case 'C':
				{
					Circle c(DefaultX, DefaultY);
					Shape sp;
					sp.kind = ShapeEnum::Circle;
					sp.crl = c;
					this->shapes_.push_back(sp);
				}
				break;

				// left
				case 0x25: if (cur_ != nullptr)  cur_->left();
					break;

					// up
				case  0x26:if (cur_ != nullptr)  cur_->up();
					break;

					// right
				case 0x27:if (cur_ != nullptr)  cur_->right();
					break;

					// down
				case 0x28:if (cur_ != nullptr)  cur_->down();
					break;

				case '+':
					if (cur_ != nullptr)
					{
						static decltype(_Point::AllPoints.begin()) iter{};
						if (iter == _Point::AllPoints.end())
							iter = _Point::AllPoints.begin();
						else
							iter++;

						this->cur_ = const_cast<_Point*>(&*iter);
					}
				default:
					std::cout << "unknown operation" << std::format("{:x}", key) << std::endl;
					break;
				}
			}
			else
				continue;
		}
		return this;
	}

	Program* dump_to_file()
	{
		return this;
	}

private:
	std::vector<Shape> shapes_;
	_Point* cur_;
};

//decltype(auto) _Point::Current {nullptr};
std::set<_Point> _Point::AllPoints{};

int main()
{
	Program prog;
	prog.start()->dump_to_file();

	return 0;
}
#endif // CASE1
