#pragma once

#include "../hv/singleton.h"
#include "iluavm.h"
#include "ilpc.h"

#include <codecvt>
#include <set>

enum class RenderLayer : int
{
    Active, Object, Ui, Menu 
};

class iRenderAssembly
{
protected:
	static sol::state vm_;	// renderer's luavm

	iRenderAssembly();
	
	virtual ~iRenderAssembly() {};

	// regist EasyX function to lua
	virtual iRenderAssembly* regist_action() noexcept;


	// virtual iRenderAssembly* load_resource(std::string&& src, std::string&& mod) noexcept;

public:
	static sol::state& renderer() noexcept
	{
		static std::once_flag flag{};
		std::call_once(flag, [vm = &iRenderAssembly::vm_]()
		{ 
			vm->open_libraries(sol::lib::base, sol::lib::coroutine,
								 sol::lib::string, sol::lib::table); 
		});
		return iRenderAssembly::vm_;
	}
};

using MagicArg	= sol::table;
using MagicFn	= std::function<void(int, int, size_t, sol::optional<MagicArg>)>;
//                                   px   py   age     arg

template<typename DepType>
class iSprite
{
public:
	// especially, a sprite with age eaquals to 'Forever' means it will be removed from it's layer 
	constexpr static size_t Forever = std::numeric_limits<size_t>::max();

	int X, Y;
	size_t Age;
    sol::optional<MagicArg> Args;

public:
	iSprite() : X(0), Y(0), Age(0), Args(sol::nullopt) {} 
	
	// update self's field with dependency 
	virtual iSprite* update(const DepType&) noexcept = 0;
	
	// perform it's magic with self field 
	virtual iSprite* cast() noexcept = 0;
    
    // interface with lua script, create a new callable fn with type is 'MagicFn'  
    template <typename... Args>
    static MagicFn magic(std::string_view index, RenderLayer layer, Args&&... args) noexcept
    {
    	return iRenderAssembly::renderer()["Renderer"]["NewSprite"].call(index, static_cast<int>(layer), args...);
    }
};

template<typename DepType>
class SpriteBase : public iSprite<DepType>
{ 
public: 
	using Updator = std::function<void(iSprite<DepType>*, const DepType&)>;
protected:
	const size_t    born_;
	const MagicFn   magic_; 
	const Updator   updator_;

public:
	SpriteBase() = delete;
	SpriteBase(size_t borntime, const MagicFn& magic, const Updator& updator) 
	: iSprite<DepType>(), born_(borntime), magic_(magic), updator_(updator) {}

	// iSprite inteface 
	SpriteBase* update(const DepType& depend) noexcept override 
	{
		std::invoke(this->updator_, static_cast<iSprite<DepType>*>(this), depend);
		return this;
	}

	// iSprite inteface 
	SpriteBase* cast() noexcept override 
	{
		std::invoke(this->magic_, this->X, this->Y, this->Age, this->Args.value_or(sol::nil));
		return this;
	}

	// render order decided by this fn 
    bool operator <=> (const SpriteBase& other) const 
    {
		// new born will be later rendered 
        return this->born_ <=> other.born_;
    }
};

class iGameInfo;
// universal sprite type, update by 'GameInfo' 
using Sprite = SpriteBase<iGameInfo*>;


class iScene : public iRenderAssembly
{
public:
    virtual ~iScene() {}
	
	// update sprites's field 
	virtual iScene* update(size_t, iGameInfo*) noexcept = 0;

	// draw a frame 
	virtual iScene* draw_frame() noexcept = 0;
	
	// add a new sprite on sesigned layer
	virtual iScene* new_sprite(RenderLayer layer, Sprite&& sprite) noexcept = 0;
	
	// clear all layer 
	virtual iScene* clear() noexcept = 0;
	
	// clear designed layer 
	virtual iScene* clear(RenderLayer layer) noexcept = 0;
}; 


class Scene : public iScene
{
	SINGLETON_DECL(Scene)
public:
	using LayerView = std::list<Sprite>;

private:
	LayerView 	actives_;	// active layer, magical,   (x, y, frame)
	LayerView   objects_;	// object layer, stationary (x, y, _	)
	LayerView 	uis_;		// ui layer		 placed		(_, _, frame)	
	LayerView 	menus_;  	// menu layer    static     (_, _, _	)
	std::mutex  lock_;


private:
	// vistor pattern to travse all sprite, order: obj < active < ui < menu
	Scene* travse(const std::function<void(LayerView&)>& visitor) noexcept;

public:
	Scene() : objects_(), actives_(), uis_(), menus_(), lock_() {}

	Scene* update(size_t frame, iGameInfo* game) noexcept;

	Scene* draw_frame() noexcept;

	Scene* new_sprite(RenderLayer layer, Sprite&& sprite) noexcept override;

	Scene* clear() noexcept override;

	Scene* clear(RenderLayer layer) noexcept override;
}; 

using DevLogBuf = std::string;

// developer dashboard shown at edge of screen   
class DevelopLogger : public SpriteBase<DevLogBuf>
{
public:
	const char* Formatter;

private:
	DevLogBuf buf_;

public:
	DevelopLogger() = delete;
	DevelopLogger(int x, int y, const char* formatter)
		:SpriteBase(0U,

			// default logger 
			[this](int x, int y, size_t _, const sol::optional<MagicArg>& __ = sol::nullopt) -> void
			{
				settextstyle(20, 0, L"Consolas");
				settextcolor(0xFFFFFF);
				auto toshow = std::format(Formatter, buf_);
				outtextxy(x, y, (std::wstring{ toshow.begin(), toshow.end() }.c_str()));
			}, 

			// default updator, do nothing 
			[](auto _, auto __) -> void {}),
		Formatter(formatter),
		buf_()
	{
		this->X = x;
		this->Y = y;
	}

	DevelopLogger* update(const DevLogBuf& newlog) noexcept override
	{
		this->buf_ = newlog;
		return this;
	}
};
 