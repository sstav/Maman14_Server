#include <iostream>
#include <thread>

#define PORT 8080

using namespace std;

class PACKET {
	unsigned int user_id;
	char version;
	char op;
	unsigned short name_len;
	char* filename;
	unsigned int Payload;
};

void main() {
	PACKET* p = new PACKET();
	cout << "Size is: " << sizeof(p);

}