

#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else

#include <iostream>
#include <random>
#include <filesystem>

#endif // CASE1

using God = Client;

#ifdef CASE1

void main() 
{
	God::prepare_for_light();
	auto light = God::i_say_there_would_be_light();
	return light->shine();
}

#else
#include "../../header/hv/TcpClient.h"
#include "../../header/hv/htime.h"

ListNode* deleteDuplicates(ListNode* head) {
    for (ListNode* cur = head; cur->next != nullptr; cur = cur->next)
    {
        ListNode* target = cur;
        while(target->next->val == cur->val)
        {
            // search target node 

        }
    }
}

using namespace hv;

int main(int argc, char* argv[]) {
    int port = 24680;

    TcpClient cli;
    int connfd = cli.createsocket(port, "172.30.38.243");
    if (connfd < 0) {
        return -20;
    }
    printf("client connect to port %d, connfd=%d ...\n", port, connfd);
    cli.onConnection = [](const SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            printf("connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
            // send(time) every 3s
            setInterval(3000, [channel](TimerID timerID) {
                if (channel->isConnected()) {
                    char str[24] = { 0 };
                    datetime_t dt = datetime_now();
                    datetime_fmt(&dt, str);
                    channel->write(str);
                }
                else {
                    killTimer(timerID);
                }
                });
        }
        else {
            printf("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
        }
    };
    cli.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
        printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
    };
    cli.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf) {
        printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
    };
    // reconnect: 1,2,4,8,10,10,10...
    ReconnectInfo reconn;
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;
    cli.setReconnect(&reconn);
    cli.start();

    while (1) Sleep(1);
    return 0;
}
#endif // CASE1
