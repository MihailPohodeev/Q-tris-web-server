#include <Room.hxx>
#include <iostream>

// constructor.
Room::Room(const std::string& roomName, unsigned int id, const RoomParams& roomParams) : roomName_(roomName), roomParams_(roomParams), ID_(id) {}

// connect client to the room.
void Room::connect_client(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(clientsMutex_);
	client_ptr->set_ID( clients_.size() );
	clients_.push_back(client_ptr);
}

// disconnect client from room.
void Room::disconnect_client(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(clientsMutex_);
	clients_.remove(client_ptr);
}

// get info about connected users.
std::shared_ptr<json> Room::get_clients_info() const
{
	std::shared_ptr<json> clientsInfo_ptr = std::make_shared<json>();

        (*clientsInfo_ptr) = json::array();
        for (auto it = clients_.begin(); it != clients_.end(); ++it)
        {
                json clientInfo; 
                clientInfo["id"]        = (*it)->get_ID();
                clientInfo["username"]  = (*it)->get_username();
                (*clientsInfo_ptr).push_back(clientInfo);
        }
	return clientsInfo_ptr;
}

// notify all users about smtng.
void Room::notify_all(std::shared_ptr<json> jsonData)
{
	std::string data = jsonData->dump();
	std::lock_guard<std::mutex> lock(clientsMutex_);
	for (auto it = clients_.begin(); it != clients_.end(); ++it)
		(*it)->send_data(data);
	std::cout << "notified!\n";
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
