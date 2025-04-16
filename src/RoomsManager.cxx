#include <RoomsManager.hxx>

// create new room.
void RoomsManager::create_room(const std::string& roomName, const RoomParams& rooms)
{
	std::lock_guard<std::mutex> lock(roomsMutex_);
	rooms_.push_back( std::make_shared<Room>(roomName, rooms) );
}

// get rooms with all parameters.
std::shared_ptr<json> RoomsManager::get_rooms_as_json() const
{
	std::shared_ptr<json> result = std::make_shared<json>();
	*result = json::array();

	{
		std::lock_guard<std::mutex> lock(roomsMutex_);
		for (auto it = rooms_.begin(); it != rooms_.end(); it++)
		{
			json elem;
			elem["room_name"] 	= (*it)->get_room_name();
			elem["queue_mode"]	= ( (*it)->get_queue_mode() == QueueMode::Different ? "different" : "same" );
			elem["players_count"]	= (*it)->get_players_count();
			elem["start_level"]	= (*it)->get_start_level();
			result->push_back(elem);
		}
	}

	return result;
}
