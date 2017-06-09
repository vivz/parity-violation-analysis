//script that takes the ASCII formatted output data from the muplus experiment and
//places the data into a ROOT tree
//usage is (from the ROOT command line): 
//root[1] .L mp2root.C+
//root[2] mp2root("infile.txt outfile.root")

//This script outputs a root file of a tree with three branches: 
//eventNum, UNIXtime, tdc[Up_TDC, Down_TDC]
//hte up and down TDC values are both in ns. 
//If the event is up, Down_TDC would be -10; if it is down, Up_TDC would be -10

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <sstream>


#include "TFile.h"
#include "TTree.h"

//#include "TString.h"
//#include "TROOT.h"

using namespace std;	//this allowes for use of cout, etc

void mp2root(char* arg){
	
	string infileName, outfileName;
	
	std::istringstream stm(arg);
	
	//break up the arg into the file names
	if(std::getline(stm, infileName, ' ') && std::getline(stm, outfileName, ' '));
	else{};
	
	cout<< "infile " << infileName<< " outfile " << outfileName << endl;
	
	//open the raw data file (if it is formatted correctly)
	ifstream fin;
	
	//check that the input file 
  	fin.open(infileName.c_str()); 
  
	//if the file can not be opened, fail and tell standard error
	assert(!fin.fail());
	
	//check that the output data file name ends in '.root'
	TFile *outfile;
  	outfile = TFile::Open( outfileName.c_str(), "RECREATE" );	//make the file
  
  
	//variables 
	int slot=0;					//the TDC number, 6=up, 7=down
	int loopNum=0;			//the loop cycle that took the data
	int eventNum=0;			//the event number for this run
	int data=0;					//the data for this event
	int UNIXtime=0;			//the UNIX time
	float tdc[2]={-10,-10};	//the tdc values (in ns)
	
  	//Create a ROOT Tree (data structure)
  	TTree *T = new TTree("T","TTree of muplus data");
  	//add 'branches' to the tree
  	T ->Branch("eventNum",&eventNum,"eventNum/I");	//branch for the event number
  	T ->Branch("UNIXtime",&UNIXtime,"UNIXtime/I");	//branch for the time
  	T ->Branch("tdc",&tdc,"tdc[2]/F");				//branch for the tdc values (in ns)
	
	
	//read in each line of the data file
	//there is no checking if there are 5 columns of data
	//so this will fail if there are more (or fewer)! 
  	while (fin >> slot >> loopNum >> eventNum >> data >> UNIXtime){
		if(slot==6 && data ){	//this is an 'UP' event
			tdc[0]=data*20;	//20 ns * number of TDC channels = time in ns
			tdc[1]=-10;		//no 'DOWN' data
		}
			
		else if(slot==7){	//this is a 'DOWN' event
			tdc[1]=data*20;	//20 ns * number of TDC channels = time in ns
			tdc[0]=-10;		//no 'UP' data
		}
		else {
			cout << "Invalid data file!" << endl;//somebody tried to put in something other than 6 or 7
		}		
		// the following line dumps all the data to the screen for checking the parsing
    //cout << slot <<"	" << loopNum<< "	" << eventNum<< "	" << data<< "	" << UNIXtime << endl;
    
    cout << "Precessing event number " << eventNum << "\r";//this overwrites the line every time
		T->Fill();	//fill data from this event into the tree	
	}
	fin.close();	//close the input file (to be nice)
	cout << endl;
	cout << " Done!" << endl;	
	T->Write();				//write the TTree to the ROOT file
	outfile->Close();	//close the ROOT file
}

