#pragma once
#include "TcpConnection.h"

using boost::asio::ip::tcp;
class TcpConnector
{
public:
	//static shared_ptr<TcpConnection> Connect(boost::asio::io_service &io_service)
  static shared_ptr<tcp::socket> Connect(boost::asio::io_service &io_service)
	{
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "55555");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    auto sock = make_shared<tcp::socket>(io_service);
		boost::system::error_code error_code;
		boost::asio::connect(*sock, endpoint_iterator, error_code);
    return error_code ? nullptr : sock;

    //return make_shared<TcpConnection>(sock);

	}
};

