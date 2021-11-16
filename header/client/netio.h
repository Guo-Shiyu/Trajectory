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
	NetIO() = default;
	~NetIO() = default;

	// init interface
	NetIO* lazy_init() noexcept override final;

	// condig interface
	NetIO* ensure() noexcept override final;

	// work thread interface
	NetIO* start() noexcept override final;
	NetIO* panic() noexcept override final;
};

//// todo: Protocal::take()
//enum class ToWhere
//{
//	Login,
//	Battlle
//};


namespace Protocol
{
class LoginBuilder
{
private:
	json tobuild_;
	// static json example_;
	static size_t req_num_;

public:
	LoginBuilder() = default;
	~LoginBuilder() = default;

	LoginBuilder& deal_type(std::string_view str) noexcept
	{
		this->tobuild_["Type"] = str;
		return *this;
	}

	LoginBuilder& deal_subtype(std::string_view str) noexcept
	{
		this->tobuild_["SubType"] = str;
		return *this;
	}

	LoginBuilder& deal_appendix(const json& apd) noexcept
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

using NetMsgHandler = std::function<void(const json&)>;
using LowerSortMap = std::unordered_map<std::string_view, NetMsgHandler>;
using UpperSortMap = std::unordered_map<std::string_view, LowerSortMap>;
using NetMsgResponser = UpperSortMap;

// defined in 'proccall.cpp'
extern NetMsgResponser LoginRpcMap, BattleRpcMap;

enum class MsgFrom
{
	LoginServer, BattleServer
};

static void response(MsgFrom where, const string& pack)
{
	json packet = json::parse(pack);
	switch (where)
	{
	case Protocol::MsgFrom::LoginServer:
		Protocol::LoginRpcMap.at(packet["Type"].get<std::string>()).at(packet["SubType"].get<std::string>())(packet);
		break;
	case Protocol::MsgFrom::BattleServer:
		break;

		// unreachable code 
	default:
		break;
	}
}
}
