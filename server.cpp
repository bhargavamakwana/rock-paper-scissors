#include <cstdint>
#include<bits/stdc++.h>
#include<stdio.h>
#include<assert.h>
#include<random>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>

//MongoDB header files 
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

//Google ProtoBuf headers
#include <google/protobuf/message.h>
#include "choice.pb.h"


//Header file for SHA256
#include "picosha2.h"


//MongoDB
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;



using namespace google::protobuf;
using namespace std;
#define ROCK 0
#define PAPER 1
#define SCISSOR 2







// Function to update the Record database 

void update_dataase(mongocxx::client& client,int freq[], float* PointOfC,float* PointOfD, int *iteration) 
{
 	//creates a document instance
	bsoncxx::builder::stream::document document{};
	//Creates collecton to Record database and tescollection 
	auto collection = client["Record"]["collection"];
	auto builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value doc_value = builder
	    << to_string(*iteration) << bsoncxx::builder::stream::open_document
	   		<<"Total Points of C"<<to_string(*PointOfC)
	    	<<"Total Points of D"<<to_string(*PointOfD)
	    	<<"ROCK"<<to_string(freq[ROCK])
	    	<<"PAPER"<<to_string(freq[PAPER])
	    	<<"SCISSOR"<<to_string(freq[SCISSOR])
	  	<< bsoncxx::builder::stream::close_document
	  	<< bsoncxx::builder::stream::finalize;
	//creates a document view  	
	bsoncxx::document::view view = doc_value.view();
	//inserts the elements of the documnet to that collection 
	collection.insert_one(doc_value.view());
	//displays entries
	auto cursor=collection.find({});
	for (auto&& doc : cursor)  cout << bsoncxx::to_json(doc) << endl;
		
}



//Delete Previous entries if any in the database. 
void DeletePrevviousEntries(mongocxx::client& client)
{
	auto collection = client["Record"]["collection"];
	collection.drop();
}


// Hash and a random number according to the Choice is generated. 
unsigned long long int generate_number_and_hash(int val,choice::Choice* choice_value)
{

	unsigned long long int temp=0;
	string hash_hex_str;
	// Runs until the choice related to the secific modulo value is obtained. 	
	while (1)
	{
		temp =rand();
		if (temp%3==val) break;
	}

	// Hash Function to generate SHA256 Hash	
	picosha2::hash256_hex_string(to_string(temp), hash_hex_str);
	
	//clear previous hash
	choice_value->clear_hash();
	//Set hash to the choice_value structure. 
	choice_value->set_hash(hash_hex_str);
	return temp;
}

// Utility function to find ceiling of r in arr[l..h] 
int findCeil(int arr[], int r, int l, int h) 
{ 
	int mid; 
	while (l < h) 
	{ 
		mid = l + ((h - l) >> 1); // Same as mid = (l+h)/2 
		(r > arr[mid]) ? (l = mid + 1) : (h = mid); 
	} 
	return (arr[l] >= r) ? l : -1; 
} 

// The main function that returns a random number from arr[] according to 
// distribution array defined by freq[]. n is size of arrays. 
int myRand(int arr[], int freq[]) 
{ 
	// Create and fill prefix array 
	int prefix[3], i; 
	prefix[0] = freq[0]; 
	for (i = 1; i < 3; ++i) 
		prefix[i] = prefix[i - 1] + freq[i]; 

	// prefix[n-1] is sum of all frequencies. Generate a random number 
	// with value from 1 to this sum 
	int r = (rand() % prefix[3 - 1]) + 1; 

	// Find index of ceiling of r in prefix arrat 
	int indexc = findCeil(prefix, r, 0, 3 - 1); 
	return arr[indexc]; 
} 

void SendData(choice::Choice& choice_value,int &clientSocketDescriptor)
{
				char sendBuffer[10000];
				string out;
				choice_value.SerializeToString(&out);
				strncpy(sendBuffer, out.c_str(), sizeof(sendBuffer));
				send(clientSocketDescriptor,sendBuffer,strlen(sendBuffer)+1,0);
}

void RecvData(choice::Choice& rchoice_value,int &clientSocketDescriptor)
{
				char recvBuffer[10000];
				int bytes=0;
				bytes=recv(clientSocketDescriptor,recvBuffer,sizeof(recvBuffer),0);
				assert (bytes>0);
				rchoice_value.ParseFromString(string(recvBuffer,0,bytes));
}

