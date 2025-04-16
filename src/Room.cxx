#include <Room.hxx>

Room::Room(const std::string& roomName, const RoomParams& roomParams) : roomName_(roomName), roomParams_(roomParams) {}

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
