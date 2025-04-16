#ifndef CLIENT_HXX
#define CLIENT_HXX

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <memory>

namespace beast		= boost::beast;
namespace websocket	= beast::websocket;
namespace asio		= boost::asio;
using tcp		= boost::asio::ip::tcp;

class Client : public std::enable_shared_from_this<Client>
{
	// websocket for client-server communication.
	websocket::stream<tcp::socket> ws_;

	// buffer for data receiving.
	beast::flat_buffer buffer_;

	// username of user.
	std::string username_;
public:
	// constructor.
	Client(tcp::socket);

	// start handling of client.
	void run();

	// main logic of data exchange between server and client.
	void input_output_handling();

	void set_username(const std::string& username);
	std::string get_username() const;
};

#endif
