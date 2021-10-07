#include "../../header/client/sketcher.h"

Sketcher *Sketcher::initvm() noexcept
{
    this->open_libraries();
    this->vm_.create_named_table("LogTable");
    this->vm_.create_named_table("TaskQue");
    return this;
}

Sketcher *Sketcher::ensure() noexcept
{
    this->luavm()->operator[]("SketcherEnsure").call();
    return this;
}

Cache *Cache::ensure() noexcept
{
    this->luavm()->operator[]("CacheEnsure").call();
    return this;
}
