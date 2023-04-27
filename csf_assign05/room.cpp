#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  Guard g(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  Guard g(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  Guard g(lock);
  std::string data = room_name + ":" + sender_username + ":" + message_text;
  for (UserSet::iterator it = members.begin(); it != members.end(); ++it) {
    User* now = *it;
    now->mqueue.enqueue(new Message(TAG_DELIVERY, data););
  }
}
#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  Guard g(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  Guard g(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  Guard g(lock);
  std::string data = room_name + ":" + sender_username + ":" + message_text;
  for (UserSet::iterator it = members.begin(); it != members.end(); ++it) {
    User* now = *it;
    now->mqueue.enqueue(new Message(TAG_DELIVERY, data););
  }
}
