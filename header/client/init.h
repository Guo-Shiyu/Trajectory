#pragma once

#include "../fsmdef.h"
#define GENERATE_SERV_FSM_DEF(base, derv_1, derv_2, ret_t, arg_t, fn_1, fn_2, fn_3, fn_4) \
GEN_ABS_CLASS_WITH_FIELD_4(base, ret_t, arg_t, fn_1, fn_2, fn_3, fn_4) \
GEN_DERV_CLASS_WITH_FIELD_4(base, derv_1, ret_t, arg_t, fn_1, fn_2, fn_3, fn_4) \
GEN_DERV_CLASS_WITH_FIELD_4(base, derv_2, ret_t, arg_t, fn_1, fn_2, fn_3, fn_4) 
namespace state 
{ 
	namespace client
	{
		_MACRO_USE_(GENERATE_SERV_FSM_DEF(BASE, DE1, DE2, void, char, load, k, s, l))
	}

}