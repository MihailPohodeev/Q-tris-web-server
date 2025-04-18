#include <RoomsManager.hxx>
#include <iostream>

// create new room.
std::shared_ptr<Room> RoomsManager::create_room(std::shared_ptr<Client> client_ptr, const std::string& roomName, const RoomParams& rooms)
{
	std::lock_guard<std::mutex> lock(waitingRoomsMutex_);
	auto room_ptr = std::make_shared<Room>(roomName, currentRoomID_, rooms);
	waitingRooms_[currentRoomID_] = room_ptr;
	room_ptr->connect_client(client_ptr);
	++currentRoomID_;
	return room_ptr;
}

// connect user to the room.
std::shared_ptr<Room> RoomsManager::connect_user_to_room( std::shared_ptr<Client> client_ptr, unsigned int roomID )
{
	try
	{
		std::lock_guard<std::mutex> lock(waitingRoomsMutex_);
		std::shared_ptr<Room> room_ptr = waitingRooms_.at(roomID);
		room_ptr->connect_client(client_ptr);
		return room_ptr;
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "Out of Range Error!\nCannot find room among waiting rooms with ID : " << roomID << '\n' << e.what() << '\n';
	}
	return nullptr;
}

// get rooms with all parameters.
std::shared_ptr<json> RoomsManager::get_rooms_as_json() const
{
	std::shared_ptr<json> result = std::make_shared<json>();
	*result = json::array();

	{
		std::lock_guard<std::mutex> lock(waitingRoomsMutex_);
		for (auto it = waitingRooms_.begin(); it != waitingRooms_.end(); it++)
		{
			json elem;
			elem["room_name"] 	= it->second->get_room_name();
			elem["room_id"]		= it->second->get_id();
			elem["queue_mode"]	= ( it->second->get_queue_mode() == QueueMode::Different ? "different" : "same" );
			elem["players_count"]	= it->second->get_players_count();
			elem["start_level"]	= it->second->get_start_level();
			result->push_back(elem);
		}
	}

	return result;
}
