#include "../../header/battle/battle.h"
#include "../../header/battle/protocol.h"

hv::EventLoopThreadPtr Battle::EventLoop{};
size_t Battle::RoundLongingTime{0};

Battle* Battle::start(const hv::EventLoopPtr& loop) noexcept 
{
    check_before_start();
    this->loop_ = loop;
    
    int counter = 0;
    for (auto& [k, v] : this->ConnectMap)
    {
        this->Order.push_back(k);
        json apdx;
        apdx["PlaceToken"] = k;
        v->write(Protocol::LoginBuilder::make().deal_type("Notice").deal_subtype("TokenDispatch").deal_appendix(apdx).build());
    }

    this->State->into(LoopStage::instance());
    return this;
}

void Battle::check_before_start() noexcept
{ 
    if (RoundLongingTime == 0) 
        RoundLongingTime = 30;
}

Battle* Battle::next_round() noexcept
{
    if (cur_.empty())
    {
        cur_ = Order.front();
    }
    else
    {
        bool found = false;
        for (auto& str : Order)
        {
            if (found)
            {
                cur_ = str;
                break;
            }

            if (str == cur_)
                found = true;
        }
    }

    for (auto& [k, v] : ConnectMap)
    {
        json apdx;
        apdx["PlaceToken"] = cur_;
        apdx["RoundLongingTime"] = RoundLongingTime;
        v->write(Protocol::LoginBuilder::make().deal_type("Notice").deal_subtype("NextRound").deal_appendix(apdx).build());
    }

    return this;
}


