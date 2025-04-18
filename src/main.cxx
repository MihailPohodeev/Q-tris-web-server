#include <thread>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include <ServiceManager.hxx>
#include <TaskPool.hxx>

boost::asio::io_context io;

int main(int argc, char** argv)
{
	std::shared_ptr<ServiceManager> serviceManager = std::make_shared<ServiceManager>(io);
	serviceManager->accept_client();

	{
		unsigned int threadsCount = std::max( 1u, std::thread::hardware_concurrency() );
		std::unique_ptr<TaskPool> taskPool = std::make_unique<TaskPool>(threadsCount);

		for ( int i = 0; i < threadsCount; i++)
			taskPool->add_task([]() { io.run(); });
	}
	return 0;
}
