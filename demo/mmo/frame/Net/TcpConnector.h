#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
class TcpConnector : public enable_shared_from_this<TcpConnector>
{
public:
  TcpConnector(boost::asio::io_service &io_service)
  : io_service_(io_service)
  , resolver_(io_service) {
  }
  void Connect() {
    tcp::resolver::query query("localhost", "55555");
    resolver_.async_resolve(query, boost::bind(&TcpConnector::ResolveHandler, shared_from_this(), _1, _2));
  }
public:
  virtual void OnConnected(shared_ptr<tcp::socket> sock, const boost::system::error_code &ec) {}
private:
  void ResolveHandler(const boost::system::error_code &ec, tcp::resolver::iterator it) {
    if (!ec) {
      auto sock = make_shared<tcp::socket>(io_service_);
      boost::asio::async_connect(*sock, it, boost::bind(&TcpConnector::ConnectHandler, shared_from_this(), sock, _1, _2));
    }
  }
  void ConnectHandler(shared_ptr<tcp::socket> sock, const boost::system::error_code &ec, tcp::resolver::iterator it) {
    OnConnected(sock, ec);
  }
private:
  boost::asio::io_service &io_service_;
  tcp::resolver resolver_;
};

