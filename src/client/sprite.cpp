#include "../../header/client/sprite.h"

SINGLETON_IMPL(Scene);

iRenderAssembly::iRenderAssembly()
{
	static std::once_flag inited{};
	std::call_once(inited, [this]()
		{
			this->open_libraries()
				->regist_action();
		});
};

iRenderAssembly* iRenderAssembly::regist_action() noexcept
{
	this->vm_.create_named_table(
		"Set",
		"SetLineStyle", [](int style, int thickness = 1)
		{ setlinestyle(style, thickness); },

		"SetLineColor", setlinecolor,

		"SetFillStyle", [](int style, long hatch = 0L)
		{ setfillstyle(style, hatch); },

		"SetFillColor", setfillcolor,

		"SetTextStyle", [](int height, int width, std::string&& font)
		{ settextstyle(height, width, std::wstring(font.begin(), font.end()).c_str()); });

	this->vm_.create_named_table(
		"Act",
		"Xyout", [](int x, int y, std::string&& text)
		{ outtextxy(x, y, std::wstring(text.begin(), text.end()).c_str()); },

		"Circle", circle,
		"Line", line);

	this->vm_.create_named_table("LineStyle",
		"Solid", PS_SOLID,
		"Dash", PS_DASH,
		"Dot", PS_DOT,
		"DashDot", PS_DASHDOT);

	this->vm_.create_named_table("FillStyle",
		"Solid", BS_SOLID,
		"Null", BS_NULL,
		"Hatched", BS_HATCHED);

	this->vm_.create_named_table("Hatch",
		"Cube", HS_CROSS,
		"DiagCube", HS_DIAGCROSS);

	this->vm_.create_named_table("ThreadId",
		"C", ThreadId::C,
		"U", ThreadId::U,
		"N", ThreadId::N,
		"R", ThreadId::R);
	this->vm_.create_named_table("RenderLayer",
		"Active",	RenderLayer::Active,
		"Ui",		RenderLayer::Ui,
		"Menu",		RenderLayer::Menu,
		"Object",	RenderLayer::Object);
	return this;
}

//iRenderAssembly* iRenderAssembly::load_resource(std::string&& src, std::string&& mod) noexcept
//{
//	this->vm_.require_script(mod, src);
//	return this;
//}

Scene* Scene::travse(const std::function<void(LayerView&)>& visit) noexcept
{
	std::lock_guard gd{ this->lock_ };
	// render order 
	visit(this->objects_);
	visit(this->actives_);
	visit(this->uis_);
	visit(this->menus_);
	return this;
}

Scene* Scene::update(size_t frame, iGameInfo* game) noexcept
{
	// TODO: frenquency control
	static auto update = [game](decltype(this->actives_)& container)
	{
		for (auto it = container.begin(); it != container.end(); ++it)
		{
			it->update(game);

			// dead sprite's age = std::numberic_limit<size_t>::max();
			if (it->Age == Sprite::Forever)
				container.erase(it);
		}
	};

	return this->travse(update);
}

Scene* Scene::draw_frame() noexcept
{
	static auto cast = [](decltype(this->actives_)& container)
	{
		for (auto& it : container)
			it.cast();
	};

	return this->travse(cast);
}

Scene* Scene::new_sprite(RenderLayer layer, Sprite&& sprite) noexcept
{
	std::lock_guard gd{ this->lock_ };
	switch (layer)
	{
	case RenderLayer::Active:
		this->actives_.push_back(sprite);
		break;
	case RenderLayer::Object:
		this->objects_.push_back(sprite);
		break;
	case RenderLayer::Ui:
		this->uis_.push_back(sprite);
		break;
	case RenderLayer::Menu:
		this->menus_.push_back(sprite);
		break;
	default:
		break;
	}
	return this;
}

Scene* Scene::clear() noexcept
{
	static auto clear = [](decltype(this->uis_)& container)
	{	container.clear();	};
	return this->travse(clear);
}

Scene* Scene::clear(RenderLayer layer) noexcept
{
	std::lock_guard gd{ this->lock_ };
	switch (layer)
	{
	case RenderLayer::Active:
		this->actives_.clear();
		break;
	case RenderLayer::Object:
		this->objects_.clear();
		break;
	case RenderLayer::Ui:
		this->uis_.clear();
		break;
	case RenderLayer::Menu:
		this->menus_.clear();
		break;

	default: 
		// unreachable code 
		break;
	}
	return this;
}
