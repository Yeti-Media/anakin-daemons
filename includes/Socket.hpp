#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sstream>

namespace Anakin {

class Socket {
    public:

        void setShowComs(bool v) {
            showComs = v;
        }

        bool isConnected() {
            return connected;
        }

        virtual bool connect() {
            return true;
        }

        virtual bool waitForConnection() {
            return true;
        }

        virtual bool disconnect() {
            return true;
        }

        virtual bool waitForDisconnection() {
            return true;
        }

        virtual void send(std::string msg) {
            std::stringstream ss2;
            ss2 << msg.length();
            std::string slength = ss2.str();
            slength = std::string( 6 - slength.length(), '0').append( slength);
            bsend(slength);
            if (this->sendType==DEFAULTSEND) {
                bsend(msg);
            } else if (this->sendType==SYNCSEND) {
                ssend(msg);
            }
        }

        virtual std::string read() {
            std::string slength = bread(6);
            int length = stoi(slength);
            std::string msg;
            if (this->sendType==DEFAULTSEND) {
                msg = bread(length);
            } else if (this->sendType==SYNCSEND) {
                msg = sread(length);
            }
            return msg;
        }

        virtual void sendStop() {
            send("stop");
        }

        const static char DEFAULTSEND = 0;
        const static char SYNCSEND = 1;
    protected:
        bool showComs=false;
        bool connected=false;
        char sendType;
        bool constructedWithSocket = false;
        virtual void bsend(std::string msg) = 0;
        virtual void ssend(std::string msg) = 0;
        virtual std::string bread(int lenght) = 0;
        virtual std::string sread(int lenght) = 0;
    private:
};

};

#endif // SOCKET_HPP
