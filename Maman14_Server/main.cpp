#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <string>

#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)

#define PORT 8080

using namespace std;

class PACKET {
public:
	unsigned int user_id;
	char version;
	char op;
	unsigned short name_len;
	char* filename;
	unsigned int size;
	char* payload;
};

void HandleRequest(SOCKET client_incomming) {
	cout << "Detected: Incomming Request..." << endl;
	char clientmsg[1024] = { 0 };
	recv(client_incomming, clientmsg, 1024, 0);
	string data_name = clientmsg;
	unsigned int data_start = data_name.find("GET /");
	unsigned int data_end = data_name.find("HTTP");
	string name = data_name.substr(data_start+5, data_end-5);
	puts(clientmsg);
	puts(name.c_str());
	string data_back = "HTTP/1.1 200 OK\n\n";

	data_back += "<html><body><center><h1>Hello Stav</h1><br><br>"+ name +"</body></html>";
	send(client_incomming, data_back.c_str(), data_back.length(), 0);

	closesocket(client_incomming);
	cout << "waiting for requests..." << endl;
}

void HandleClient(SOCKET s) {

	SOCKET client_incomming = accept(s, NULL, NULL);
	std::thread ct(HandleRequest, client_incomming);
	ct.detach();
	
}


void main() {
	

	PACKET* p = new PACKET();
	p->user_id = 10;
	p->version = '1';
	p->op = '2';
	p->name_len = 3;
	p->filename = nullptr;
	p->size = 4;
	p->payload = nullptr;


	cout << "Server is running..." << endl;
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// AF_INET = TCP Protocol , SOCK_STREAM = stream socket ,  IPPROTO_TCP = 6
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	//Bing the socket to the ip/port
	bind(s, (const sockaddr*)&server, sizeof(server));
	//Listen for incoming requests
	//SOMAXCONN  = 0x7fffffff -> Means unlimited connectation request is allowed.
	listen(s, SOMAXCONN);
	cout << "Listening on 127.0.0.1:" << PORT << endl;
	cout << "waiting for requests..." << endl;
	while (true) {
		HandleClient(s);
	}
	
	WSACleanup();
	closesocket(s);
	cout << "Size is: " << sizeof(PACKET);
	cout << "Bye";

}