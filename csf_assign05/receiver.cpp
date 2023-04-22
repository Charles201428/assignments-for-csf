#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::string;






int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  conn.connect(server_hostname, server_port);

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  Message login_message = Message(TAG_RLOGIN, username); //login message
  conn.send(login_message);
  Message response1;
  conn.receive(response1);
  if (response1.tag == TAG_ERR) {
    perror(response1.data.c_str()); 
    exit(-1);
  } 
  else if (response1.tag != TAG_OK) { //any other non-error but non-ok tag 
    perror("unexpected server response tag\n");
    exit(-1);
  }

  Message join_message = Message(TAG_JOIN, room_name); //join message
  conn.send(join_message);
  Message response2;
  conn.receive(response2);
  if (response2.tag == TAG_ERR) {
    perror(response2.data.c_str()); 
    exit(-1);
  } 
  else if (response2.tag != TAG_OK) { //any other non-error but non-ok tag 
    perror("unexpected server response tag\n");
    exit(-1);
  }



  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  bool status = true;
  while (status) {
      Message response;
      if (!conn.receive(response)) { //if no message received successfully
        if (conn.get_last_result() == Connection::EOF_OR_ERROR) { //check the last result
          exit(-1);
        } else {
          perror("message invalid format\n"); 
        }
      } else {
        if (response.tag == TAG_DELIVERY) { //if it got delivered
          int first_colon_index = response.data.find(":"); //find the first colon followed by room
          std::string room = response.data.substr(0,first_colon_index);
          if (room == room_name) { //whether the same room
            std::string message = response.data.substr(first_colon_index + 1); 
            int second_colon_index = message.find(":");
            printf("%s", (message.substr(0, second_colon_index) + ": " + message.substr(second_colon_index + 1)).c_str()); 
          } 
        } else {
          perror("unexpected server response tag\n"); //print error if it's other tags
        }
      }
   }
   return 0;
}


