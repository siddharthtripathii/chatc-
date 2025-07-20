#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s, string name) {
    string message;
    while (true) {
        getline(cin, message);
        string msg = name + " : " + message;
        int bytesSent = send(s, msg.c_str(), msg.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            cout << "Error sending message" << endl;
            break;
        }

        if (message == "Quit") {
            cout << "Exiting..." << endl;
            shutdown(s, SD_BOTH);
            break;
        }
    }
}

void ReceiveMsg(SOCKET s) {
    char buffer[4096];
    int recvLength;

    while (true) {
        recvLength = recv(s, buffer, sizeof(buffer), 0);
        if (recvLength <= 0) {
            cout << "Disconnected from server" << endl;
            break;
        }
        else {
            string msg(buffer, recvLength);
            cout << msg << endl;
        }
    }
}

int main() {
    if (!Initialize()) {
        cout << "Initialize Winsock failed" << endl;
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "Invalid socket" << endl;
        WSACleanup();
        return 1;
    }

    int port = 12345;
    string serverAddress = "192.168.1.5/24";

    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddress.c_str(), &serveraddr.sin_addr);

    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "Failed to connect to server: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Successfully connected to the server!" << endl;

    cout << "Enter your chat name: ";
    string name;
    getline(cin, name);

    thread senderThread(SendMsg, s, name);
    thread receiverThread(ReceiveMsg, s);

    senderThread.join();
    receiverThread.join();

    closesocket(s);
    WSACleanup();
    return 0;
}
