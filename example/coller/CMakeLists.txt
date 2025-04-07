set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST_F callfriendservice.cc ../friend.pb.cc)

add_executable(consumer ${SRC_LIST}) #调用者
add_executable(consumer_f ${SRC_LIST_F})
target_link_libraries(consumer mprpc protobuf )  
target_link_libraries(consumer_f mprpc protobuf )