#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::string;

void send_helper(Connection& connectio){
  while(true){
    std::string input_gotten;
    std::getline(std::cin, input_gotten);
    struct Message sentmessage;
    struct Message receivedd;
    if (input_gotten[0] == '/') {
      if (input_gotten.find("join") == 1) {
        sentmessage = {TAG_JOIN, input_gotten.substr(input_gotten.find(" ") + 1, input_gotten.length() - input_gotten.find(" ") - 1)};
      } else if (input_gotten.find("leave") == 1) {
        sentmessage = {TAG_LEAVE, ""};
      } else if (input_gotten.find("quit") == 1) {
        sentmessage = {TAG_QUIT, ""};
        connectio.send(sentmessage);
        if (connectio.receive(receivedd)) {
          return;
        }
      } else {
        fprintf(stderr, "%s\n", "The command is not valid");
      }
    } else { 
      sentmessage = {TAG_SENDALL, input_gotten};
    }


    if (sentmessage.tag != TAG_QUIT) {
        connectio.send(sentmessage);
        connectio.receive(receivedd);
    }
  }
}



int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO: connect to server
  Connection newcon;
  newcon.connect(server_hostname, server_port);
  // TODO: send slogin message
  struct Message Sentmessage = {TAG_SLOGIN, username};
  struct Message receivemessage;

  if (!newcon.send(Sentmessage)) {
    fprintf(stderr, "%s\n", "the error happens");
    exit(2);
  } 
  if (!newcon.receive(receivemessage)){
    exit(2);
  }
  if (receivemessage.tag == TAG_ERR) { 
    fprintf(stderr, "%s", receivemessage.data.c_str());
    exit(2);
  } else if (receivemessage.tag != TAG_OK) {
    fprintf(stderr, "%s\n", "the wrong tag");
    exit(2);
  }
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  send_helper(newcon); // enter the loop
  return 0;

}
