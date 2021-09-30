#pragma once

#include "log.h"
#include <string>

#define _MACRO_USE_(stmt)	stmt	// explict use precedural macro

//MACRO_RULES // expand an arg with designed macro rule 
//#define EXPAND_ARG_WITH_RULE(macro_rule, ...) macro_rule(__VA_ARGS__)
//
//MACRO_RULES // generate a pure virtual function 
//#define GEN_PVFN(ret_t, arg_t, fn_name)	virtual ret_t fn_name(arg_t) = 0;
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_1(ret_t, arg_t, name_1) GEN_PVFN(ret_t, arg_t, name_1);
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_2(ret_t, arg_t, name_1, _2)	\
//GEN_PVFN(ret_t, arg_t, name_1)					\
//GEN_PVFNS_WITH_1(ret_t, arg_t, _2)
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_3(ret_t, arg_t, name_1, _2, _3)	\
//GEN_PVFN(ret_t, arg_t, name_1)					\
//GEN_PVFNS_WITH_2(ret_t, arg_t, _2, _3)
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_4(ret_t, arg_t, name_1, _2, _3, _4)	\
//GEN_PVFN(ret_t, arg_t, name_1)					\
//GEN_PVFNS_WITH_3(ret_t, arg_t, _2, _3, _4)
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_5(ret_t, arg_t, name_1, _2, _3, _4, _5)	\
//GEN_PVFN(ret_t, arg_t, name_1)					\
//GEN_PVFNS_WITH_4(ret_t, arg_t, _2, _3, _4, _5)
//
//MACRO_RULES
//#define GEN_PVFNS_WITH_6(ret_t, arg_t, name_1, _2, _3, _4, _5, _6)	\
//GEN_PVFN(ret_t, arg_t, name_1)					\
//GEN_PVFNS_WITH_5(ret_t, arg_t, _2, _3, _4, _5, _6)
//
//MACRO_RULES // generate a virtual function 
//#define GEN_VFN(ret_t, arg_t, fn_name)	virtual ret_t fn_name(arg_t);
//
//MACRO_RULES
//#define GEN_VFNS_WITH_1(ret_t, arg_t, name_1) GEN_VFN(ret_t, arg_t, name_1)
//
//MACRO_RULES
//#define GEN_VFNS_WITH_2(ret_t, arg_t, name_1, _2)	\
//GEN_VFN(ret_t, arg_t, name_1)					\
//GEN_VFNS_WITH_1(ret_t, arg_t, _2)
//
//MACRO_RULES
//#define GEN_VFNS_WITH_3(ret_t, arg_t, name_1, _2, _3)	\
//GEN_VFN(ret_t, arg_t, name_1)					\
//GEN_VFNS_WITH_2(ret_t, arg_t, _2, _3)
//
//MACRO_RULES
//#define GEN_VFNS_WITH_4(ret_t, arg_t, name_1, _2, _3, _4)	\
//GEN_VFN(ret_t, arg_t, name_1)					\
//GEN_VFNS_WITH_3(ret_t, arg_t, _2, _3, _4)
//
//MACRO_RULES
//#define GEN_VFNS_WITH_5(ret_t, arg_t, name_1, _2, _3, _4, _5)	\
//GEN_VFN(ret_t, arg_t, name_1)					\
//GEN_VFNS_WITH_4(ret_t, arg_t, _2, _3, _4, _5)
//
//MACRO_RULES
//#define GEN_VFNS_WITH_6(ret_t, arg_t, name_1, _2, _3, _4, _5, _6)	\
//GEN_VFN(ret_t, arg_t, name_1)					\
//GEN_VFNS_WITH_5(ret_t, arg_t, _2, _3, _4, _5, _6)
//
//// generate a part of class
//MACRO_RULES
//#define CLASS_PUB_DERIVE(base) : public base
//
//// generate a part of class
//MACRO_RULES
//#define CLASS_BLANK_DERIVE    
//
//// generate a part of class
//MACRO_RULES
//#define CLASS_START(name, derive_stmt) class name derive_stmt { public:
//
//// generate a part of class
//MACRO_RULES
//#define CLASS_END  };
//
//MACRO_RULES	// generate abstract class
//#define GEN_ABS_CLASS_WITH_FIELD_1(class_name, ret_t, arg_t, field_1) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_1(ret_t, arg_t, field_1)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_2(class_name, ret_t, arg_t, field_1, _2) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_2(ret_t, arg_t, field_1, _2)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_3(class_name, ret_t, arg_t, field_1, _2, _3) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_3(ret_t, arg_t, field_1, _2, _3)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_4(class_name, ret_t, arg_t, field_1, _2, _3, _4) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_4(ret_t, arg_t, field_1, _2, _3, _4)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_5(class_name, ret_t, arg_t, field_1, _2, _3, _4, _5) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_5(ret_t, arg_t, field_1, _2, _3, _4, _5)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_6(class_name, ret_t, arg_t, field_1, _2, _3, _4, _5, _6) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_6(ret_t, arg_t, field_1, _2, _3, _4, _5, _6)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_7(class_name, ret_t, arg_t, field_1, _2, _3, _4, _5, _6, _7) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_7(ret_t, arg_t, field_1, _2, _3, _4, _5, _6, _7)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_ABS_CLASS_WITH_FIELD_8(class_name, ret_t, arg_t, field_1, _2, _3, _4, _5, _6, _7, _8) \
//CLASS_START(class_name, CLASS_BLANK_DERIVE)			\
//GEN_PVFNS_WITH_8(ret_t, arg_t, field_1, _2, _3, _4, _5, _6, _7, _8)		\
//CLASS_END
//
//MACRO_RULES	// generate derived class
//#define GEN_DERV_CLASS_WITH_FIELD_1(bcls_name, self_name, ret_t, arg_t, field_1) \
//CLASS_START(self_name, CLASS_PUB_DERIVE(bcls_name))			\
//GEN_VFNS_WITH_1(ret_t, arg_t, field_1)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_DERV_CLASS_WITH_FIELD_2(bcls_name, self_name, ret_t, arg_t, field_1, _2) \
//CLASS_START(self_name, CLASS_PUB_DERIVE(bcls_name))			\
//GEN_VFNS_WITH_2(ret_t, arg_t, field_1, _2)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_DERV_CLASS_WITH_FIELD_3(bcls_name, self_name, ret_t, arg_t, field_1, _2, _3) \
//CLASS_START(self_name, CLASS_PUB_DERIVE(bcls_name))			\
//GEN_VFNS_WITH_3(ret_t, arg_t, field_1, _2, _3)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_DERV_CLASS_WITH_FIELD_4(bcls_name, self_name, ret_t, arg_t, field_1, _2, _3, _4) \
//CLASS_START(self_name, CLASS_PUB_DERIVE(bcls_name))			\
//GEN_VFNS_WITH_4(ret_t, arg_t, field_1, _2, _3, _4)		\
//CLASS_END
//
//MACRO_RULES
//#define GEN_DERV_CLASS_WITH_FIELD_5(bcls_name, self_name, ret_t, arg_t, field_1, _2, _3, _4, _5) \
//CLASS_START(self_name, CLASS_PUB_DERIVE(bcls_name))			\
//GEN_VFNS_WITH_5(ret_t, arg_t, field_1, _2, _3, _4, _5)		\
//CLASS_END

