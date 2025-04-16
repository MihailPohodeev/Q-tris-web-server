#include <ServiceManager.hxx>
#include <nlohmann/json.hpp>
#include <iostream>
#include <Client.hxx>

using json = nlohmann::json;

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
			
			std::shared_ptr client_ptr = std::make_shared<Client>(std::move(socket));
			client_ptr->run([client_ptr, self](std::shared_ptr<json> request)
			{
				json response;
				try
				{
					std::cout << request->dump() << '\n';	

					std::string command = request->at("command");

					if (command == "create_room")
					{
						response["command"] = "create_room_response";
						RoomParams rp;
						rp.queueMode 	= (request->at("queue_mode") == "different" ? QueueMode::Different : QueueMode::Same);
						rp.playersCount = request->at("players_count");
						rp.startLevel	= request->at("start_level");

						if (rp.playersCount < 2 || rp.playersCount > 4 || rp.startLevel < 1)
						{
							response["status"] = "fail";
						}
						else
						{
							self->roomsManager_.create_room(request->at("room_name"), rp);
							response["status"] = "success";
						}
					}
					else if (command == "get_rooms_list")
					{
						response["command"] = "get_rooms_list_response";
						std::shared_ptr<json> rooms = self->roomsManager_.get_rooms_as_json();
						response["status"] = "success";
						response["body"]   = *rooms;
					}
					else if (command == "set_username")
					{
						response["command"] = "set_username_response";
						client_ptr->set_username( request->at("username") );
						response["status"]  = "success";
					}

					client_ptr->send_data( response.dump() );
				}
				catch (const json::type_error& e)
				{
					response["status"] = "fail";
					client_ptr->send_data( response.dump() );
					std::cerr << "Type Error!\n" << e.what() << '\n';
				}
				catch (const json::out_of_range& e)
				{
					response["status"] = "fail";
					client_ptr->send_data( response.dump() );
					std::cerr << "Out of Range Error!\n" << e.what() << '\n';
				}
			});
		}
		self->accept_client();
	});
}
