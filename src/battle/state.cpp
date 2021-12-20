#include "../../header/battle/battle.h"
#include "../../header/hv/hlog.h"

IMPL_STATE(LoopStage)
void LoopStage::into(Battle* b)
{
	// ������ʱ���� ÿ��һ��ʱ������״̬�� 
	// �ı�CurTOken�� ����һ���� ֪ͨ�ͻ����������䶨ʱ��
	// �ͻ��˷��䶨ʱ����ʱ��ʧЧ�� ������������
	// 
	// ���յ���ÿһ���㱨������Ϣ�İ����� �� protocol ��
	// �ı������֪ͨ������Ҹ��µ�����Ϣ 

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
