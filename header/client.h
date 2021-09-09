#pragma once

#include "client/interface.h"
#include "client/states.h"
#include "client/threads.h"
#include "client/character.h"
#include "client/msgdispatch.h"
#include "client/clientdef.h"

class Client : public MsgInterface, public InitInterface {
public:
    using SelfState = StateMachine<Client>;

private:
    SelfState* stm_;
    GameInfo* game_;
    Render* render_;
    NetIO* net_;
    UserIO* uio_;

public:
    static json setting_;
    static json self_;

public:
    Client() : stm_(nullptr), game_(nullptr), render_(nullptr), 
            net_(nullptr), uio_(nullptr) {}

    SelfState* get_state() const noexcept { return this->stm_; }
    GameInfo* get_game_info() const noexcept { return this->game_; }
    Render* get_render() const noexcept { return this->render_; }
    NetIO* get_netio() const noexcept { return this->net_; }
    UserIO* get_uio() const noexcept { return this->uio_; }

    // run each thread
    void shine() noexcept;

    // load and dispatch JSON settings to derived-class
    static void prepare_for_light();

    // Client::new();  singleton pattern
    static Client* i_say_there_would_be_light();

    void on_msg(Message& m) override final;
    void send_msg_to(Recvable target, Recvable self, Message& m) override final;
    
    // check net / setting / lua script is all right
    void check() override final;

    // call each stop func of member and log to screen
    void stop_client();


    void on_crash();
private:
    // call each init func of member and initialize self
    Client* init_self() noexcept override final;
};

