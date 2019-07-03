# Rock Paper Scissors Game

## Introduction

This application implements simple rock paper scissor game using C++ and client server based mechanism. Google Protobuf is used to pass the messages to the client and vice versa. The results are then stored back to mongoDB. 

## Requirements

* Ubuntu 16.04
* [Google Protobuf v2 or higher](https://github.com/protocolbuffers/protobuf/releases/tag/v3.6.1)
* [MongoDB Server](https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/)
* [MongoDB C++ Driver](http://mongocxx.org/mongocxx-v3/installation/) 

> **NOTE**: Please note that the implementation of the task is restricted to the same PC as asked. 

- A header file required to genearte hash named as "picosha2.h". Which is shared along with the folder. 
- Before going further please ensure that the above dependencies are met and installed properly. 


-Although the generated ".pb.cc" file is shared, please keep in mind that any changes in .proto file may require to compile it individually 
if the functions are to be used. 

## Run the Applicaiton 

```sh
1. make cpp
2. ./server PORT_NO
3. ./client localhost PORT_NO
```


* Although it pre-undeerstood that PORT_NO should be same please ensure to take care of that.  Please run commands 2 and 3 in seperate terminals.

* Also make sure you monitor the server terminal to see the status.

* Sleep time is introduced so that one can see the game and enjoy it.

