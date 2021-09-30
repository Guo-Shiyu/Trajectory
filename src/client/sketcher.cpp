#include "../../header/client/sketcher.h"

Sketcher *Sketcher::load_resource(std::string &&src, std::string &&mod) noexcept
{
    this->luavm()->require_script(mod, src);
    return this;
}

template<typename... Args>
Sketcher* Sketcher::submit(std::string&& index, Args... args) noexcept  
{
    std::lock_guard g(lock_);
    *cache_[cache_->size()].create_named(index, args ...);
    return this;
}

Sketcher* Sketcher::ensure() noexcept
{
    auto result = this->luavm()->operator[]("Ensure").call();
    assert(result.valid());
    return this;
}