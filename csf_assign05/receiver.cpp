#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"


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

  Message logmessage = Message(TAG_RLOGIN, username); 
  conn.send(logmessage);
  Message response1;
  conn.receive(response1); 
  //check the login status
  if (response1.tag == TAG_ERR) {
    fprintf(stderr, "%s", response1.data.c_str());
    exit(-1);
  } 
  else if (response1.tag != TAG_OK) { 
    fprintf(stderr, "%s\n", "not normal login tag");
    exit(-1);
  }

  Message message_joined = Message(TAG_JOIN, room_name); 
  conn.send(message_joined);
  Message response2;
  conn.receive(response2);
  //check the join status
  if (response2.tag == TAG_ERR) {
    fprintf(stderr, "%s", response2.data.c_str());
    exit(-1);
  } 
  else if (response2.tag != TAG_OK) { 
    fprintf(stderr, "%s\n", "not normal join tag");
    exit(-1);
  }



  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  bool status = true;
  while (status) {
      Message response;
      if (!conn.receive(response)) { 
        if (conn.get_last_result() == Connection::EOF_OR_ERROR) { 
        } else {
          perror("the formate is invalid\n"); 
        }
      } else {
        if (response.tag == TAG_DELIVERY) { 
          int firstco = response.data.find(":"); 
          std::string room = response.data.substr(0,firstco);
          //see whether the message gets received normally
          if (room == room_name) {
            std::string message = response.data.substr(firstco + 1); 
            int secondco = message.find(":");
            printf("%s", (message.substr(0, secondco) + ": " + message.substr(secondco + 1)).c_str()); 
          } 
        } else {
          perror("server's response tag is unexpected\n");
        }
      }
   }
   return 0;
 }
