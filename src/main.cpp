#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>

#include "./HttpStatus.hpp"
#include "./StringUtils.hpp"
#include "./HttpRequest.hpp"
#include "./handler.hpp"

bool send_full_response(int client_fd, const std::string& response) {
    size_t total_sent = 0;
    size_t to_send = response.size();
    const char* data = response.data(); // works for both text and binary

    while (total_sent < to_send) {
        ssize_t sent = send(client_fd, data + total_sent, to_send - total_sent, 0);
        if (sent > 0) {
            total_sent += sent;
        } else if (sent == 0) {
            fprintf(stderr, "Connection closed by peer\n");
            return false;
        } else {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            perror("send failed");
            return false;
        }
    }

    return true;
}

void handle_client(int client_fd, std::string directory) {
    std::string buffer;
    HttpRequest::HttpRequest current_request; // persistent for partial request
    bool headers_parsed = false;
    size_t expected_length = 0;

    while (true) {
        char tmp[4096];
        ssize_t n = recv(client_fd, tmp, sizeof(tmp), 0);
        if (n <= 0) break;
        buffer.append(tmp, n);

        while (true) {
            if (!headers_parsed) {
                size_t header_end = buffer.find("\r\n\r\n");
                if (header_end == std::string::npos) break; 
                std::string headers_with_request_line = buffer.substr(0, header_end + 4);
                current_request = HttpRequest::HttpRequest::from_headers(headers_with_request_line);

                buffer.erase(0, header_end + 4); 
                expected_length = current_request.getContentLength();
                headers_parsed = true;
            }

            size_t to_take = std::min(expected_length - current_request.body.size(), buffer.size());
            current_request.body.append(buffer.substr(0, to_take));
            buffer.erase(0, to_take);

            if (current_request.body.size() < expected_length) {
                break;
            }

            std::string response = handler::handle(current_request, directory, handler::isClose(current_request));
            send_full_response(client_fd, response);
            
            if (handler::isClose(current_request)) {
              close(client_fd);
              return;
            }

            current_request = HttpRequest::HttpRequest();
            headers_parsed = false;
            expected_length = 0;
            
        }
    }
    close(client_fd);
}


int main(int argc, char **argv) {
  std::string directory = (argc > 2) ? argv[2] : ""; 

  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // TODO: Uncomment the code below to pass the first stage 
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";
  
  while (true) {
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    if (client_fd < 0) {
        perror("accept failed");
        close(server_fd);
        return 1;
    }
    std::cout << "Client connected\n";

    std::thread t(handle_client, client_fd, directory);
    t.detach();
  }
  close(server_fd);

  return 0;
}
