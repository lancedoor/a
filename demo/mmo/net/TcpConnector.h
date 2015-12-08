#pragma once
#include "TcpConnection.h"

using boost::asio::ip::tcp;
class TcpConnector
{
public:
	static shared_ptr<TcpConnection> Connect(boost::asio::io_service &io_service)
	{
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "55555");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		shared_ptr<TcpConnection> conn = make_shared<TcpConnection>(io_service);
		boost::system::error_code error_code;
		boost::asio::connect(conn->socket_, endpoint_iterator, error_code);

		return error_code ? nullptr : conn;
	}
};

