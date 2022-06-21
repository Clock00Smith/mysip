#include "sip/sip-parser.h"
#include "socket/raw-socket.h"
int main(){
    RawSocket socket("0.0.0.0", 5060);
    RawSocket::RecvData rd = socket.Recv();
    std::cout << rd.fromHost << ":" << rd.fromPort << std::endl;
    auto sipMessage = SIPParser(rd.data).parse(); 
    std::cout << *sipMessage << std::endl;
}