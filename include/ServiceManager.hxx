#ifndef SERVICE_MANAGER_HXX
#define SERVICE_MANAGER_HXX

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>

#define STANDARD_PORT 38532

namespace beast 	= boost::beast;
namespace websocket	= beast::websocket;
namespace asio		= boost::asio;
using tcp		= boost::asio::ip::tcp;



class ServiceManager : public std::enable_shared_from_this<ServiceManager>
{
	// io_context for management asynchronous operations.
	asio::io_context& io_context_;

	// acceptor for accepting new clients.
	tcp::acceptor acceptor_;
public:
	// constructor
	ServiceManager(asio::io_context& io, asio::ip::port_type port = STANDARD_PORT);

	// start accepting new clients.
	void accept_client();

	// delete unnesessary constructors and operators.
	// --------------------------------------------------
	ServiceManager(ServiceManager&)  = delete;
	ServiceManager(ServiceManager&&) = delete;

	ServiceManager operator= (ServiceManager&)  = delete;
	ServiceManager operator= (ServiceManager&&) = delete;
	// --------------------------------------------------
};

#endif