#define GEN_STATE(base, arg_t,  self) \
class self : public base {  \
    public:                 \
    void into(arg_t* arg);  \
    void on(arg_t* arg);    \
    void off(arg_t* arg);   \
    constexpr std::string as_str() { return  #self ;}\
    static self* instance();\
};                          \

#define GEN_STATE_2(base, arg, _1, _2) \
GEN_STATE(base, arg, _1)         \
GEN_STATE(base, arg, _2)         \

#define GEN_STATE_3(base, arg, _1, _2, _3) \
GEN_STATE(base, arg, _1)         \
GEN_STATE_2(base, arg, _2, _3)   \

#define GEN_STATE_4(base, arg, _1, _2, _3, _4) \
GEN_STATE(base, arg, _1)         \
GEN_STATE_3(base, arg, _2, _3, _4)   \

#define GEN_STATE_5(base, arg, _1, _2, _3, _4, _5) \
GEN_STATE(base, arg, _1)         \
GEN_STATE_4(base, arg, _2, _3, _4, _5)    \

#define GEN_STATE_6(base, arg, _1, _2, _3, _4, _5, _6) \
GEN_STATE(base, arg, _1)         \
GEN_STATE_5(base, arg, _2, _3, _4, _5, _6)    \

#define GEN_STATE_7(base, arg, _1, _2, _3, _4, _5, _6, _7) \
GEN_STATE(base, arg, _1)         \
GEN_STATE_6(base, arg, _2, _3, _4, _5, _6, _7)    \

template<class C>
class StateBase {
    public:
    virtual void into(C* )  = 0;
    virtual void on(C* )    = 0;
    virtual void off(C* )   = 0;
    virtual constexpr std::string as_str() { return "StateBase"; };
    virtual ~StateBase() {};
};

template<class T>
class StateMachine {
    public:
    using StatePtr = StateBase<T>*;
    private:
    T* owner_;
    StatePtr pre_;
    StatePtr cur_;
    StatePtr global_;   // a useless var

    public:
    
    template<class T>
    StateMachine(T* owner) :owner_(owner), 
                            pre_(nullptr), 
                            cur_(nullptr),
                            global_(nullptr) 
    {}

    void set_previous(const StatePtr p) noexcept { this->pre_ = p; }

    void set_current(StatePtr s) noexcept 
    { 
        this->pre_ = this->cur_;
        this->cur_ = s; 
    }
    
    void set_global(const StatePtr g) noexcept { this->global_ = g; }

    // change state to new state
    void into(const StatePtr new_state) noexcept
    {
        if(this->cur_ != nullptr) this->cur_->off(this->owner_);
        this->set_current(new_state);
        clog("change state, from: {}, into: {}",
            this->pre_ == nullptr ? "NullState" : this->pre_->as_str(),
            this->cur_->as_str());
        this->cur_->into(this->owner_);
    }

    // warp of StateBase<T>::on
    void execute() 
    {
        clog("execute state, on:{}", this->cur_->as_str());
        this->cur_->on(this->owner_);
    }

    // return previous state
    void blip() noexcept { this->into(this->pre_); }

    // here pointer p must be constructed in singleton pattern
    bool in_state(const StatePtr p) const noexcept { return this->cur_ == p; }

    StatePtr get_curent() const noexcept { return this->cur_; }

    StatePtr get_global() const noexcept { return this->global_; }

    StatePtr get_previous() const noexcept { return this->pre_; }
};