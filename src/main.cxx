#include <iostream>
#include <ServiceManager.hxx>
#include <memory>

int main(int argc, char** argv)
{
	boost::asio::io_context io;
	std::shared_ptr<ServiceManager> serviceManager = std::make_shared<ServiceManager>(io);
	serviceManager->accept_client();

	io.run();
	return 0;
}
