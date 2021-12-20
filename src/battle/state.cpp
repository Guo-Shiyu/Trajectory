#include "../../header/battle/battle.h"
#include "../../header/hv/hlog.h"

IMPL_STATE(LoopStage)
void LoopStage::into(Battle* b)
{
	// 启动定时器， 每隔一段时间进入该状态， 
	// 改变CurTOken， 到下一个， 通知客户端启动发射定时器
	// 客户端发射定时器超时则失效， 否则正常工作
	// 
	// 将收到的每一个汇报发射信息的包处理 在 protocol 中
	// 改变地形则通知所有玩家更新地形信息 

	b->Id = b->eveloop()
			->setTimeout(Battle::RoundLongingTime * 1000, [=](auto _)
		{
			b->State->into(LoopStage::instance());
		});

	b->next_round();

	std::cout << "1231231231" << std::endl;
}

void LoopStage::on(Battle* b)
{

}

void LoopStage::off(Battle* b)
{

}

IMPL_STATE(OtherStage)
void OtherStage::into(Battle* b)
{

}

void OtherStage::on(Battle* b)
{

}

void OtherStage::off(Battle* b)
{

}
