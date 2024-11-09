#include <iostream>
#include "TP.h"

using namespace std;

int main(){
    TPQueue qu(4);

    for (int i = 0; i < 8; ++i) { 
		qu.enqueue([i] { 
			cout << "Task " << i << " is running on thread " << this_thread::get_id() << endl; 
			
            // Simulate some work 
			this_thread::sleep_for(chrono::seconds(1)); 
		}); 
	}

	qu.changeTPSize(5);

	for (int j = 0; j < 10; ++j) { 
		qu.enqueue([j] { 
			cout << "Task " << 8 + j << " is running on thread 2 " << this_thread::get_id() << endl; 
			
            // Simulate some work 
			this_thread::sleep_for(chrono::seconds(1)); 
		}); 
	}
}