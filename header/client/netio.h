#pragma once

#include "../hv/TcpClient.h"
#include "states.h"

#include "ithreads.h"

class iNetIO : public WorkThread<StateMachine<iNetIO>, std::nullptr_t>
{
protected:
    hv::TcpClient* conn_;

public:
    iNetIO() : WorkThread(), conn_(nullptr) {}
    hv::TcpClient* connect() noexcept 
    {
        return this->conn_;
    }

    ~iNetIO() = default;
};

class NetIO : public iNetIO
{
    SINGLETON_DECL(NetIO)

    public:
    NetIO()     = default;
    ~NetIO()    = default;
    
    // init interface
    NetIO*  lazy_init() noexcept override final;

    // condig interface
    NetIO*  ensure() noexcept override final;

    // work thread interface
    NetIO*  start() noexcept override final;
    NetIO*  panic() noexcept override final;
};

// todo: Protocal::take()
enum class ToWhere
{
    Login,
    Battlle
};

enum class ProtoType 
    {
        Hello, Request, Order  
    };

    enum class OrdSubType
    {
        Create, Join, Quit, Start
    };

    enum class ReqSubType
    {
        Room, Weapon, Area
    };

class Protocol
{
public:
    class LoginBuilder
    {
    private:
        json tobuild_;
        // static json example_;
        static size_t req_num_;

    public:
        LoginBuilder() = default;
        ~LoginBuilder() = default;

        LoginBuilder& deal_type(ProtoType type) noexcept
        {
            switch (auto& ref = this->tobuild_; type)
            {
            case ProtoType::Hello:
                ref["Type"] = "Hello";
                break;
            case ProtoType::Request:
                ref["Type"] = "Request";
                break;
            case ProtoType::Order:
                ref["Type"] = "Order";
                break;
            
            // unreachable
            // default:
            //     break;
            }
            return *this;
        }

        LoginBuilder& deal_subtype(ReqSubType type) noexcept
        {
            switch (auto& ref = this->tobuild_; type)
            {
            case ReqSubType::Room:
                ref["SubType"] = static_cast<int>(ReqSubType::Room);
                break;
            case ReqSubType::Weapon:
                ref["SubType"] = static_cast<int>(ReqSubType::Weapon);
                break;
            case ReqSubType::Area:
                ref["SubType"] = static_cast<int>(ReqSubType::Area);
                break;

            // unreachable
            // default:
            //     break;
            }
            return *this;
        }

        LoginBuilder& deal_subtype(OrdSubType type) noexcept
        {
            switch (auto& ref = this->tobuild_; type)
            {
            case OrdSubType::Create:
                ref["SubType"] = static_cast<int>(OrdSubType::Create);
                break;
            case OrdSubType::Join:
                ref["SubType"] = static_cast<int>(OrdSubType::Join);
                break;
            case OrdSubType::Quit:
                ref["SubType"] = static_cast<int>(OrdSubType::Quit);
                break;
            case OrdSubType::Start:
                ref["SubType"] = static_cast<int>(OrdSubType::Start);
                break;
            default:
                break;
            }
            return *this;
        }

        LoginBuilder& deal_appendix(json apd) noexcept
        {
            this->tobuild_["Appendix"] = apd;
            return *this;
        }

        std::string build() noexcept
        {
            this->tobuild_["ReqCounter"] = req_num_++;
            return this->tobuild_.dump();
        }

        static LoginBuilder make() noexcept 
        {
            return LoginBuilder();
        }

    };

    // template<typename T> requires( std::is_same_v<T, ReqSubType> || std::is_same_v<T, OrdSubType>)
    // static std::string login_take(ProtoType pt, T sub, std::optional<json> appendix) noexcept
    // {
    //     LoginBuilder builder{};
    //     if ( builder.deal_type(pt).deal_subtype(sub), appendix.has_value())
    //         builder.deal_appendix(std::forward<json>(appendix.value()));
    //     return builder.build();
    // }

public:
    // static std::string take(ProtoType ty) noexcept;
    // static std::string take(ProtoType ty, OrdSubtype sty) noexcept;
    // static std::string take(ProtoType ty, ReqSubType sty) noexcept;

    // template<typename T> requires(std::is_same_v<T, ReqSubType> || std::is_same_v<T, OrdSubType>)
    // static std::string take(ToWhere where, ProtoType pt, T sub, std::optional<json> appendix = std::nullopt) noexcept
    // {
    //     switch (where)
    //     {
    //     case ToWhere::Login:
    //         return login_take(pt, sub, appendix);
        
    //     //case ToWhere::Battlle:
            
    //     default:
    //         std::terminate();
    //         return "termiante";
    //     }
    //     // unreachable
    // }

};

/*
    template<class T> requires (std::is_same_v<T, OrdSubType> || std::is_same_v<T, ReqSubType>)
    static std::string login_take(ProtoType type, std::optional<T> subty = std::nullopt, std::optional<json> appendix = std::nullopt) noexcept
    {
        auto& ret{ Protocol::instance()["Login"] };
        ret["ReqCounter"] = static_cast<size_t>(req_counter++);
        if (appendix.has_value()) ret["Appendix"] = appendix.value();

        switch (type)
        {
        case ProtoType::Request:
            ret["Type"] = "request";
            switch (static_cast<ReqSubType>(subty.value()))
            {
            case ReqSubType::Area:
                ret["SubType"] = static_cast<int>(ReqSubType::Area);
                break;

            case ReqSubType::Room:
                ret["SubType"] = static_cast<int>(ReqSubType::Room);
                break;

            case ReqSubType::Weapon:
                ret["SubType"] = static_cast<int>(ReqSubType::Weapon);
                break;

                // unreachable
                // default: break; 
            }
            break;

        case ProtoType::Order:
            ret["Type"] = "order";
            switch (static_cast<OrdSubType>(subty.value()))
            {
            case OrdSubType::Create:
                ret["SubType"] = static_cast<int>(OrdSubType::Create);
                break;

            case OrdSubType::Join:
                ret["SubType"] = static_cast<int>(OrdSubType::Join);
                break;

            case OrdSubType::Quit:
                ret["SubType"] = static_cast<int>(OrdSubType::Quit);
                break;

            case OrdSubType::Start:
                ret["SubType"] = static_cast<int>(OrdSubType::Start);
                break;

                // unreachable
                // default: break;
            }
            break;

        case ProtoType::Hello:
            ret["Type"] = "hello";
            break;

            // unreachable
            // default: break;
        }
        return ret.dump();
    }
*/