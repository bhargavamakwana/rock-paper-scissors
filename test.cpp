//C program to generate random numbers according to given frequency distribution 
#include <stdio.h> 
#include <stdlib.h> 
#include<bits/stdc++.h>
#include <ctime>
#include <fstream>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>
#include<random>
#include<time.h>
#include <google/protobuf/message.h>
// #include "choice.pb.h"
using namespace google::protobuf;
using namespace std;
#define ROCK 0
#define PAPER 1
#define SCISSOR 2

#include "picosha2.h"
string tie="tie";
string Cwins="C";
string Dwins="D";

string generate_number(int val)
{

unsigned long long int temp=0;
	while (1)
	{
		temp =rand();
		if (temp%3==val) break;
	}

	string hash_hex_str;
	picosha2::hash256_hex_string(to_string(temp), hash_hex_str);
	return hash_hex_str;
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

// Driver program to test above functions 
// int main() 
// { 
// 	int arr[] = {1, 2, 3}; 
// 	int freq[] = {10, 5, 20, 100}; 
// 	int i, n = sizeof(arr) / sizeof(arr[0]); 

// 	// Use a different seed value for every run. 
// 	// srand(time(NULL)); 

// 	// Let us generate 10 random numbers accroding to 
// 	// given distribution 
// 	for (i = 0; i < 5; i++) printf("%d\n", myRand(arr, freq, n)); 
// 	string s;
	
// 	return 0; 
// } 
// choice::Choice choice_value;
// choice::Choice rchoice_value;
// // This function fills in a Person message based on user input.
// int Set_values(choice::Choice* choice_value,int arr[], int freq[]) {
//   cout << "Selecting Choice... ";
//   int id;
//   id= myRand(arr,freq);
//   choice_value->set_choice(id);
//   return id;  
// }
// cout << "Enter name: ";
  // string val="Tie";
  // choice_value->set_whowin(val);

// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
/*  GOOGLE_PROTOBUF_VERIFY_VERSION;
  // Add an address.
  srand (time(NULL));
  int ValueOf[] = {0, 1, 2};
  int freq[] = {1, 1, 1};
  int  count=10;
  while(count--){
  	string output;
  	int ftemp=Set_values(&choice_value,ValueOf,freq);
  	freq[ftemp]++;
 	choice_value.SerializeToString(&output);
    rchoice_value.ParseFromString(output);

				const choice::Choice& temp = rchoice_value;
	  			cout << "Choice: " << temp.choice() << endl;
	    		cout << "Name: " << temp.whowin() << endl;
  // Optional:  Delete all global objects allocated by libprotobuf.
  }
  cout<<"frequency of ROCK is: "<< freq[ROCK]<<endl;
  cout<<"frequency of PAPER is: "<< freq[PAPER]<<endl;
  cout<<"frequency of SCISSOR is: "<< freq[SCISSOR]<<endl;
  google::protobuf::ShutdownProtobufLibrary();
*/

int c=10;
while(c--)
{
	cout<<generate_number(2)<<endl;
	cout<<generate_number(3)<<endl;
	cout<<generate_number(5)<<endl;
}

  return 0;
}
