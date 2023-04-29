Charles Weng yweng13
Chengqian Fu cfu19

For Milestone 2:
Charles did part of server.cpp and did message.cpp
Chengqian did part of server.cpp and did room.cpp



Report:
The critical sections are determined by evaluating whether they access the shared data. Finding the room in the server is the critical section and creating the room is the critical section as well. The reason is that we shouldn’t allow multiple threads to change the room at the same time.  Broadcast_message  function is also a critical section, since it reads the data which means it shouldn’t be changed by another thread. For the room file, there are a few other critical sections, add_member and remove_member are both critical sections since they change the users there. Another critical part is the enqueue and dequeue functions in message_queue.cpp and they can’t be executed at the same time because they modify the share queue there. The mutexes used in the form of Guard objects will make sure the queue won’t be modified concurrently by different threads.  Sephamore is also used, like in dequeue function, we make sure the thread wait until the queue is not empty to do it. 
