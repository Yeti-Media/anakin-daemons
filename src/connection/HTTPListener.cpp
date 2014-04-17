#include "connection/HTTPListener.hpp"
#include <iostream>
#include <ctime>
#include "utils/Constants.hpp"

using namespace Anakin;


HTTPListener* HTTPListener::getInstance(  std::string port,
        tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
        BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
        int threads)
{

    if (HTTPListener::instance == NULL || port != HTTPListener::port)
    {
        if (HTTPListener::instance != NULL)
        {
            mg_destroy_server(&(HTTPListener::server));
            delete HTTPListener::instance;
        }
        HTTPListener::instance = new HTTPListener(port, readingQueue, writtingQueue, threads);
    }
    srand(time(0));
    return HTTPListener::instance;
}

void HTTPListener::start()
{
    HTTPListener::running = true;
    std::cout << "Starting on port : " << mg_get_option(HTTPListener::server, "listening_port") << std::endl;
    while (HTTPListener::running)
    {
        if (HTTPListener::running) mg_poll_server(HTTPListener::server,300);
    }
    mg_destroy_server(&(HTTPListener::server));
}

//PROTECTED

HTTPListener::HTTPListener( std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
                            int threads)
{
    HTTPListener::server = mg_create_server(NULL, ev_handler);
    mg_set_option(HTTPListener::server, "listening_port", port.c_str());
    std::string sthreads = std::to_string(threads);
    mg_set_option(HTTPListener::server, "num_threads", sthreads.c_str());
    HTTPListener::readingQueue = readingQueue;
    HTTPListener::writtingQueue = writtingQueue;
    HTTPListener::running = false;
}

int HTTPListener::ev_handler(struct mg_connection *conn, enum mg_event ev)
{
    int result = MG_FALSE;

    if (ev == MG_REQUEST)
    {
        //the event is a request (POST, GET for example)
        std::string method = conn->request_method; //method: POST, GET
        std::string action = conn->uri; //the URL in a GET or the action of a POST
        std::string body(conn->content, conn->content_len); //request's body
        std::string request;
        int reqID;
        if (strcmp(body.c_str(), "stop") == 0)
        {
            //the request is a stop message
            HTTPListener::running = false; //will stop the HTTP server
            request = body;
            reqID = 0; //a random id will never have this value
        }
        else
        {
            //first we convert the body (in JSON format) to a string representing a request
            request = rw.jsonReqToString(rw.parseJSON(body.c_str()));
            reqID = generateRandomID(); //generate a random request ID
            std::string sreqID = std::to_string(reqID);
            request += " -" + Constants::PARAM_REQID + " " + sreqID; //add a -reqID id to the request
        }
        char requestType = method=="POST"?(method=="GET"?HTTPSocket::GET:HTTPSocket::RESPONSE):HTTPSocket::POST;
        int status = conn->status_code;
        HTTPSocket::MessageData* md = new HTTPSocket::MessageData(action, request, requestType, status);

        HTTPListener::readingQueue->push(md); //pushes the message received
        HTTPSocket::MessageData* rd;
        if (!HTTPListener::running)
        {
            //if the message received was a stop message
            //then we generate a response that will just return "stop received"
            std::string r_action = "";
            std::string r_body = "stop received";
            rd = new HTTPSocket::MessageData(r_action.c_str(), r_body.c_str(), HTTPSocket::RESPONSE, 200);
            HTTPListener::writtingQueue->put(reqID, rd);
        }
        //we get the response corresponding with request reqID
        //and with this a HTTP response is constructed and sent
        rd = HTTPListener::writtingQueue->get(reqID);
        mg_send_status(conn, rd->status);
        std::string cc_header = "Connection";
        std::string cc_value = "close";
        mg_send_header(conn, cc_header.c_str(), cc_value.c_str());
//        std::string cl_header = "Content-Lenght";
//        std::string cl_value = std::to_string(rd->body.size());
//        mg_send_header(conn, cl_header.c_str(), cl_value.c_str());
        std::string responseData = rd->body + "\r\n";
        mg_send_data(conn, responseData.c_str(), rd->body.size());
        std::string stopMsg = "0\r\n\r\n";
        mg_send_data(conn, stopMsg.c_str(), 0);
        result = MG_CLOSE; //MG_CLOSE will close the connection after the response is sent
    }
    else if (ev == MG_AUTH)
    {
        //for auth events we just return true
        result = MG_TRUE;
    }

    return result;
}

HTTPListener* HTTPListener::instance = NULL;
std::string HTTPListener::port = "";
bool HTTPListener::running = false;
struct mg_server * HTTPListener::server;
tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* HTTPListener::readingQueue;
BlockingMap<int, HTTPSocket::MessageData*>* HTTPListener::writtingQueue;
ResultWriter HTTPListener::rw;

//PRIVATE

int HTTPListener::generateRandomID()
{
    int r1 = rand() % 10;
    int r2 = rand() % 10;
    int r3 = rand() % 10;
    int r4 = rand() % 10;
    int r5 = rand() % 10;
    int r6 = rand() % 10;
    int randomValue = r1 + (r2*10) + (r3*100) + (r4*1000) + (r5*10000) + (r6*100000);
    //std::cout << "randomValue: " << randomValue << std::endl;
    return randomValue;
}
