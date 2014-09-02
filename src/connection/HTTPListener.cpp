#include <connection/HTTPListener.hpp>
#include <utils/Constants.hpp>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <mongoose.h>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <limits.h>

using namespace Anakin;
using namespace std;

HTTPListener* HTTPListener::getInstance(string port,
		tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
		BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
		int threads) {

	if (HTTPListener::instance == NULL || port != HTTPListener::port) {
		if (HTTPListener::instance != NULL) {
			mg_destroy_server(&(HTTPListener::server));
			delete HTTPListener::instance;
		}
		HTTPListener::instance = new HTTPListener(port, readingQueue,
				writtingQueue, threads);
	}
	srand(time(0));
	return HTTPListener::instance;
}

void HTTPListener::start() {
	HTTPListener::running = true;
	cout << "Starting on port : "
			<< mg_get_option(HTTPListener::server, "listening_port")
			<< endl;
	while (HTTPListener::running) {
		if (HTTPListener::running)
			mg_poll_server(HTTPListener::server, 300);
	}
	mg_destroy_server(&(HTTPListener::server));
}

//PROTECTED

HTTPListener::HTTPListener(string port,
		tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
		BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
		int threads) {
	HTTPListener::server = mg_create_server(NULL, ev_handler);
	mg_set_option(HTTPListener::server, "listening_port", port.c_str());
	string sthreads = to_string(threads);
	mg_set_option(HTTPListener::server, "num_threads", sthreads.c_str());
	HTTPListener::readingQueue = readingQueue;
	HTTPListener::writtingQueue = writtingQueue;
	HTTPListener::running = false;
}

int HTTPListener::ev_handler(struct mg_connection *conn, enum mg_event ev) {
	int result = MG_FALSE;

	if (ev == MG_REQUEST) {
		//the event is a request (POST, GET for example)
		string method = conn->request_method; //method: POST, GET
		string action = conn->uri; //the URL in a GET or the action of a POST
		string body(conn->content, conn->content_len); //request's body
		string request;
		int reqID;
		if (body.find("\"action\":\"stop\"") != string::npos) {
			//the request is a stop message
			HTTPListener::running = false; //will stop the HTTP server
			request = body;
			reqID = 0; //a random id will never have this value
		} else {
			//first we convert the body (in JSON format) to a string representing a request
			I_CommunicationFormatter * cf = new CommunicationFormatterJSON();
			//FIXME Renzo how to delete the result of cf->formatRequest(body.c_str())
			request = *cf->formatRequest(body.c_str());

			//generate a request ID
			if (HTTPListener::lastID==INT_MAX) {
				HTTPListener::lastID = 0;
			}
			HTTPListener::lastID++;
			reqID = HTTPListener::lastID;
			//generation end

			string sreqID = to_string(reqID);
			request += " -" + Constants::PARAM_REQID + " " + sreqID; //add a -reqID id to the request
			delete cf;
		}
		char requestType =
				method == "POST" ?
						(method == "GET" ?
								HTTPSocket::GET : HTTPSocket::RESPONSE) :
						HTTPSocket::POST;
		int status = conn->status_code;
		HTTPSocket::MessageData* md = new HTTPSocket::MessageData(action,
				request, requestType, status);
		HTTPListener::readingQueue->push(md); //pushes the message received
		HTTPSocket::MessageData* rd;
		if (!HTTPListener::running) {
			//if the message received was a stop message
			//then we generate a response that will just return "stop received"
			string r_action = "";
			string r_body = "stop received";
			rd = new HTTPSocket::MessageData(r_action.c_str(), r_body.c_str(),
					HTTPSocket::RESPONSE, 200);
			HTTPListener::writtingQueue->put(reqID, rd);
		}
		//we get the response corresponding with request reqID
		//and with this a HTTP response is constructed and sent
		rd = HTTPListener::writtingQueue->get(reqID);
		//FIXME memory leak at rd and md??
		mg_send_status(conn, rd->status);
		string cc_header = "Connection";
		string cc_value = "close";
		mg_send_header(conn, cc_header.c_str(), cc_value.c_str());
//        string cl_header = "Content-Lenght";
//        string cl_value = to_string(rd->body.size());
//        mg_send_header(conn, cl_header.c_str(), cl_value.c_str());
		string responseData = rd->body + "\r\n";
		mg_send_data(conn, responseData.c_str(), rd->body.size());
		string stopMsg = "0\r\n\r\n";
		mg_send_data(conn, stopMsg.c_str(), 0);
		result = MG_CLOSE; //MG_CLOSE will close the connection after the response is sent
	} else if (ev == MG_AUTH) {
		//for auth events we just return true
		result = MG_TRUE;
	}

	return result;
}

HTTPListener* HTTPListener::instance = NULL;
string HTTPListener::port = "";
bool HTTPListener::running = false;
struct mg_server * HTTPListener::server;
tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* HTTPListener::readingQueue;
BlockingMap<int, HTTPSocket::MessageData*>* HTTPListener::writtingQueue;
int HTTPListener::lastID = 0;

//CommunicationFormatterJSON HTTPListener::cf;
