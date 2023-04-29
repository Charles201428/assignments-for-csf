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

using std::string;
string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

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

class ServerException: public std::exception {
  protected: 
    string excpmessage;
  public:
    ServerException(void) : excpmessage("") {}
    ServerException(const string &message) : excpmessage(message) {}
    const char *what(void) {return excpmessage.c_str();}
};

namespace {


string right_trim(const string &s) {
  size_t end = s.find_last_not_of("\n\r\t\f\v");
  return (end == string::npos) ? "" : s.substr(0, end + 1);
}


void chat_with_sender(Connection* new_connection, Server* server, string username) {
  Message curr_mes;
  Room* current_room = nullptr;
  while (true) {
    new_connection->receive(curr_mes);
    if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw ServerException();
    }

    if (new_connection->get_last_result() == Connection::INVALID_MSG) {
      Message error_message = {TAG_ERR,"wrong message"};
      new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw ServerException();
  }
    } else {
      if (curr_mes.tag == TAG_JOIN) {
        if (curr_mes.data.find("\n") != string::npos) {
          current_room = server->find_or_create_room(right_trim(curr_mes.data));
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
        } else {
            Message error_message = {TAG_ERR,"wrong room format"};
            new_connection->send(error_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
        }
      }
      else if (curr_mes.tag == TAG_LEAVE) {
        if (current_room == nullptr) {
          Message error_message = {TAG_ERR,"wrong"};
          new_connection->send(error_message);
          if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
            throw ServerException();
          }

        } else {
          current_room = nullptr;
          Message ok_message = {TAG_OK,""};
          new_connection->send(ok_message);
          if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
            throw ServerException();
          }
        }
      } else if (curr_mes.tag == TAG_SENDALL) {
        if (current_room == nullptr) {
            Message error_message = {TAG_ERR,"don't join a room"};
            new_connection->send(error_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
        } else {
          if (curr_mes.data.find("\n") == string::npos) {
            Message error_message = {TAG_ERR,"wrong formate"};
            new_connection->send(error_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
          } else {
            current_room->broadcast_message(username, right_trim(curr_mes.data));
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
          }
        }
      } else if (curr_mes.tag == TAG_QUIT) {
        Message ok_message = {TAG_OK,""};
        new_connection->send(ok_message);
        if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
          throw ServerException();
        }
        throw ServerException();
      } else {
          Message error_message = {TAG_ERR,"wrong tag pops up"};
          new_connection->send(error_message);
          if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
            throw ServerException();
          }
      }
    }
  }
}



void chat_with_receiver(Connection* new_connection, Server* server, User* new_user) {
  Message join_meg;
  if (!new_connection->receive(join_meg)) {
    Message error_message = {TAG_ERR,"something went wrong"};
    new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw ServerException();
  }
    throw ServerException();
  }
  if (join_meg.tag != TAG_JOIN || join_meg.data.find("\n") == string::npos) {
    Message error_message = {TAG_ERR,"wrong format"};
    new_connection->send(error_message);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw ServerException();
  }
    throw ServerException();
  }
  Room *room = server->find_or_create_room(right_trim(join_meg.data));
  //add user to room
  room->add_member(new_user);
  new_connection->send(Message(TAG_OK, ""));
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    room->remove_member(new_user);
    throw ServerException();
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



void *worker(void *arg) {
  pthread_detach(pthread_self());
  ConnInfo* conn = static_cast<ConnInfo *>(arg);
  Server* Server = conn->server;
  Connection* new_connection = conn->new_connection;
  Message login_mes;
  User* new_user = new User("");
  try {
    new_connection->receive(login_mes);
  if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
    throw ServerException();
  }

    if (new_connection->get_last_result() == Connection::INVALID_MSG) {
      Message error_message = {TAG_ERR,"wrong format"};
      new_connection->send(error_message);
      if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
        throw ServerException();
      }
    }

    if (login_mes.data.find("\n") == string::npos) {
        Message error_message = {TAG_ERR,"wrong format"};
        new_connection->send(error_message);
        if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
          throw ServerException();
        }
        throw ServerException();
    } else {
        if (login_mes.data.length() <= 1 || (right_trim(login_mes.data).find_first_not_of(alphabet) != string::npos)) {
        Message error_message = {TAG_ERR,"wrong tag"};
        new_connection->send(error_message);
        if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
          throw ServerException();
        }
        throw ServerException();
        } else{
            new_user->username = right_trim(login_mes.data);
            Message ok_message = {TAG_OK,""};
            new_connection->send(ok_message);
            if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
              throw ServerException();
            }
        }
    }

    if (login_mes.tag == TAG_SLOGIN) {
      chat_with_sender(new_connection, Server, new_user->username);
    } else if (login_mes.tag == TAG_RLOGIN) {
      chat_with_receiver(new_connection, Server, new_user);
    } else {
      Message error_message = {TAG_ERR,"wrong tag"};
      new_connection->send(error_message);
      if (new_connection->get_last_result() == Connection::EOF_OR_ERROR) {
        throw ServerException();
      }
      throw ServerException();
    }
  } catch (ServerException& expception) {
    delete new_user;
    return nullptr;
  }
  delete new_user;
  return nullptr;
}

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
  for (RoomMap::iterator it = m_rooms.begin(); it != m_rooms.end(); it++) {
    delete it->second;
  }
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

Room *Server::find_or_create_room(const string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard g(m_lock);
  if (m_rooms.find(room_name) == m_rooms.end()) {
    m_rooms[room_name] = new Room(room_name);
  }
  return m_rooms[room_name];
}
