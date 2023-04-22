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
    std::cout << "> ";
    std::cout.flush();
    std::string input_gotten;
    std::getline(std::cin, input_gotten);
    struct Message sentMsg;
    struct Message receivedd;
    if (input_gotten[0] == '/') {
      if (input_gotten.find("join") == 1) {
          sentMsg = {TAG_JOIN, input_gotten.substr(input_gotten.find(" ") + 1, input_gotten.length() - input_gotten.find(" ") - 1)};
      } else if (input_gotten.find("leave") == 1) {
        sentMsg = {TAG_LEAVE, ""};
      } else if (input_gotten.find("quit") == 1) {
        sentMsg = {TAG_QUIT, ""};
        connectio.send(sentMsg);
        if (connectio.receive(receivedd)) {
          return;
        }
      } else {
        fprintf(stderr, "%s\n", "The command is not valid");
      }
    } else { 
      sentMsg = {TAG_SENDALL, input_gotten};
    }

    if (sentMsg.tag != TAG_QUIT) {
        connectio.send(sentMsg);
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
  struct Message sentMsg = {TAG_SLOGIN, username};
  struct Message receivedMsg;

  if (!newcon.send(sentMsg)) {
    exit(2);
  } 
  if (!newcon.receive(receivedMsg)){
    exit(2);
  }
  if (receivedMsg.tag == TAG_ERR) { 
    fprintf(stderr, "%s", receivedMsg.data.c_str());
    exit(2);
  } else if (receivedMsg.tag != TAG_OK) {
    fprintf(stderr, "%s\n", "the wrong tag");
    exit(2);
  }
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  send_helper(newcon) // enter the loop
  return 0;

}
