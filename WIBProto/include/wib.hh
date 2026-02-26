#ifndef __WIB_HH
#define __WIB_HH

#include "wib.pb.h"

#include <zmq.hpp>
#include <string>
#include <iostream>

class WIB_ZMQ {

public:

    WIB_ZMQ();
    ~WIB_ZMQ();

    void connect();

    void connect_name(const std::string &zmq_endpoint);

    void reconnect();

    void disconnect();

    template <class R, class C>
    bool send_command(const C &msg, R &repl, int timeout_ms = -1) {

        // EG: with a REQ/REP pattern, after a socket.send, the next allowed operation is a socket.recv.
    	// In case the zmq_poll returns false, the method is returned, therefore a recv operation is not performed.
    	// This will cause socket.send to raise an exception at the next method call.
    	// Unrecoverable unless the recv is successful in the next iteration or FSM is reset by reconnecting socket:
    	// https://stackoverflow.com/questions/26915347/zeromq-reset-req-rep-socket-state

        // trj -- change model to connect on each message and disconnect after we are done
	connect();

        wib::Command command;
        command.mutable_cmd()->PackFrom(msg);

        std::string cmd_str;
        command.SerializeToString(&cmd_str);

        zmq::message_t request(cmd_str.size());
        memcpy((void*)request.data(), cmd_str.c_str(), cmd_str.size());
        m_socket->send(request,zmq::send_flags::none);

        if (zmq_poll(&m_poller, 1, timeout_ms) <= 0) {
            std::cout << "poll failed for endpoint " << m_zmq_endpoint << " Reconnecting socket" << std::endl;
            //this->reconnect();
	    disconnect();
            return false;
        }

        zmq::message_t reply;
        m_socket->recv(reply,zmq::recv_flags::none);

        std::string reply_str(static_cast<char*>(reply.data()), reply.size());
        repl.ParseFromString(reply_str);

	disconnect();
	
        return true;
    }
    
protected:
    
    zmq::context_t m_context;
    zmq::socket_t * m_socket;
    zmq::pollitem_t m_poller;
    
private:
    std::string m_zmq_endpoint;
};

#endif

