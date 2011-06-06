
#include <arpa/inet.h>
#include "utility.h"
#include "rpc_server.h"
#include "event.h"
#include "log.h"
#include "net_utility.h"
#include "rpc_connection.h"

namespace eventrpc {
RpcServer::RpcServer()
  : host_(""),
    port_(0),
    listen_fd_(0) {
}

RpcServer::~RpcServer() {
  dispatcher_->DeleteEvent(event_);
  delete event_;
}

void RpcServer::Start() {
  listen_fd_ = NetUtility::Listen(host_.c_str(), port_);
  ASSERT_TRUE(listen_fd_ > 0);
  event_ = new RpcServer::RpcServerEvent(listen_fd_,
                                         EVENT_READ, this);
  dispatcher_->AddEvent(event_);
  ASSERT_NE(static_cast<RpcServerEvent*>(NULL), event_);
}

void RpcServer::Stop() {
  dispatcher_->Stop();
}

int RpcServer::HandleAccept() {
  int fd;
  char buffer[20];
  for (int i = 0; i < 200; ++i) {
    struct sockaddr_in address;
    fd = NetUtility::Accept(listen_fd_, &address);
    if (fd == -1) {
      break;
    }
    VLOG_INFO() << "accept connection from "
      << inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer))
      << ":" << ntohs(address.sin_port);
    RpcConnection* connection = rpc_connection_manager_.GetConnection();
    connection->set_fd(fd);
    connection->set_client_address(address);
    connection->set_rpc_method_manager(&rpc_method_manager_);
    connection->set_rpc_connection_manager(&rpc_connection_manager_);
    connection->set_dispacher(dispatcher_);
    dispatcher_->AddEvent(connection->event());
  }
}

}; // namespace eventrpc
