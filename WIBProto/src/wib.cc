#include "wib.hh"

WIB_ZMQ::WIB_ZMQ() 
    : m_context(1)
    , m_socket(NULL)
{

}

WIB_ZMQ::~WIB_ZMQ() 
{

}

void WIB_ZMQ::connect_name(const std::string &zmq_endpoint)
{
    m_zmq_endpoint = zmq_endpoint;
}

void WIB_ZMQ::connect()
{
    m_socket = new zmq::socket_t(m_context, ZMQ_REQ);
    m_socket->connect(m_zmq_endpoint);
    m_poller.socket = *m_socket;
    m_poller.events = ZMQ_POLLIN;
}

void WIB_ZMQ::reconnect()
{
    m_socket->close();
    m_socket = new zmq::socket_t(m_context, ZMQ_REQ);
    m_socket->connect(m_zmq_endpoint);
    m_poller.socket = *m_socket;
    m_poller.events = ZMQ_POLLIN;
}

void WIB_ZMQ::disconnect()
{
    m_socket->close();
}