int Set_values(choice::Choice* choice_value,int arr[], int freq[],unsigned long long int* ChoiceEncrypted ) {
  cout << "Selecting Choice... "<<endl;
  int id;

  //finds Choice based on the Bias. 
  id= myRand(arr,freq);

  // The choice is given to the below function to generate hash and the number associated with it is returned.
  *ChoiceEncrypted = generate_number_and_hash(id,choice_value);
  return id;  
}

string ifTie(int &clientSocketDescriptor,choice::Choice& choice_value,choice::Choice& rchoice_value)
{
				string hashc,hashd,vhashd;
				string out;
				int bytes=0;
				unsigned long long int rndm=0,encD;
				char recvBuffer[10000],sendBuffer[10000];
				//Set status to Draw and notify player D.
				choice_value.set_status("Draw");
				SendData(choice_value,clientSocketDescriptor);
			
				sleep(1);
				
				//Generate random integer.
				rndm=rand();
				picosha2::hash256_hex_string(to_string(rndm), hashc);
				
				//Send hash to D
				choice_value.set_hash(hashc);
				SendData(choice_value,clientSocketDescriptor);
				
				//Receive hash of the random number from D.
				RecvData(rchoice_value,clientSocketDescriptor);
				hashd=rchoice_value.hash();

				//send the random number. 
				choice_value.set_choice(rndm);
				SendData(choice_value,clientSocketDescriptor);
				
				//Revceive random number from D.
				RecvData(rchoice_value,clientSocketDescriptor);
				encD=rchoice_value.choice();	
				//Verify the hash.
				picosha2::hash256_hex_string(to_string(encD), vhashd);
				assert (vhashd==hashd);
				//receive decision
				RecvData(rchoice_value,clientSocketDescriptor);
				
				if (encD>rndm) 
				{
					assert(rchoice_value.whowin()=="D");
					return "D";
				}
				else 
					{
						assert(rchoice_value.whowin()=="C");
						return "C";				
					}	
}
// Decision is taken based on the rules defined. Also the count of the winning choice is updated.
string decision(int cc, int cd,int freq[])
{

	if (cc==ROCK && cd ==ROCK) 
		{
			return "tie";
		}	

	if (cc==ROCK && cd ==PAPER) 
		{
			freq[PAPER]++;
			return "D";
		}

	if (cc==ROCK && cd ==SCISSOR) 
		{
			freq[ROCK]++;
			return "C";
		}

	if (cc==PAPER && cd ==PAPER) 
		{
			return "tie";
		}
	if (cc==PAPER && cd ==SCISSOR)
	{
		freq[SCISSOR]++;	
		return "D";	
	} 
	if (cc==PAPER && cd ==ROCK) 
		{
			freq[PAPER]++;
			return "C";
		}
	if (cc==SCISSOR && cd ==SCISSOR) 
	{
		return "tie";
	}
	if (cc==SCISSOR && cd ==PAPER)
	{
		freq[SCISSOR]++	;
		return "C";	
	} 
	if (cc==SCISSOR && cd ==ROCK)
	{
		freq[ROCK]++;
		return "D";
	} 
}


