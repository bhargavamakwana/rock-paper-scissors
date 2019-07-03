###################################################################################################

For the compiation and succesful executon of the code following things are to be taken cared of:


Please note that the implementation of the task is restricted to the same PC as asked. 

################

Following dependencies are needed: 

- Google Protobuf installation - Please dwnload it from [here](https://github.com/protocolbuffers/protobuf/releases/tag/v3.6.1) in the form of tar ball.
Extract it and follow the instructions from README. 
- MongoDB server installtion. - Please follow instructions from [here](https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/).
- MongDB C++ Driver installation - Please follow instructions from [here](http://mongocxx.org/mongocxx-v3/installation/).
- A header file required to genearte hash named as "picosha2.h". Which is shared along with the folder. 
- Before going further please ensure that the above dependencies are met and installed properly. 


-Now although the generated ".pb.cc" file is shared, please keep in mind that any changes in .proto file may require to compile it individually 
if the functions are to be used. 

-The make file is shared with the folder. 

###############
After the above steps, please follow following steps: 

1. make cpp
2. ./server PORT_NO
3. ./client localhost PORT_NO



Have included the comments so that one can understand the functioning. 
Although it pre-undeerstood that PORT_NO should be same please ensure to take care of that.  Please run commands 2 and 3 in seperate terminals.

Also make sure you monitor the server terminal to see the statuses. 

sleep time is introduced so that one can see the game and enjoy it. :)


