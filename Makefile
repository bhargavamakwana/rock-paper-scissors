# See README.txt.
cpp:    server_client    

clean:
	rm -f test
	rm -f server_client
	rm -f server client
	rm -f protoc_middleman choice.pb.cc choice.pb.h 

protoc_middleman: choice.proto
	protoc --cpp_out=. choice.proto
	@touch protoc_middleman

server_client: server.cpp client.cpp protoc_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ server.cpp choice.pb.cc -o server `pkg-config --cflags --libs protobuf libmongocxx` 
	# c++ --std=c++11 test.cpp $(pkg-config --cflags --libs )
	c++ client.cpp choice.pb.cc -o client `pkg-config --cflags --libs protobuf`
	
	# c++ test.cpp choice.pb.cc -o test `pkg-config --cflags --libs protobuf`
# list_people_cpp: list_people.cc protoc_middleman
# 	pkg-config --cflags protobuf  # fails if protobuf is not installed
# 	c++ list_people.cc addressbook.pb.cc -o list_people_cpp `pkg-config --cflags --libs protobuf`