int main(int argc,char *argv[])
{
	srand (time(NULL));
	int ValueOf[] = {0, 1, 2};
	int freq[] = {1, 1, 1};
	int iteration=1;
	int bytes=0;
	string output;
	string HashOfD,VerifyHashOfD;
	float PointOfC=0,PointOfD=0;
	unsigned long long int ChoiceEncryptedC=0,ChoiceEncryptedD=0;
	choice::Choice choice_value;
	choice::Choice rchoice_value;
	const choice::Choice& temp = rchoice_value;

	//Initialise socket variables.
	int clientSocketDescriptor,socketDescriptor;

	struct sockaddr_in serverAddress,clientAddress;
	socklen_t clientLength;

	char recvBuffer[10000],sendBuffer[10000];
	bzero(&serverAddress,sizeof(serverAddress));


// Initialize mongodb server and associate client.

	mongocxx::instance instance{}; // This should be done only once.
	mongocxx::uri uri("mongodb://localhost:27017");
	mongocxx::client client(uri);

	//Deletes entries from previous run.
	DeletePrevviousEntries(client);

/*Socket address structure*/
	serverAddress.sin_family=AF_INET;
	//if (argv[1]=="localhost")
	serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
	//serverAddress.sin_addr.s_addr=inet_addr((argv[1]));
	serverAddress.sin_port=htons(atoi(argv[1]));

/*TCP socket is created, an Internet socket address structure is filled with
wildcard address & server’s well known port*/
	socketDescriptor=socket(AF_INET,SOCK_STREAM,0);

/*Bind function assigns a local protocol address to the socket*/
	bind(socketDescriptor,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

/*Listen function specifies the maximum number of connections that kernel should queue
for this socket*/
	listen(socketDescriptor,5);
	printf("%s\n","Server is running ...");

/*The server to return the next completed connection from the front of the
completed connection Queue calls it*/
	clientSocketDescriptor=accept(socketDescriptor,(struct sockaddr*)&clientAddress,&clientLength);
	assert (clientSocketDescriptor>0);// printf("error connecting wit client restart the process or check the port\n");


	while(1)
	{
		
		bzero(&sendBuffer,sizeof(sendBuffer));
		bzero(&recvBuffer,sizeof(recvBuffer));



	/*Chooses Choice from Rock Paper Scissor and returns it. Also the number associated with the choice is generated.
	 The hash of the number is automatically set to the choice_value structure. 
	*/
		int ChoiceOfC=Set_values(&choice_value,ValueOf,freq,&ChoiceEncryptedC);
	//Send ready Signal
		choice_value.set_status("Ready");
		SendData(choice_value,clientSocketDescriptor);
		
	//Receive ready Signal.	
		RecvData(rchoice_value,clientSocketDescriptor);
		if (temp.status()=="Ready") 
		{

		/*Structure choice_value is serialised and given to the string output. This variable although is of string type is not readable as is in the 
		encoded format. hash is sent to D 
		*/	
			SendData(choice_value,clientSocketDescriptor);
	
		// The sleep time is introduced to Enjoy the game. :)
			sleep(1);

		//Receive hash from Player D
			RecvData(rchoice_value,clientSocketDescriptor);
			HashOfD=temp.hash();

		//The Random number associated with the choice is set to the choice_value structure  choice variable. 
			choice_value.set_choice(ChoiceEncryptedC);	
			SendData(choice_value,clientSocketDescriptor);


			sleep(1);
			RecvData(rchoice_value,clientSocketDescriptor);
			ChoiceEncryptedD=temp.choice();

			//The Encrypted Choice obtained from D is cross checked with the hash provided earlier. This is to ensure no one cheats.
			picosha2::hash256_hex_string(to_string(ChoiceEncryptedD), VerifyHashOfD);
			assert (VerifyHashOfD==HashOfD);	

		//The Encrypted choice is broken down to the actual choice using modulo operaiton.	
			int ChoiceOfD=ChoiceEncryptedD%3;
			cout << "Choice from D: " << ChoiceOfD << endl;

		//The decision is taken based on the choice obtained from the encrypted one and the result is set to the choice_value structure.		
			choice_value.set_whowin(decision(ChoiceOfC,ChoiceOfD,freq));


		// Receive decision
			RecvData(rchoice_value,clientSocketDescriptor);
		// Send Decision
			SendData(choice_value,clientSocketDescriptor);
		//Receive confirmation
			RecvData(rchoice_value,clientSocketDescriptor);
		// Check if D agrees.	
			assert(rchoice_value.status()=="Update Database");
			cout<<rchoice_value.status()<<endl;
			
		//The decisions are compared to check both have taken the same decision. Check if C agrees.
		assert (choice_value.whowin()==rchoice_value.whowin());
		cout<<"Player who Wins this round is : "<< choice_value.whowin()<<endl;
			 
		//Points are updated respective to who wins.

			if (rchoice_value.whowin()=="C") PointOfC++;
			else if (rchoice_value.whowin()=="D") PointOfD++;
			else 
			{
				PointOfC+=0.5; PointOfD+=0.5;
			}


		// Database entires relating to the points of both the players as well as the count of Rock Paper and Scissor is updated 
			update_dataase(client,freq,&PointOfC,&PointOfD,&iteration);

		// Iteration count 
			cout<<"Iteration: "<< iteration<<endl;
			iteration++;
			cout<<"PointOfC:   "<<PointOfC<<endl;
			cout<<"PointOfD:   "<<PointOfD<<endl;

		//Check The winner 
			if (PointOfC>=100 && PointOfD<100)
			{
				cout<<"C wins the game"<<endl;
				break;	
			} 
			else if (PointOfC<100 && PointOfD>=100)
			{
				cout<<"D wins the game"<<endl;
				break;	
			}
			else if (PointOfC>=100 && PointOfD>=100)	
			{
				cout<<"Draw!! generating random integers on both sides."<<endl;
				cout<<"The winner is :"<<ifTie(clientSocketDescriptor,choice_value,rchoice_value)<<endl;
				break;	
			}

		}
	}

	return 0;
}
