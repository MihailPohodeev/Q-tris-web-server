#include <ServiceManager.hxx>
#include <iostream>
#include <Client.hxx>

ServiceManager::ServiceManager(asio::io_context& io, asio::ip::port_type port) : io_context_(io), acceptor_(io, {tcp::v4(), port})
{
	std::cout << "Server has been launched on port : " << port << '\n';
}

// accept new client and continue waiting others potential clients.
void ServiceManager::accept_client()
{
	acceptor_.async_accept([self = shared_from_this()](beast::error_code ec, tcp::socket socket)
	{
		if (ec)
		{
			std::cerr << "Can't accept new client!\n";
		}
		else
		{
			std::cout << "Trying to accept new client!\n";
			std::make_shared<Client>(std::move(socket))->run();
		}
		self->accept_client();
	});
}
