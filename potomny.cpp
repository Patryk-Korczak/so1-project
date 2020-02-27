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



UINT WINAPI calculate(LPVOID parm);
void append_result_file();
LARGE_INTEGER execute_time = {};


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
	
	delete file_content; // free memory as soon as possible

	
	//use vector to read and store int's from string
	vector<int> numbers;
	int value = 0;
	stringstream stream(data);
	
	while(stream) {
    stream>>value;
    numbers.push_back(value);
    }
    
    numbers.pop_back(); // pop last member as it will we doubled -- possible fix
    
    
    for(unsigned int i=0; i<numbers.size(); i++){
    	cout << numbers[i] <<" ";
	}
	
	cout << endl;
	
	
	HANDLE watek = (HANDLE)_beginthreadex(NULL,0,calculate,&numbers,0,NULL);

	
	WaitForSingleObject(watek,INFINITE);
	
	append_result_file();
	
	//getchar();   //use getchar, if you want to check avg,min and max values, otherwise process closes instantly after finishing.
	
	return 0;
}



UINT WINAPI calculate(LPVOID parm){
	
	
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&StartingTime);

	vector <int> * numbers =  (vector <int>*)parm;
	
	int size = numbers->size();
	
	int min = numbers->at(0);
	int max = numbers->at(0);
	float avg = 0;
	
	for(int i = 0; i < size; i++){
		avg+=numbers->at(i);
		
		if(numbers->at(i) < min){
			min = numbers->at(i);
		}
		
		if(numbers->at(i) > max){
			max = numbers->at(i);
		}
	}
	
	
	avg/=size;
	
	
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
	cout << "Min = " << min << endl;
	cout << "Max = " << max << endl;
	cout << "Avg = " << avg << endl;
	cout << "Thread execution time: " << ElapsedMicroseconds.QuadPart <<endl;
	
	execute_time.QuadPart = ElapsedMicroseconds.QuadPart;
	
	
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
	to_write+=to_string(execute_time.QuadPart);
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





	
	
	
	
	

