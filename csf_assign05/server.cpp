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
namespace {

bool is_valid_tag(const Message &msg) {
  return (msg.tag == TAG_DELIVERY) || (msg.tag == TAG_EMPTY) 
      || (msg.tag == TAG_ERR) || (msg.tag == TAG_JOIN)
      || (msg.tag == TAG_LEAVE) || (msg.tag == TAG_OK)
      || (msg.tag == TAG_QUIT) || (msg.tag == TAG_RLOGIN)
      || (msg.tag == TAG_SENDALL) || (msg.tag == TAG_SENDUSER)
      || (msg.tag == TAG_SLOGIN);
}

bool has_no_newline(const std::string &data) {
  for (int i = 0; i < static_cast<int>(data.length()); i++) {
    if (data.at(i) == '\n') {
      return false;
    }
  }
  return true;
}

bool check_validity_of_message(Message &msg) {
  if (is_valid_tag(msg) && has_no_newline(msg.data)) {
    return true;
  }
  return false;
}

void handle_join_receiver(User *user, Connection *connection, Room *&chat_room, Server *srv, Message &join_message) {
  if (join_message.tag != TAG_JOIN) {
    Message error_msg(TAG_ERR, "invalid join message");
    connection->send(error_msg);
  } else {
    chat_room = srv->find_or_create_room(join_message.data);
    chat_room->add_member(user);
    Message login_success(TAG_OK, "joined room " + join_message.data);
    connection->send(login_success);
  }
}

void process_receiver_messages(User *user, Connection *connection, Room *chat_room) {
  Message *current_msg;
  bool keep_running = true;

  while (keep_running) {
    current_msg = user->mqueue.dequeue();
    if ((current_msg != nullptr) && !(connection->send(*current_msg))) {
      chat_room->remove_member(user);
      keep_running = false;
    } else {
      delete current_msg;
    }
  }
}

void chat_with_receiver(User *user, Connection *connection, Server *srv) {
  Message join_message;
  connection->receive(join_message);
  Room *chat_room = nullptr;

  handle_join_receiver(user, connection, chat_room, srv, join_message);
  process_receiver_messages(user, connection, chat_room);
}




void handle_join(Connection *connection, Message &current_message, Room *&chat_room, Server *srv) {
  chat_room = srv->find_or_create_room(current_message.data);
  Message login_success(TAG_OK, "joined room");
  connection->send(login_success);
}

void handle_leave(Connection *connection, Room *&chat_room) {
  if (chat_room == nullptr) {
    Message error_msg(TAG_ERR, "not in a room yet");
    connection->send(error_msg);
  } else {
    Message exit_room(TAG_OK, "left room");
    connection->send(exit_room);
    chat_room = nullptr;
  }
}

void handle_quit(Connection *connection) {
  Message quit_msg(TAG_OK, "quitting");
  connection->send(quit_msg);
}

void handle_sendall(Connection *connection, Room *chat_room, User *user, Message &current_message) {
  if (chat_room == nullptr) {
    Message error_msg(TAG_ERR, "not in a room yet");
    connection->send(error_msg);
  } else {
    chat_room->broadcast_message(user->username, current_message.data);
    Message sent_msg(TAG_OK, "msg sent");
    connection->send(sent_msg);
  }
}

void chat_with_sender(User *user, Connection *connection, Server *srv) {
  Room *chat_room = nullptr;
  Message current_message;
  bool keep_running = true;

  while (keep_running) {
    if (!connection->receive(current_message)) {
      Message error_msg(TAG_ERR, "not received");
      connection->send(error_msg);
    }

    if (!check_validity_of_message(current_message)) {
      Message error_msg(TAG_ERR, "invalid");
      connection->send(error_msg);
    }
    else if (current_message.tag == TAG_JOIN) {
      handle_join(connection, current_message, chat_room, srv);
    }
    else if (current_message.tag == TAG_LEAVE) {
      handle_leave(connection, chat_room);
    }
    else if (current_message.tag == TAG_QUIT) {
      handle_quit(connection);
      keep_running = false;
    }
    else if (current_message.tag == TAG_SENDALL) {
      handle_sendall(connection, chat_room, user, current_message);
    }
    else {
      Message error_msg(TAG_ERR, "unidentified input");
      connection->send(error_msg);
    }
  }
}





void *worker(void *args) {
  pthread_detach(pthread_self());

  // Extract argument data
  ConnInfo *input = static_cast<ConnInfo *>(args);
  Connection *connection = input->new_connection;
  Server *srv = input->server;
  delete input;

  Message login_message;
  connection->receive(login_message);
  User participant(login_message.data);
  if (login_message.tag == TAG_RLOGIN) {
    participant.receiver = true;
  } else if (login_message.tag == TAG_SLOGIN) {
    participant.receiver = false;
  } else {
    Message error_msg(TAG_ERR, "incorrect login message");
    connection->send(error_msg);
    delete connection;
    return nullptr;
  }
  Message login_success(TAG_OK, "successfully logged in");
  connection->send(login_success);

  if (participant.receiver) {
    chat_with_receiver(&participant, connection, srv); 
  } else {
    chat_with_sender(&participant, connection, srv);
  }

  delete connection;
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
