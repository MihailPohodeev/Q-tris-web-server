#include <Room.hxx>
#include <chrono>
#include <iterator>
#include <iostream>
#include <boost/asio.hpp>

extern boost::asio::io_context io;

// constructor.
Room::Room(const std::string& roomName, unsigned int id, const RoomParams& roomParams) : roomName_(roomName),
											 roomParams_(roomParams),
											 ID_(id),
											 readyClients_(roomParams.playersCount),
											 isGameProcessOccuringFlag_(false),
											 timer_(io) {}

// handle room's game process.
void Room::start_handle_room()
{
	timer_.expires_after( std::chrono::seconds(1) );
	timer_.async_wait([self = shared_from_this()](const auto& ec)
		{
			if (!ec)
			{
				if (self->isGameProcessOccuringFlag_)
				{
					std::cout << "Game started!\n";
				}
				else
				{
					std::lock_guard<std::mutex> lock(self->clientsMutex_);
					bool canStartGame = true;
					for (auto it = self->readyClients_.begin(); it != self->readyClients_.end(); it++)
					{
						if ( !(*it) )
						{
							canStartGame = false;
							break;
						}
					}
					if (canStartGame)
						self->isGameProcessOccuringFlag_ = true;
				}
			}

			self->start_handle_room();
		});
}

// connect client to the room.
void Room::connect_client(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(clientsMutex_);
	clients_.push_back(client_ptr);
	for ( auto it = readyClients_.begin(); it != readyClients_.end(); it++)
		(*it) = false;
}

// disconnect client from room.
void Room::disconnect_client(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(clientsMutex_);
	clients_.remove(client_ptr);
	for ( auto it = readyClients_.begin(); it != readyClients_.end(); it++)
		(*it) = false;
}

// set ready-flag for client.
bool Room::set_client_ready_flag(std::shared_ptr<Client> client_ptr, bool value)
{
	int index = _get_client_position_in_list_(client_ptr);
	if (index < 0 || index > roomParams_.playersCount)
		return false;
	readyClients_[index] = value;
	return true;
}

// get info about connected users.
std::shared_ptr<json> Room::get_clients_info() const
{
	std::shared_ptr<json> clientsInfo_ptr = std::make_shared<json>();

        (*clientsInfo_ptr) = json::array();

	std::lock_guard<std::mutex> lock(clientsMutex_);
        for (auto it = clients_.begin(); it != clients_.end(); ++it)
        {
                json clientInfo; 
                clientInfo["id"]        = std::distance(clients_.begin(), it);
                clientInfo["username"]  = (*it)->get_username();
                clientsInfo_ptr->push_back(clientInfo);
        }
	return clientsInfo_ptr;
}

int Room::_get_client_position_in_list_(std::shared_ptr<Client> client_ptr)
{
	auto it = std::find(clients_.begin(), clients_.end(), client_ptr);
	if ( it == clients_.end() )
		return -1;
	return std::distance(clients_.begin(), it);
}

// get client's position in list.
int Room::get_client_position_in_list(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(clientsMutex_);
	auto it = std::find(clients_.begin(), clients_.end(), client_ptr);
	if ( it == clients_.end() )
		return -1;
	return std::distance(clients_.begin(), it);
}

// notify all users about smtng.
void Room::notify_all(std::shared_ptr<json> jsonData)
{
	//std::string data = jsonData->dump();
	std::lock_guard<std::mutex> lock(clientsMutex_);
	for (auto it = clients_.begin(); it != clients_.end(); ++it)
	{
		(*jsonData)["client_id"] = _get_client_position_in_list_(*it);
		(*it)->send_data( jsonData->dump() );
	}
}

// get room's name.
std::string Room::get_room_name() const
{
	return roomName_;
}

// get queue mode.
QueueMode Room::get_queue_mode() const
{
	return roomParams_.queueMode;
}

// get players count.
int Room::get_players_count() const
{
	return roomParams_.playersCount;
}

// get start level.
int Room::get_start_level() const
{
	return roomParams_.startLevel;
}

// get room's ID.
unsigned int Room::get_id() const
{
	return ID_;
}

// get current value of real player in the room.
unsigned int Room::real_players_count() const
{
	return clients_.size();
}
