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
			
			std::shared_ptr client_ptr = std::make_shared<Client>( std::move(socket) );
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
							std::shared_ptr<Room> room_ptr = self->roomsManager_.create_room(client_ptr, request->at("room_name"), rp);
							client_ptr->set_room(room_ptr);
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
					else if (command == "connect_to_room")
					{
						response["command"] = "connect_to_room_response";
						std::shared_ptr<Room> room_ptr = self->roomsManager_.connect_user_to_room( client_ptr, request->at("room_id") );
						client_ptr->set_room(room_ptr);
						room_ptr->notify_all( room_ptr->get_clients_info() );
						response["status"]  = ( room_ptr.get() ? "success" : "fail" );
					}
					else if (command == "get_room_clients_info")
					{
						response["command"] = "room_clients_info";
						std::shared_ptr<json> clientsDataJSON_ptr = client_ptr->get_room()->get_clients_info();
						response["clients"] = *clientsDataJSON_ptr;
						response["status"] = "success";
					}
					else if (command == "disconnect")
					{
						response["command"] = "disconnect_response";
						std::shared_ptr<Room> room_ptr = client_ptr->get_room();
						room_ptr->disconnect_client(client_ptr);
						client_ptr->set_room(nullptr);
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
