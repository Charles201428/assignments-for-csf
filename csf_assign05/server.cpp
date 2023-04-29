#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////
struct ConnInfo{
  Connection* new_connection;
  Server* server;

  ConnInfo(Connection * new_connection, Server *server) {
    this->new_connection = new_connection;
    this->server = server;
  }

  ~ConnInfo(){
    delete new_connection;
  }
};
typedef struct ConnInfo ConnInfo;

class TerminiationException: public std::exception {
  protected: 
    std::string excpmessage;
  public:
    TerminiationException(void) : excpmessage("") {}
    TerminiationException(const std::string &message) : excpmessage(message) {}
    const char *what(void) {return excpmessage.c_str();}
};

namespace {

void chat_with_sender(Connection* new_connection, Server* thisServer, std::string username);

void chat_with_receiver(Connection* new_connection, Server* server, User* new_user);

std::string rtrim(const std::string &s);

void *worker(void *arg) {
  pthread_detach(pthread_self());
  ConnInfo* info = static_cast<ConnInfo *>(arg);
  Server* thisServer = info->server;
  Connection* new_connection = info->new_connection;
  Message received;
  User* new_user = new User("");

  try {
    new_connection->receive(received);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }

    if (new_connection->get_last_result() == Connection::INVALID_MSG) {
      Message error_message = {TAG_ERR,"wrong format"};
      new_connection->send(error_message);
   if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
    }

    if (received.data.find("\n") == std::string::npos) {
        Message error_message = {TAG_ERR,"wrong format"};
        new_connection->send(error_message);
        if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
          throw TerminiationException();
        }
        throw TerminiationException();
    } else {
        if (received.data.length() <= 1 || (rtrim(received.data).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890") != std::string::npos)) {
        Message error_message = {TAG_ERR,"wrong tag"};
        new_connection->send(error_message);
        if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
          throw TerminiationException();
        }
        throw TerminiationException();
        } else{
            new_user->username = rtrim(received.data);
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw TerminiationException();
            }
        }
    }

    if (received.tag == TAG_SLOGIN) {
      chat_with_sender(new_connection, thisServer, new_user->username);
    } else if (received.tag == TAG_RLOGIN) {
      chat_with_receiver(new_connection, thisServer, new_user);
    } else {
      sentErrorMessage(new_connection, "invalid login tag");
      throw TerminiationException();
    }
  } catch (TerminiationException& ex) {
    delete new_user;
    return nullptr;
  }
  delete new_user;
return nullptr;
}

}

void chat_with_sender(Connection* new_connection, Server* thisServer, std::string username) {
  Message response;
  Room* current_room = nullptr;
  while (true) {
    new_connection->receive(response);
    if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
    }

    if (new_connection->get_last_result() == Connection::INVALID_MSG) {
      Message error_message = {TAG_ERR,"wrong message"};
      new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
    } else {
      if (response.tag == TAG_JOIN) {
        if (!response.data.find("\n") == std::string::npos) {
          current_room = thisServer->find_or_create_room(rtrim(response.data));
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw TerminiationException();
            }
        } else {
            Message error_message = {TAG_ERR,"wrong room format"};
            new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
        }
      }
      else if (response.tag == TAG_LEAVE) {
        if (current_room == nullptr) {
          Message error_message = {TAG_ERR,"wrong"};
          new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }

        } else {
          current_room = nullptr;
          Message ok_message = {TAG_OK,""};
          new_connection->send(ok_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
        }
      } else if (response.tag == TAG_SENDALL) {
        if (current_room == nullptr) {
            Message error_message = {TAG_ERR,"don't join a room"};
            new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
        } else {
          if (response.data.find("\n") == std::string::npos) {
            Message error_message = {TAG_ERR,"wrong formate"};
            new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
          } else {
            current_room->broadcast_message(username, rtrim(response.data));
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
          }
        }
      } else if (response.tag == TAG_QUIT) {
        Message ok_message = {TAG_OK,""};
        new_connection->send(ok_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
        throw TerminiationException();
      } else {
          Message error_message = {TAG_ERR,"wrong tag pops up"};
          new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
      }
    }
  }
}



void chat_with_receiver(Connection* new_connection, Server* server, User* new_user) {
  Message join_message;
  if (!new_connection->receive(join_message)) {
    Message error_message = {TAG_ERR,"something went wrong"};
    new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
    throw TerminiationException();
  }
  if (join_message.tag != TAG_JOIN || join_message.data.find("\n") == std::string::npos) {
    Message error_message = {TAG_ERR,"wrong format"};
    new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw TerminiationException();
  }
    throw TerminiationException();
  }
  Room *room = server->find_or_create_room(rtrim(join_message.data));
  //add user to room
  room->add_member(new_user);
  new_connection->send(Message(TAG_OK, ""));
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    room->remove_member(new_user);
    throw TerminiationException();
  }
  while (true) {
    Message* response = new_user->mqueue.dequeue(); 
    if (response != nullptr) {
      new_connection->send(*response);
      delete response;
    }
    if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
      break;
    }
  }
  room->remove_member(new_user);
}

std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of("\n\r\t\f\v");
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  m_ssock = open_listenfd((char*) std::to_string(m_port).c_str());;
  if (m_ssock < 0) {
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  while(true) {
    int clientfd = Accept(m_ssock, nullptr, nullptr);
    ConnInfo* info = new ConnInfo(new Connection(clientfd), this);

    pthread_t thr_id;
    if (pthread_create(&thr_id, nullptr, worker, info) != 0) {
      fprintf(stderr, "%s\n","pthread creation failed");
      exit(2);
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard g(m_lock);
  if (m_rooms.find(room_name) == m_rooms.end()) {
    m_rooms[room_name] = new Room(room_name);
  }
  return m_rooms[room_name];
}
