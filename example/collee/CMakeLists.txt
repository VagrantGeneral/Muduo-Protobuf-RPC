set(SRC_LIST userservice.cc ../user.pb.cc)
set(SRC_LIST_F friendservice.cc ../friend.pb.cc)

add_executable(provider ${SRC_LIST}) #提供者
add_executable(provider_f ${SRC_LIST_F})
target_link_libraries(provider mprpc protobuf)
target_link_libraries(provider_f mprpc protobuf)