#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::string;

int send_helper(string& input_gotten, Connection& connectio){
  Message sentMsg;
  Message received;
  std::stringstream ssinput(input_gotten);
  std::string sss;
  ssinput >> sss;
  if (input_gotten[0] == '/') {
    if (input_gotten.compare("/join") == 0) {
        ssinput >> sss;
        sentMsg = {TAG_JOIN, sss};
    } else if (input_gotten.compare("/leave") == 0) {
      sentMsg = {TAG_LEAVE, ""};
    } else if (input_gotten.compare("/quit") == 0) {
      sentMsg = {TAG_QUIT, "bye"};
      connectio.send(sentMsg);
      if (connectio.receive(received)) {
        return 1;
      }
    } else {
      fprintf(stderr, "%s\n", "The command is not valid");
    }
  } else { 
    sentMsg = {TAG_SENDALL, input_gotten};
  }

  if (sentMsg.tag != TAG_QUIT) {
      connectio.send(sentMsg);
      connectio.receive(received);
  }

  return 0;
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
  Message sentMsg = {TAG_SLOGIN, username};
  newcon.send(sentMsg);

  Message response;
  newcon.receive(response);
  if (response.tag == TAG_ERR) { 
    fprintf(stderr, "%s", response.data.c_str());
    exit(2);
  } else if (response.tag != TAG_OK) {
    fprintf(stderr, "%s\n", "Wrong Tag");
    exit(2);
  }
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
    while (true) {
      std::string user_input;
      std::getline(std::cin, user_input);
      if (send_helper(user_input, con) == 1) {
        break;
      }
  }
  con.close();
  return 0;

}
