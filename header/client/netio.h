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
template<bool IsFromClient>
class PacketBuilder
{
private:
	json tobuild_;
	// static json example_;
	static size_t req_num_;

public:
	PacketBuilder() = default;
	~PacketBuilder() = default;

	PacketBuilder& deal_type(std::string_view str) noexcept
	{
		this->tobuild_["Type"] = str;
		return *this;
	}

	PacketBuilder& deal_subtype(std::string_view str) noexcept
	{
		this->tobuild_["SubType"] = str;
		return *this;
	}

	PacketBuilder& deal_appendix(const json& apd) noexcept
	{
		this->tobuild_["Appendix"] = apd;
		return *this;
	}

	std::string build() noexcept
	{
		this->tobuild_["ReqCounter"] = req_num_++;
		this->tobuild_["FromTag"] = IsFromClient;
		return this->tobuild_.dump();
	}

	static PacketBuilder make() noexcept
	{
		return PacketBuilder();
	}
};

using LoginBuilder	= PacketBuilder<true>;
using BtlSevBuilder = PacketBuilder<false>;

using NetMsgHandler = std::function<void(const json&)>;
using LowerSortMap	= std::unordered_map<std::string_view, NetMsgHandler>;
using UpperSortMap	= std::unordered_map<std::string_view, LowerSortMap>;
using NetMsgResponser = UpperSortMap;

// defined in 'proccall.cpp'
extern NetMsgResponser LoginRpcMap, BattleRpcMap;

enum class MsgFrom
{
	LoginServer, BattleServer
};

static void response(MsgFrom where, const std::string& pack)
{
	json packet = json::parse(pack);
	switch (where)
	{
	case Protocol::MsgFrom::LoginServer:
		// dispatch 'Appendix' field to Remote-Process-Map
		Protocol::LoginRpcMap.at(packet["Type"].get<std::string>()).at(packet["SubType"].get<std::string>())(packet["Appendix"]);
		break;
	case Protocol::MsgFrom::BattleServer:
		break;

		// unreachable code 
	default:
		break;
	}
}
}
