#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


template <typename T>
std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}



UINT WINAPI avg(LPVOID parm);
UINT WINAPI min(LPVOID parm);
UINT WINAPI max(LPVOID parm);
void append_result_file();

CRITICAL_SECTION CriticalSection; 
int que[3] = {0,1,2};       // 0- avg thread 1- min thread 2- max thread; change values in array to decide which thread executes first.
int que_iterator = 0;
LARGE_INTEGER max_execute_time = {};
LARGE_INTEGER min_execute_time = {};
LARGE_INTEGER avg_execute_time = {};


int main() {
	
	HANDLE hFile = NULL;
	int spinlock_counter = 0;
	
	while(!hFile){
		if(spinlock_counter!=0){
			cout << "Spinlock!" << endl;
		}
		hFile = CreateFile(TEXT("random.txt"), // name of the write
                       GENERIC_READ,          // open for reading
                       0,                      // do not share
                       NULL,                   // default security
                       OPEN_ALWAYS,          // ALWAYS open only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template
	}
	
	//copy text from file to string
	
	char * file_content = new char[GetFileSize(hFile,NULL)];
	ReadFile(hFile,file_content,GetFileSize(hFile,NULL),NULL,NULL);
	string data(file_content);	//convert char array to string
	delete file_content; //delete allocated memory ASAP
	
	//use vector to read and store int's from string
	vector<int> numbers;
	int value = 0;
	stringstream stream(data);
	
	while(stream) {
    stream>>value;
    numbers.push_back(value);
    }
    numbers.pop_back(); // pop last member as it will we doubled -- possible fix
    
    for(unsigned i=0; i<numbers.size(); i++){
    	cout << numbers[i] <<" ";
	}
	
	cout << endl;
	
	if (!InitializeCriticalSectionAndSpinCount(&CriticalSection, 
        0x00000400) ){
        	return 1;
		}		
	//execute threads
	
	HANDLE watek[3];
	watek[0] = (HANDLE)_beginthreadex(NULL,0,avg,&numbers,0,NULL);
	watek[1] = (HANDLE)_beginthreadex(NULL,0,min,&numbers,0,NULL);
	watek[2] = (HANDLE)_beginthreadex(NULL,0,max,&numbers,0,NULL);
	
	WaitForMultipleObjects(3,watek,TRUE,INFINITE);		
	DeleteCriticalSection(&CriticalSection);
	//append results file
	
	append_result_file();
	
	//getchar(); //use getchar if you want to see avg, min and max values, otherwise process closes instantly.
	
	return 0;
}



UINT WINAPI avg(LPVOID parm){
	
	int number_in_que = 0;
	
	
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&StartingTime);

	vector <int> * numbers =  (vector <int>*)parm;
	
	int size = numbers->size();
	
	
	float avg = 0;
	
	for(int i = 0; i < size; i++){
		avg+=numbers->at(i);
	}
	
	
	avg/=size;
	
	
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
	while(number_in_que!=que[que_iterator]){
		//do nothing
	}
	
	EnterCriticalSection(&CriticalSection);
	cout << "Average = " << avg;
	cout << " and thread AVG execution time: " << ElapsedMicroseconds.QuadPart <<endl;
	avg_execute_time.QuadPart = ElapsedMicroseconds.QuadPart;
	LeaveCriticalSection(&CriticalSection);
	que_iterator++;
	
	
	
	return 0;
}

UINT WINAPI min(LPVOID parm){
	int number_in_que = 1;
	
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&StartingTime);

	vector <int> * numbers =  (vector <int>*)parm;
	
	int size = numbers->size();
	
	int min = numbers->at(0);
	
	
	for(int i = 0; i < size; i++){
		
		
		if(numbers->at(i) < min){
			min = numbers->at(i);
		}
		
		
	}
	
	
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
	while(number_in_que!=que[que_iterator]){
		//do nothing
	}
	
	EnterCriticalSection(&CriticalSection);
	cout << "Minimum = " << min;
	cout << " and thread MIN execution time: " << ElapsedMicroseconds.QuadPart <<endl;
	min_execute_time.QuadPart = ElapsedMicroseconds.QuadPart;
	LeaveCriticalSection(&CriticalSection);
	que_iterator++;
	return 0;
	
}
UINT WINAPI max(LPVOID parm){
	
	int number_in_que = 2;
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&StartingTime);

	vector <int> * numbers =  (vector <int>*)parm;
	
	int size = numbers->size();	
	int max = numbers->at(0);
	
	
	for(int i = 0; i < size; i++){
		
		if(numbers->at(i) > max){
			max = numbers->at(i);
		}
	}
	
	
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
	while(number_in_que!=que[que_iterator]){
		//do nothing
	}
	
	EnterCriticalSection(&CriticalSection);
	cout << "Maximum = " << max;
	cout << " and thread MAX execution time: " << ElapsedMicroseconds.QuadPart <<endl;
	max_execute_time.QuadPart = ElapsedMicroseconds.QuadPart;
	LeaveCriticalSection(&CriticalSection);
	que_iterator++;
	
	return 0;
}

void append_result_file(){
	
	 HANDLE hFile = CreateFile(TEXT("results.txt"),    // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       OPEN_EXISTING,          // open file
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Couldn't create file. \n");
        return;
    }
    
    bool write_file_flag;
    long unsigned int written_bytes;
    
	
	SetFilePointer(hFile, 0, NULL, FILE_END);
	
	string to_write="\t\t";
	to_write+=to_string(avg_execute_time.QuadPart);
	to_write+="\t\t";
	to_write+=to_string(min_execute_time.QuadPart);
	to_write+="\t\t";
	to_write+=to_string(max_execute_time.QuadPart);
	to_write+="\r\n";
	
	
	
	
	
	
	
	
    write_file_flag = WriteFile( 
                    hFile,           // open file handle
                    &to_write[0],      // start of data to write
                    to_write.length(),  // number of bytes to write
                    &written_bytes, // number of bytes that were written
                    NULL);            // no overlapped structure

	if(!write_file_flag){
		cout << "Couldn't write file." << endl;
		CloseHandle(hFile);
		return ;
	}
		
	CloseHandle(hFile);
	
	
	return ;
	
}




	
	
	
	
	
