#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <string>
#include <vector>
#include <regex>
#include <random>
#include <direct.h>
#include "main.h"

#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)

#define getcwd _getcwd // stupid MSFT "deprecation" warning
#define BACKUP_PATH ""
#define RELATIVE_PATH "C:\\Users\\mstav\\source\\repos\\sstav\\Maman14_Server\\Debug\\"
#define BACKUP_PATH2 "c:\\backupsvr\\"

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

class IPSetting {
	string ip;
	string port;
public:
	IPSetting(string filename) {
		ifstream  file(RELATIVE_PATH + filename);
		stringstream buffer;
		buffer << file.rdbuf();
		string _data = this->trim(buffer.str());

		int startPos = _data.find(":");
		string set_ip = _data.substr(0, startPos);
		string set_port = _data.substr(startPos+1, _data.length());

		this->ip = set_ip;
		this->port = set_port;
	}

	string getIp() {
		return ip;
	}

	int getPort() {
		if (port == "") return 0;
		return stoi(port);
	}

	string trim(const string& str)
	{
		size_t first = str.find_first_not_of(' ');
		if (string::npos == first)
		{
			return str;
		}
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

};


class Backup {
	vector<string> backup_files;
public:
	Backup(string filename) {
		string line;
		ifstream file;
		file.open(RELATIVE_PATH + filename);

		if (!file.is_open()) {
			perror("Error open");
			exit(EXIT_FAILURE);
		}
		while (getline(file, line)) {
			backup_files.push_back(line);
		}
	}

	int length() {
		return this->getFiles().size();
	}

	vector<string> getFiles() {
		return backup_files;
	}

};

class ServerAction {
	
public:
	static bool save_file_from_backup(string filename , string username, string file_data) {
		try {
			string str = (RELATIVE_PATH + username);
			try {
				_mkdir((RELATIVE_PATH + username).c_str());
			}
			catch (...) {}
				
			
			ofstream out(RELATIVE_PATH + username + "\\"  + filename);
			out << file_data.c_str();
			out.close();
			return true;
		}
		catch (exception& es) {
			cout << "ERROR: " << es.what();
			return false;
		}
	}

	static bool delete_file(string filename, string username) {
		if (remove((RELATIVE_PATH + username + "\\" + filename).c_str()) != 0)
			perror("Error deleting file");
		else
			puts("File successfully deleted");
		return 0;
	}
};


string string_replace(string find, string replace , string str , int limit = 0) {
	int start_pos,i=0;
	while (start_pos = str.find(find)) {
		if (start_pos < 0) {
			break;
		}
		str.replace(start_pos, find.length(), replace);
		if (limit != 0) {
			i++;
			if (i == limit) {
				break;
			}
		}
	}

	return str;

}

std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> strings;
	size_t start;
	size_t end = 0;
	while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
		end = str.find(delim, start);
		strings.push_back(str.substr(start, end - start));
	}
	return strings;
}


string getModuleFilePath(char* argv) {
	std::string cur_dir(argv);
	int pos = cur_dir.find_last_of("/\\");

	std::cout << "path: " << cur_dir.substr(0, pos) << std::endl;
	std::cout << "file: " << cur_dir.substr(pos + 1) << std::endl;
	return 0;
}

bool IsDirectory(const char* path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		return false;
}


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
	name = string_replace("%20", " ", name);
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


IPSetting* init_IPSetting() {
	IPSetting* ipsetting = new IPSetting("server.info");
	cout << ipsetting->getIp() << " : " << ipsetting->getPort();
	return ipsetting;
}

Backup* init_Backup() {
	Backup* backup_files = new Backup("backup.info");
	return backup_files;
}

int create_key() {
	return (int)rand();
}
void main() {

	IPSetting* ipsetting = init_IPSetting();
	Backup* backup_files = init_Backup();
	for (int i = 0; i < backup_files->length(); i++) {
		cout << backup_files->getFiles()[i];
	}

	ServerAction::save_file_from_backup("sss.txt", "isma", "Hellow Hellow Hello shalom\r\nBatia Save");
	ServerAction::delete_file("fgfg.txt", "isma");


	cout << "Server is running..." << endl;
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// AF_INET = TCP Protocol , SOCK_STREAM = stream socket ,  IPPROTO_TCP = 6
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(ipsetting->getPort());
	//Bing the socket to the ip/port
	bind(s, (const sockaddr*)&server, sizeof(server));
	//Listen for incoming requests
	//SOMAXCONN  = 0x7fffffff -> Means unlimited connectation request is allowed.
	listen(s, SOMAXCONN);
	cout << "Listening on " << ipsetting->getIp() << ":" << ipsetting->getPort() << endl;
	cout << "waiting for requests..." << endl;
	while (true) {
		HandleClient(s);
	}
	
	WSACleanup();
	closesocket(s);
	cout << "Size is: " << sizeof(PACKET);
	cout << "Bye";

}