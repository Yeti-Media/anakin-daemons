#include <AnakinFlags.hpp>
#include <DataOutput.hpp>
#include <CommandRunner.hpp>
#include <Socket.hpp>
#include <ATCPSocket.hpp>
#include <AUDPSocket.hpp>
#include <Server.hpp>
#include "RequestServer.hpp"
#include "DelimiterBasedTCPSocket.hpp"
#include "DTCPServerSocket.hpp"

#define CONSOLE 1
#define TCP     2
#define UDP     4
#define DTCP    8

using namespace Anakin;

int main(int argc, const char * argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cerr.tie(nullptr);
    char iMode=CONSOLE;
    char oMode=CONSOLE;
    unsigned short portIn = 18003;
    std::string ipOut="127.0.0.1";
    std::string portOut="18002";
    bool verbose=false;

//    const char * argv_[] = {
//        "./anakin2",
//        "-iTCP", "18003",
//        "-oTCP", "127.0.0.1", "18002"
//    };
//    int argc_ = 6;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }

    Flags* anakinInput = new Flags();
    anakinInput->setOverridingFlag("help");
    anakinInput->setNoValuesFlag("verbose");

    //INPUT
    anakinInput->setNoValuesFlag("iConsole");
    anakinInput->setOptionalFlag("iTCP");
    anakinInput->setOptionalFlag("iUDP");
    anakinInput->setOptionalFlag("iDTCP");
    anakinInput->setIncompatibility("iConsole", "iTCP");
    anakinInput->setIncompatibility("iConsole", "iUDP");
    anakinInput->setIncompatibility("iConsole", "iDTCP");
    anakinInput->setIncompatibility("iUDP", "iTCP");
    anakinInput->setIncompatibility("iUDP", "iDTCP");
    anakinInput->setIncompatibility("iTCP", "iDTCP");

    //OUTPUT
    anakinInput->setNoValuesFlag("oConsole");
    anakinInput->setOptionalFlag("oTCP");
    anakinInput->setOptionalFlag("oUDP");
    anakinInput->setOptionalFlag("oDTCP");
    anakinInput->setIncompatibility("oConsole", "oTCP");
    anakinInput->setIncompatibility("oConsole", "oUDP");
    anakinInput->setIncompatibility("oConsole", "oDTCP");
    anakinInput->setIncompatibility("oUDP", "oTCP");
    anakinInput->setIncompatibility("oUDP", "oDTCP");
    anakinInput->setIncompatibility("oTCP", "oDTCP");

    anakinInput->setMinCount(2);
    anakinInput->setVerbose(true);

    if (anakinInput->validateInput(input)) {
        if (anakinInput->flagFound("help")) {
            cout << "Anakin help\n\n";
            cout << "usage: ./anakin2 -help\n";
            cout << "usage: ./anakin2 (-iConsole|(-iTCP|iUDP <port>)) (-oConsole|(-oTCP|oUDP <ip> <port>))\n";
            cout << "-iConsole/oConsole: use console to input or output respectively\n";
            cout << "-iTCP/oTCP <ip> <port> : use a TCP connection with ip and port for input or output respectively\n";
            cout << "-iUDP/oUDP <ip> <port> : use a UDP connection with ip and port for input or output respectively\n";
            return 0;
        }
        if (anakinInput->flagFound("verbose")) {
            verbose = true;
        }

        vector<string>* values;
        //INPUT
        if (anakinInput->flagFound("iConsole")) {
            iMode = CONSOLE;
        }
        if (anakinInput->flagFound("iTCP")) {
            iMode = TCP;
            values = anakinInput->getFlagValues("iTCP");
            if (values->size() == 1) {
                portIn = stoi(values->at(0));
            } else {
                cout << "param iTCP needs only one value\n";
                return -1;
            }
        }
        if (anakinInput->flagFound("iUDP")) {
            iMode = UDP;
            values = anakinInput->getFlagValues("iUDP");
            if (values->size() == 1) {
                portIn = stoi(values->at(0));
            } else {
                cout << "param iUDP needs only one value\n";
                return -1;
            }
        }
        if (anakinInput->flagFound("iDTCP")) {
            iMode = DTCP;
            values = anakinInput->getFlagValues("iDTCP");
            if (values->size() == 1) {
                portIn = stoi(values->at(0));
            } else {
                cout << "param iDTCP needs only one value\n";
                return -1;
            }
        }

        //OUTPUT
        if (anakinInput->flagFound("oConsole")) {
            oMode = CONSOLE;
        }
        if (anakinInput->flagFound("oTCP")) {
            oMode = TCP;
            values = anakinInput->getFlagValues("oTCP");
            if (values->size() == 2) {
                ipOut = values->at(0);
                portOut = values->at(1);
            } else {
                cout << "param oTCP needs two values\n";
                return -1;
            }
        }
        if (anakinInput->flagFound("oUDP")) {
            oMode = UDP;
            values = anakinInput->getFlagValues("oUDP");
            if (values->size() == 2) {
                ipOut = values->at(0);
                portOut = values->at(1);
            } else {
                cout << "param oUDP needs two values\n";
                return -1;
            }
        }
        if (anakinInput->flagFound("oDTCP")) {
            oMode = DTCP;
            values = anakinInput->getFlagValues("oDTCP");
            if (values->size() == 2) {
                ipOut = values->at(0);
                portOut = values->at(1);
            } else {
                cout << "param oDTCP needs two values\n";
                return -1;
            }
        }
    } else {
        cout << "Input error!\n";
        return -1;
    }

    Socket* soutput;
    if (oMode & TCP) {
        soutput = new ATCPSocket(ipOut, portOut);
    } else if (oMode & UDP) {
        soutput = new AUDPSocket(ipOut, portOut);
    } else if (oMode & DTCP) {
        soutput = new DelimiterBasedTCPSocket(ipOut, portOut, "<line>", "<stop>");
    }
    if (oMode & TCP || oMode & UDP || oMode & DTCP) {
        soutput->connect();
    }

    AnakinFlags* aflags = new AnakinFlags();

    DataOutput* output;
    if (oMode & CONSOLE) {
        output = new DataOutput();
    } else {
        output = new DataOutput(soutput);
    }

    Server* server = new RequestServer(portIn, 10, 4, verbose, iMode, "<line>", "<end>");
    server->start(aflags, output);

    return 0;
}


