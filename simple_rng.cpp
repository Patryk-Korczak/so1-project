#include <cstdlib>  
#include <ctime>


void random_init(){  
	
	static bool seed_flag = true; 
	
	if(seed_flag){
		srand(time(NULL)); 
		seed_flag = false;
	}
	 
}

int get_single_random_number(int min, int max){ 
	
	random_init();
	
	return rand()%max+min;
}

