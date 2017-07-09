// @file SIM02.cpp
// Headers
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <list>
#include <pthread.h>
#include "SimpleTimer.h"
#include "SimpleTimer.cpp"
SimpleTimer currentTimer;

using namespace std;
// Globals

// Struct to hold configuration information

 struct configData{

	int versionPhase;
     	string filePath;
        string scheduling;
     	int processorCycleTime;
     	int monitorDisplayTime;
    	int hardDriveCycleTime;
     	int printerCycleTime;
     	int keyboardCycleTime;
     	char log;
     	string outputFilePath;

 };

// Struct to hold meta data information and process control block
struct metaData{

	string componentLetter;
	string operation;
        int cycleTime;
        int storeTime;
        int endTime;
        int processID;
        string operationCase;

};

// Function prototypes

   /** loadConfig will intialize the configgration file needed
       @param: filename and config struct
       @pre none
       @post will load all data into config struct
       @return none*/
   void loadConfig(char* filename,configData& storeConfig);

   /** uploadMetaData will store the data to a node based linked list
       @param: filename from cmnd line and created node list
       @pre none
       @post will store all data into a node based list
       @return none*/
   void uploadMetaData(list<metaData>& temp,string filename);

   /** startProcess will start the simulator
       @param: config, metaData 
       @pre none
       @post will begin process and will display or write file
       @return none*/
   void startProcess(configData& information,list<metaData>& storedList);

   /** process function used when creating a pthread in order to call a function
       @param: none
       @pre none
       @post none
       @return none*/

   void* createProcess(void*){

          // Will do nothing
         return '\0';
        }
 

   /** calculate time for each process call
       @param: config needed and metadata
       @pre none
       @post will calculate time
       @return float usedTime*/
   void calcTime(configData& information,list<metaData>& storedList);


// main to take in arguments from command line
int main(int argc, char* argv[])
{
   // Intialize varibles
    configData created;
    list<metaData> metaDataList;

   // Test for correct file input upon startup (from 750730)
     if( argc < 2)
       {
           // No file was provided
            cout<< " Error: A file must be provided to run simulation, try agin " <<endl;
            exit(1);

       }

   // Update current status of loading 
     cout<<" Processing file input . . . "<<endl;

   // Load and Store configuration file 
     loadConfig(argv[1],created);    

   // Read in Meta data to test and store into linked list
    uploadMetaData(metaDataList,created.filePath);    

   // Test for the CPU scheduling requested
    if(created.scheduling == "FIFO")
      {

       // Start FIFO scheduling
        startProcess(created,metaDataList);
      }

    else if(created.scheduling == "SJF")
      {

       // Start SJF scheduling 

      }

    else
      cout<<" Error a specified scheduling algorithum was not choosen "<<endl;

return 0;
} 

// Function Implmenation
void loadConfig(char* filename,configData& storeConfig){
   // Intialize variables
    ifstream fin;
    string temp;
    string file = "config_1.cnf";
    string compare = filename;

   // open and clear file
    fin.clear();
    fin.open(filename);

   // Test if file is correct (from 750730)
   if(compare != file || fin.peek() == EOF)
       {

         cout<<endl<<" Filename was not found or the file is empty "<<endl;

       // End program
         exit(1);

       }

   // go through file and take in needed data (format file set)

      // ignore to skip first line, not needed.
       getline(fin,temp);

      // store the version/Phase
       getline(fin,temp,':');
       fin>>storeConfig.versionPhase;

      // store the file/Path
       getline(fin,temp,':');
       fin>>storeConfig.filePath;

      // store the Scheduling type for FIFO or SJF
       getline(fin,temp,':');
       fin>>storeConfig.scheduling;
       
      // store processor cycle time
       getline(fin,temp,':');
       fin>>storeConfig.processorCycleTime;

      // store Monitor display time
       getline(fin,temp,':');
       fin>>storeConfig.monitorDisplayTime;

      // store hard drive cycle time
       getline(fin,temp,':');
       fin>>storeConfig.hardDriveCycleTime;

      // store printer cycle time
       getline(fin,temp,':');
       fin>>storeConfig.printerCycleTime;

      // store keyboard cycle time
       getline(fin,temp,':');
       fin>>storeConfig.keyboardCycleTime;

      // store log first char <B>, <M> or <F>
       getline(fin,temp,':');
       fin>>temp>>temp;
       fin>>storeConfig.log;
       
      // store new log file path to ouput 
       getline(fin,temp,':');
       fin>>storeConfig.outputFilePath;

   // close file
    fin.close();

}

