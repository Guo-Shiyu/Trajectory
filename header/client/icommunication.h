#pragma once

#include "iinit.h"
#include "assist.h"

#include <vector>
#include <initializer_list>
#include <any>
#include <array>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>

enum class ThreadId : size_t
{
    C = 0, // client
    R = 1, // render
    N = 2, // net io
    U = 3, // user io
};

template <typename Key, typename Value, std::size_t Size>
struct Map
{
    std::array<std::pair<Key, Value>, Size> data;

    [[nodiscard]] // should not discard return value
    constexpr Value
    at(const Key &key) const noexcept
    {
        const auto itr =
            std::find_if(std::begin(data), std::end(data),
                         [&key](const auto &v)
                         { return v.first == key; });
        if (itr != std::end(data))
            return itr->second;
        else
            return "unkown error condition";
    }
};

using namespace std::literals::string_literals;
constexpr static std::array<std::pair<ThreadId, std::string_view>, 4U> 
    kthd_map{{
        {ThreadId::C, "Client"},
        {ThreadId::R, "Render"},
        {ThreadId::N, "NetIO"},
        {ThreadId::U, "UserIO"},
    }};

constexpr static std::string_view as_str(const ThreadId id)
{
    constexpr Map<ThreadId, std::string_view, kthd_map.size()>
        map = {{kthd_map}};
    return map.at(id);
}

template <typename Arg, typename C>
concept ArgsContainer = std::is_same<C, std::vector<Arg>>::value || std::is_same<C, std::list<Arg>>::value;

template <typename Arg, typename Container>
requires ArgsContainer<Arg, Container>
class ParamPackage
{
public:
    ParamPackage() : args_() {}

    ParamPackage(const std::initializer_list<Arg> &params)
    {
        args_.reserve(params.size());
        args_.insert(args_.begin(), params.begin(), params.end());
    }

    ~ParamPackage() = default;

    // set args
    ParamPackage *with(Arg &&arg) noexcept
    {
        this->args_.push_back(arg);
        return this;
    }

    // set args
    ParamPackage *with(const std::initializer_list<Arg> &args) noexcept
    {
        for (auto &arg : args)
            this->args_.push_back(arg);
        return this;
    }

    // get args container
    Container& args_pack() noexcept
    {
        return this->args_;
    }

    // create a new param package
    template <typename... Args>
    static std::shared_ptr<ParamPackage> create(Args... args) noexcept
    {
        return std::make_shared<ParamPackage>(std::initializer_list<Arg>{args...});
    }

private:
    Container args_;
};

using ArgsPackBuilder = ParamPackage<std::any, std::vector<std::any>>;
using ArgsPack      = std::shared_ptr<ParamPackage<std::any, std::vector<std::any>>>;
using ProcIndex     = std::string_view;
using MsgHandler    = std::function<void(std::optional<ArgsPack>)>;
using CallMap       = std::unordered_map<ProcIndex, MsgHandler>;

// all member func are not thread safe
class CallMapBuilder
{
private:
    CallMap inner_;
    static std::vector<ProcIndex> index_cache_;

public:
    CallMapBuilder() : inner_()
    {
        index_cache_.clear();
    }

    ~CallMapBuilder() = default;

    CallMapBuilder *append(const ProcIndex &index, const MsgHandler &hand) noexcept
    {
        this->inner_.try_emplace(index, hand);
        return this;
    }

    CallMapBuilder *set_indexs(std::initializer_list<ProcIndex> &idxs)
    {
        if (index_cache_.size() != 0)
            throw "call map builder: index cache not empty";
        for (auto &i : idxs)
            index_cache_.push_back(i);
        return this;
    }

    CallMapBuilder *with_procs(std::initializer_list<MsgHandler> &handlers)
    {
        if (index_cache_.size() != handlers.size())
            throw "call map builder: procs not match indexs";

        auto i = this->index_cache_.begin();
        auto j = handlers.begin();
        for (size_t k = 0; k < handlers.size(); k++)
            this->append(std::move(*i), *j);

        return this;
    }

    CallMapBuilder *set_index(ProcIndex idx)
    {
        index_cache_.push_back(idx);
        return this;
    }

    CallMapBuilder *with_proc(MsgHandler&& handler)
    {
        this->append(std::move(index_cache_.back()), std::move((handler)));
        return this;
    }

    CallMap build() noexcept
    {
        return std::move(this->inner_);
    }
};

class iMsg
{
protected:
    std::unique_ptr<CallMap> fnmap_;
    iMsg() : fnmap_(nullptr) {}

public:
    // init proc-call map
    iMsg *set_proccall_map(CallMap &&map)
    {
        this->fnmap_ = std::make_unique<CallMap>(std::forward<CallMap>(map));
        return this;
    }

    // on message
    virtual void response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept;

    // notify other thread with a message
    virtual void notify(const ThreadId target, ProcIndex i, std::optional<ArgsPack> args) noexcept;

    virtual ~iMsg() {}
};

class Dispatcher : public iInit
{
private:
    static std::unordered_map<ThreadId, iMsg*> map_;

    constexpr Dispatcher() {}

    // regist thread map
    Dispatcher* lazy_init() noexcept override final;

    // #[allow(unimplemented)]
    void destory();

public:
    // dispatch msg
    void dispatch(const ThreadId target, const ThreadId sender, ProcIndex index, std::optional<ArgsPack> arg) const noexcept;

    //singleton 
    static Dispatcher* instance() noexcept;
};