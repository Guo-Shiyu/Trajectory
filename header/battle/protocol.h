#include "../hv/json.hpp"

using nlohmann::json;

// forward declare 
class BattleServer;

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

using LoginBuilder = PacketBuilder<false>;

using NetMsgHandler = std::function<void(BattleServer*, json&)>;
using LowerSortMap	= std::unordered_map<std::string_view, NetMsgHandler>;
using UpperSortMap	= std::unordered_map<std::string_view, LowerSortMap>;
using NetMsgResponser = UpperSortMap;

extern NetMsgResponser LoginRpcMap, ClientRpcMap;

enum class MsgFrom
{
	LoginServer, Client
};

static void response(MsgFrom where, const std::string& pack, BattleServer* server)
{
	json packet = json::parse(pack);
	switch (where)
	{
	case Protocol::MsgFrom::LoginServer:
		// dispatch 'Appendix' field to Remote-Process-Map
		Protocol::LoginRpcMap.at(packet["Type"].get<std::string>()).at(packet["SubType"].get<std::string>())(server, packet["Appendix"]);
		break;
	case Protocol::MsgFrom::Client:
		Protocol::ClientRpcMap.at(packet["Type"].get<std::string>()).at(packet["SubType"].get<std::string>())(server, packet["Appendix"]);
		break;

		// unreachable code 
	default:
		break;
	}
}

} // namespace Protocol
