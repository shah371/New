#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
 
const int PORT = 8080;
const int MAX_CONNECTIONS = 5;
 
struct Device {
    std::string name;
    bool state;
};
 
std::vector<Device> devices;
 
    void initializeDevices() {
        devices = {
            {"lamp", false},
            {"fan", false},
            {"tv", false},
            {"refrigerator", false},
            {"air_cooler", false},
            {"washing_machine", false}
        };
    }
 
    void handle_client(int client_socket) {
        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cerr << "Error receiving data from client." << std::endl;
            return;
        }
 
        buffer[bytes_received] = '\0';
        std::string request(buffer);
 
        std::string device, action;
        size_t pos = request.find(',');
        if (pos != std::string::npos) {
            device = request.substr(0, pos);
            action = request.substr(pos + 1);
            Device* findDevice(const std::string& deviceName);
            Device* foundDevice = findDevice(device);
            if (foundDevice != nullptr && (action == "on" || action == "off")) {
                foundDevice->state = (action == "on");
                std::string response = device + " is turned " + action;
                send(client_socket, response.c_str(), response.length(), 0);
            } else {
                const char* response = "Invalid command. Please check your input.";
                send(client_socket, response, strlen(response), 0);
            }
        } else {
            const char* response = "Invalid command format. Please use 'device,action'.";
            send(client_socket, response, strlen(response), 0);
        }
    }
 
    Device* findDevice(const std::string& deviceName) {
        for (Device& device : devices) {
   if (device.name == deviceName) {
                return &device;
            }
        }
        return nullptr;
    }
 
    
int main() {
   int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            std::cerr << "Error creating server socket." << std::endl;
            exit(EXIT_FAILURE);
        }
        
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(PORT);
 
        if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
            std::cerr << "Error binding server socket." << std::endl;
            close(server_socket);
            exit(EXIT_FAILURE);
        }
 
       if (listen(server_socket, MAX_CONNECTIONS) == -1) {
            std::cerr << "Error listening on server socket." << std::endl;
            close(server_socket);
            exit(EXIT_FAILURE);
        }
 
        initializeDevices();
        std::cout << "Home Automation Server is listening on port " << PORT << std::endl;
 
    
        while (true) {
            int client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket == -1) {
                std::cerr << "Error accepting client connection." << std::endl;
                continue;
            }
 
            handle_client(client_socket);
            
        }
    
    
    return 0;
}
