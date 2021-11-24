#pragma once

#include "ithreads.h"
#include "sprite.h"

#include <thread>
#include <string_view>

class iRender : public WorkThread<std::nullptr_t, std::thread>, public iRenderAssembly
{
public:
	iScene*		Scene;
	int         Fx, Fy;		// scope foucus point 
	size_t      Frame;		// frame counter 

public:
	iRender() : WorkThread(), iRenderAssembly(), Scene(nullptr), Fx(0), Fy(0), Frame(0) {}
	
	//clear all render layer
	virtual iRender* clear_all() noexcept = 0;

	// clear designed render layer 
	virtual iRender* clear(RenderLayer layer) noexcept = 0;
	
	// refresh instant(developer) log
	virtual iRender* refresh(ThreadId id, DevLogBuf&& newlog) noexcept = 0;
	
	// recorrect foucus point 
	virtual iRender* foucus(std::optional<int> x = std::nullopt, std::optional<int> y = std::nullopt) noexcept = 0;

	// place new sprite on a layer  
	template <typename... Args>
	iRender* submit(RenderLayer layer, std::string_view index, Sprite::Updator&& updator = [](auto, auto) {}, Args&&... args) noexcept
	{
		this->Scene->new_sprite(layer, Sprite(this->Frame, Sprite::magic(index, layer, args...), updator));
		return this;
	}
};

class Render : public iRender
{
	SINGLETON_DECL(Render)
private: 
	DevelopLogger *cli_, *net_, *user_;

	void show_devlog() noexcept;

public:
	Render() : iRender(), cli_(nullptr), net_(nullptr), user_(nullptr) {}

	// init interface
	Render* lazy_init() noexcept override final;

	// condig interface
	Render* ensure() noexcept override final;

	// work thread interface
	Render* start() noexcept override final;
	Render* panic() noexcept override final;

	// iRender interface 
	Render* clear(RenderLayer layer) noexcept override;
	Render* clear_all() noexcept override; 
	Render* foucus(std::optional<int> x = std::nullopt, std::optional<int> y = std::nullopt) noexcept override;
	Render* refresh(ThreadId id, DevLogBuf&& newlog) noexcept override;
}; 