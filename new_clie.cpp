#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
const int PORT = 8080;
struct Command {
    std::string device;
    std::string action;
};
 
void send_command(int client_socket, const Command& command) {
        
        std::string message = command.device + "," + command.action;
        send(client_socket, message.c_str(), message.size(), 0);
 
        char buffer[1024];
        std::string response;
        ssize_t bytes_received;
        
        while((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0))>0){
            buffer[bytes_received] = '\0';
            response += buffer;
            if (response.find('\n') !=std::string::npos) {
               break;
            }
        }
        if (bytes_received <= 0) {
            std::cerr << "\nError receiving response from server." << std::endl;
            return;
        }
 
   
        std::cout << "\nServer response: " << response << std::endl;
}
   
        
   
 
int main() {
    
 
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            std::cerr << "\nError creating client socket." << std::endl;
            exit(EXIT_FAILURE);
        }
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
 
       if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
            std::cerr << "Invalid server address." << std::endl;
            ::close(client_socket);
            exit(EXIT_FAILURE);
        }
 
        if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
            std::cerr << "\nError connecting to the server." << std::endl;
            ::close(client_socket);
            exit(EXIT_FAILURE);
        }
        else{
            std::cout<<"\nConnected to server\n";
        }
 
    
 
    while (true) {
        std::cout<<"\n" << "Available devices:\n Lamp\n Fan\n Tv\n Refrigerator\n Air_cooler\n Washing_machine\n" << std::endl;
        std::cout << "Enter the device you want to control (or 'exit' to quit): ";
        std::string device;
        std::getline(std::cin, device);
 
        if (device == "exit") {
            break;
        }
 
        std::cout << "Enter the action (on/off): ";
        std::string action;
        std::getline(std::cin, action);
        
 
        Command command{device, action};
        send_command(client_socket, command);
    }
    
    ::close(client_socket);
 
    return 0;
}