void uploadMetaData(list<metaData>& temp, string filename){
   // Intialize varibles
    ifstream fin;
    string ignore;
    metaData storeValues;
    char test = ';'; // intial true

   // clear and open file
    fin.clear();
    fin.open(filename.c_str());

   // Test if file is correct (from 750730)
   if(!fin.good() || fin.peek() == EOF)
       {

         cout<<endl<<" Test metaData was not found or the file is empty "<<endl;

       // End program
         exit(1);

       }

      // ignore first header line
         getline(fin,ignore,':');

   // loop through file till end dilimnator is reached
    while(test != '.')
      {
       
        // take in first char for component
         getline(fin,storeValues.componentLetter,'(');
      
        // take in operation line
         getline(fin,storeValues.operation,')');
          
        // take in cycletime
         fin>>storeValues.cycleTime;

        // store all items into node list
         temp.push_back(storeValues);

        // test for period to inform that its the end of meta data
         fin>>test;

        // ignore white space
         getline(fin,ignore,' ');

      }
 
   // Store in a dilimnator for end system program
    storeValues.componentLetter = "S";
    temp.push_back(storeValues);
  
   // close file
    fin.close();

}

void startProcess(configData& information,list<metaData>& storedList) {
   // Intialize varibles
    std::list<metaData>::iterator it = storedList.begin();
    std::list<metaData>::iterator end = storedList.end();
    ofstream fout;
    string filename = information.outputFilePath;
    string IO;
    pthread_t p1;
    int processNumber = 0;

   // test if outputing to file, display or both            
    if(information.log == 'B')
     {

       // create new filepath to oupput data
        fout.open(filename.c_str());

        cout<<" Time"<<" - "<<"Simulator Program starting"<<endl;
        cout<<" Time"<<" - "<<"OS: preparing all processes"<<endl;

        fout<<" Time"<<" - "<<"Simulator Program starting"<<endl;
        fout<<" Time"<<" - "<<"OS: preparing all processes"<<endl;

	// Select next process in FIFO order
	cout<<" Time"<<" - "<<"OS: Selecting next process"<<endl;
        fout<<" Time"<<" - "<<"OS: Selecting next process"<<endl;
        // Next item in FIFO queue
        it++;  

        // Prime loop for first process
         processNumber++;
         cout<<" Time"<<" - "<<"OS: Starting Process "<<processNumber<<endl;
         fout<<" Time"<<" - "<<"OS: Starting Process "<<processNumber<<endl;
         it++;

        // loop for the rest of I/O processes
         while(it != end)
            {

             // Test for end of application to start next one
              if(it->componentLetter == "A" && it->operation == "end")
                {
                   cout<<" Time"<<" - "<<"OS: removing process "<<processNumber<<endl;
                   fout<<" Time"<<" - "<<"OS: removing process "<<processNumber<<endl;
            
                   it++; 
                   it++;
          
                   // Test for end of system 
                     if(it->componentLetter == "S")
                      {
                        // end of metaData list break loop

                         cout<<" Time"<<" - "<<"Simulator program ending "<<endl;
                         fout<<" Time"<<" - "<<"Simulator program ending "<<endl;
                         break;

                      }

                   // start new process if not end of the list
  
                   cout<<" Time"<<" - "<<"OS: selecting next process "<<endl;
                   fout<<" Time"<<" - "<<"OS: selecting next process "<<endl;
                   processNumber++;
                   cout<<" Time"<<" - "<<"OS: Starting Process "<<processNumber<<endl; 
                   fout<<" Time"<<" - "<<"OS: Starting Process "<<processNumber<<endl;    

                }
            
             // create process for I/O calls 
              pthread_create(&p1,NULL,createProcess,NULL);
            
            // Test care for the word run
             if(it->operation == "run")
               {

                 it->operation = "processing";

               }

             // cases for input/output or action
              if(it->componentLetter == "I")
               {
                 
                 IO = " input";

               }

             else if(it->componentLetter == "O")
               {

                 IO = " output";

               }

              else IO = " action";

             // run current process 
              cout<<" Time"<<" - "<<"process " <<processNumber<<":"<<" Start "<<it->operation<<IO<<endl;
              cout<<" Time"<<" - "<<"process "<<processNumber<<":"<<" End "<<it->operation<<IO<<endl;

              fout<<" Time"<<" - "<<"process "<<processNumber<<":"<<" Start "<<it->operation<<IO<<endl;
              fout<<" Time"<<" - "<<"process "<<processNumber<<":"<<" End "<<it->operation<<IO<<endl;


            // make sure process is done after time cycle
              pthread_cancel(p1);

            // increment to next process
              it++;

            } // end of while loop

        
       // close file
        fout.close();


     } // end of if statement for Both to monitor & file

   }

