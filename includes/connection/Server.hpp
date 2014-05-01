#ifndef SERVER_HPP
#define SERVER_HPP
#include "connection/ServerSocket.hpp"
#include "processing/AnakinFlags.hpp"
#include "output/DataOutput.hpp"
#include "processing/SFBMCache.hpp"
#include "db/DBDriver.hpp"
#include "connection/HTTPSocket.hpp"

namespace Anakin {

/**
 * Base class for listening to requests
 *
 * this class implement a skeleton algorithm to process messages
 * a redactec version is shown below
 *
 *
 *    startServer();
 *    std::string msg;
 *    bool run = true;
 *    bool stopReceivedInsideInput = false;
 *    do {
 *        msg = read();
 *        if (!stopMessageReceived(msg)) {
 *            std::vector<std::vector<std::string>*>* inputs = getInputs(msg, &stopReceivedInsideInput);
 *            for (uint i = 0; i < inputs->size(); i++) {
 *                execute(inputs->at(i));
 *            }
 *            if (stopReceivedInsideInput) {
 *                stopReceived();
 *                run = false;
 *            }
 *        } else {
 *            stopReceived();
 *            run = false;
 *        }
 *    } while (run);
 *    endServer();
 *    output->close();
 *    [in some modes a stop function will be called for the socket used]
 *
 */
class Server {
public:
	/**
	 * Constructor
	 * port : the port to listen
	 * verbose : if the server will output information to console or not
	 * mode : how to listen to requests (CONSOLE, TCP, UDP, DTCP, HTTP)
	 * ld (only for DTCP) : the line delimiter
	 * md (only for DTCP) : the message delimiter
	 */
	Server(unsigned const short port, bool verbose, char mode = TCP,
			std::string ld = "", std::string md = "");
	/**
	 * this will start the skeleton algorithm shown above
	 * aflags : this will check the message and validate that have the required flags and values
	 * output : this is used to output the processing results
	 */
	void start(AnakinFlags* aflags, DataOutput* output);
	static const char CONSOLE = 1;
	static const char TCP = 2;
	static const char UDP = 4;
	static const char DTCP = 8;
	static const char HTTP = 16;
	/**
	 * Because HTTPSocket can't be used as a client the
	 * socket created by the server is the same used to
	 * respond.
	 */
	HTTPSocket* getHttpSocket();
protected:
	/**
	 * will read a message from the right input (socket, console, httpsocket)
	 */
	std::string read();
	/**
	 * this is used to support several requests on the same message
	 * the only mode that supports this is DTCP, all others will send
	 * one request per message
	 */
	std::vector<std::vector<std::string>*>* getInputs(std::string rawInput,
			bool * stopReceivedInsideInput);
	/**
	 * because the input is validated as a vector of strings, the original message
	 * must be transformed from one string to a vector
	 */
	std::vector<std::string>* rawToInput(std::string rawInput);
	/**
	 * this will process an input
	 */
	virtual void execute(std::vector<std::string>* input);
	/**
	 * this will execute when receiving a stop message
	 * this is the last function to be called by stopReceived
	 */
	virtual void executeStop();
	/**
	 * will return true if rawMsg is a stop message
	 */
	virtual bool stopMessageReceived(std::string rawMsg);
	/**
	 * will return true if rawMsg is a stop message in HTTP
	 */
	virtual bool stopMessageReceivedHTTP(std::string rawMsg);
	/**
	 * this is used to execute anything needed before starting to receiving messages
	 */
	virtual void startServer();
	/**
	 * anything that needs to be done before closing all connections
	 */
	virtual void endServer();
	/**
	 * called after receiving a stop message
	 */
	void stopReceived();
	unsigned short port;
	ServerSocket* server;
	bool verbose;
	Socket* socket;
	HTTPSocket* httpSocket;
	char mode;
	AnakinFlags* aflags;
	DataOutput* output;
	SFBMCache* cache;
	DBDriver* dbdriver;

	bool initialization = false;
	std::string initializationError;
	bool cacheInitialization = false;
	std::wstring cacheInitializationError;
private:
};

}
;

#endif // SERVER_HPP
