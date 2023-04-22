#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"


using std::string;

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  // TODO: call rio_readinitb to initialize the rio_t object
    m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
    if (m_fd < 0) {
      fprintf(stderr, "%s\n", "Could not connect to server");
      exit(1);
    }
    // TODO: call rio_readinitb to initialize the rio_t object
    rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (is_open()) {
    close();
  }
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  if (m_last_result == EOF_OR_ERROR) {
    return false;
  }
  return true;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (is_open()) {
    ::close(m_fd);
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char* converted_string;
  std::string concated = msg.tag + ':' + msg.data;
    size_t strsize = concated.length();
    char* encoded = new char[strsize + 2];
    memcpy(encoded, concated.c_str(), strsize);
    encoded[strsize] = '\n';
    encoded[strsize+1] = '\0';
    return encoded;
  char* encoded = msg.createEncoded(); // convert struct Message to char*
  size_t n = strlen(encoded);
  if (rio_writen(m_fd, encoded, n) != (ssize_t) n) { // write message to server
    m_last_result = EOF_OR_ERROR;
    delete[] encoded;
    return false;
  }
  m_last_result = SUCCESS;
  delete[] encoded;
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
}
