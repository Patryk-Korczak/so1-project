#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>

#include "simple_rng.h"

using namespace std;


template <typename T>
std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}


void print_array(vector <int> * random_int_array); //prints array to console
bool save_array(vector <int> * random_int_array);  // returns TRUE if success, if saving failed returns FALSE
void random_numbers(vector <int> * random_int_array, int cnt, int min, int max); // saves random numbers in array
bool create_result_file(int cnt, int wait_time, int repeats); //creates 'results.txt' file that will be appended by child process

/*
* Using snake_case naming type.
*/


int main(int argc, char** argv) {
	
	if(argc == 1){
		cout << "Error, no additional parameter included." << endl;
		
		return 1;
	}
	else if(argc!=4){
		cout << "Error, not enough parameters provided - expected: name, count, wait time, repeats." <<endl;
		
		return 1;
	}
	else{
	
		int cnt = atoi(argv[1]);
		int wait_time = atoi(argv[2]);
		int repeats = atoi(argv[3]);
		
		//create results file to store execution time each time new process starts
		
		bool output = create_result_file(cnt,wait_time,repeats);
		if(!output){
			cout << "Couldn't create output file, program will now exit." << endl;
			return 1;
		}
		
		vector <int> random_int_array;
		
		for(int i = 0; i < repeats; i++){
		
			random_numbers(&random_int_array, cnt, 1, 100);
			print_array(&random_int_array);
		
		
			bool is_saved = save_array(&random_int_array);
			
			random_int_array.clear();
		
			if(!is_saved){
				cout << " Couldn't save array, program will now exit." << endl;
				return 1;
			}
		
			STARTUPINFO si = STARTUPINFO();
			PROCESS_INFORMATION pi;
	
			bool process_start = CreateProcess("potomny.exe",0,0,0,0,CREATE_NEW_CONSOLE,0,0, &si, &pi);	
			if(!process_start){
				cout << "Couldn't create child process, program will now exit." << endl;
				return 1;
			}
			WaitForSingleObject(pi.hProcess,INFINITE);
			
			Sleep(wait_time * 1000);
		}
			cout << endl;
			cout << "____________"<<endl;
			cout << "Executed child process " << repeats << " times with " << wait_time << " seconds interval. Check 'results.txt' file to see execute times." << endl;	
			cout << "____________"<<endl;
					
	}
	
	
	
	
	return 0;
}


void print_array(vector <int> *random_int_array){
	for(unsigned int i=0; i<random_int_array->size(); i++){
		cout << random_int_array->at(i) << " ";
	}
	cout << endl;	
}

void random_numbers(vector <int> *random_int_array, int cnt, int min, int max){
	for(int i=0; i<cnt; i++){
		random_int_array -> push_back(get_single_random_number(min,max));
	}
}

bool save_array(vector <int> *random_int_array){
	
	 HANDLE hFile = CreateFile(TEXT("random.txt"),    // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_ALWAYS,          // ALWAYS create new file only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Couldn't create file. \n");
        return 0;
    }
    
    bool write_file_flag;
    long unsigned int written_bytes;
    
	string to_write;
	
	for(unsigned int i = 0; i < random_int_array->size(); i++){
		to_write+=to_string(random_int_array->at(i));
		to_write+=" ";
	}
	
    write_file_flag = WriteFile( 
                    hFile,           // open file handle
                    &to_write[0],      // start of data to write
                    to_write.length(),  // number of bytes to write
                    &written_bytes, // number of bytes that were written
                    NULL);            // no overlapped structure

	if(!write_file_flag){
		cout << "Couldn't write file." << endl;
		CloseHandle(hFile);
		return 0;
	}
	else{
		cout << "Saved " << written_bytes <<  " bytes." << endl;
	}
	
	CloseHandle(hFile);
	
	
	return 1;
	
}
	
bool create_result_file(int cnt, int wait_time, int repeats){
	
	 HANDLE hFile = CreateFile(TEXT("results.txt"),    // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_ALWAYS,          // ALWAYS create new file only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Couldn't create file. \n");
        return 0;
    }
    
    bool write_file_flag;
    long unsigned int written_bytes;
    SYSTEM_INFO si;
    
	string to_write = "Total numbers: ";
	to_write+= to_string(cnt);
	to_write+=" | Pause: ";
	to_write+= to_string(wait_time);
	to_write+=" | Repeats: ";
	to_write+= to_string(repeats);
	to_write+=" | Version: Single-Thread ";
	to_write+="\r\n";
	to_write+="MACHINE: \r\n";
	
	short processor = si.dwProcessorType;
	
	if(processor == 9 ){
		to_write+= "PROCESSOR_ARCHITECTURE_AMD64\r\n";
	}
	else if(processor == 5){
		to_write+= "PROCESSOR_ARCHITECTURE_ARM\r\n";
	}
	else if(processor == 12){
		to_write+= "PROCESSOR_ARCHITECTURE_ARM64\r\n";
	}
	else if(processor == 6){
		to_write+= "PROCESSOR_ARCHITECTURE_IA64\r\n";
	}
	else if(processor == 0){
		to_write+= "PROCESSOR_ARCHITECTURE_INTEL\r\n";
	}
	else{
		to_write+= "PROCESSOR_ARCHITECTURE_UNKNOWN\r\n";
	}
	
	to_write+="Page size: ";
	to_write+=to_string(si.dwPageSize);
	to_write+="\r\n";
	
	
	to_write+="\t\tEXECUTION TIME\r\n";
	
	
	
	
	
	
    write_file_flag = WriteFile( 
                    hFile,           // open file handle
                    &to_write[0],      // start of data to write
                    to_write.length(),  // number of bytes to write
                    &written_bytes, // number of bytes that were written
                    NULL);            // no overlapped structure

	if(!write_file_flag){
		cout << "Couldn't write file." << endl;
		CloseHandle(hFile);
		return 0;
	}
		
	CloseHandle(hFile);
	
	
	return 1;
	
}
	
	
	
	
	
	

