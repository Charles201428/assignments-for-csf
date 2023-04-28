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

bool check_validity_of_message(Message &msg) {
  //only proceed in the following case
  if ((msg.tag == TAG_DELIVERY) || (msg.tag == TAG_EMPTY) 
      || (msg.tag == TAG_ERR) || (msg.tag == TAG_JOIN)
      || (msg.tag == TAG_LEAVE) || (msg.tag == TAG_OK)
      || (msg.tag == TAG_QUIT) || (msg.tag == TAG_RLOGIN)
      || (msg.tag == TAG_SENDALL) || (msg.tag == TAG_SENDUSER)
      || (msg.tag == TAG_SLOGIN)) {
    

    //return false if newline char detected
    for (int i = 0; i < static_cast<int>(msg.data.length()); i++) {
      if (msg.data.at(i) == '\n') {
        return false;
      }
    } 
    return true;
  }

  return false;
}

void communicate_with_receiver(User *user, Connection *connection, Server *srv) {
  Message join_message;
  connection->receive(join_message);
  Room *chat_room;
  Message * current_msg;
  if (join_message.tag != TAG_JOIN) {
    Message error_msg(TAG_ERR, "invalid join message");
    connection->send(error_msg);
  } else {
    chat_room = srv->find_or_create_room(join_message.data);
    chat_room->add_member(user);
    Message login_success(TAG_OK, "joined room " + join_message.data);
    connection->send(login_success);
  }
  bool keep_running = true;
  while (keep_running) {
  // Wait for message queue to be non-empty
    current_msg = user->mqueue.dequeue();
    if ((current_msg != nullptr) && !(connection->send(*current_msg))) {
      chat_room->remove_member(user);
      keep_running = false;
    } else {
    delete current_msg;
    }
  }
}





void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

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
