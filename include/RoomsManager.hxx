#ifndef ROOMS_MANAGER_HXX
#define ROOMS_MANAGER_HXX

#include <list>
#include <mutex>
#include <memory>
#include <string>
#include <Room.hxx>
#include <nlohmann/json.hpp>
#include <RoomParams.hxx>

using json = nlohmann::json;

class RoomsManager
{
	// list with rooms.
	std::list< std::shared_ptr<Room> > rooms_;
	
	// mutex for rooms_ list guarding.
	mutable std::mutex roomsMutex_;
public:
	// constructor.
	RoomsManager() = default;

	// create new room.
	void create_room(const std::string& roomName, const RoomParams& rooms);

	// get rooms with all parameters.
	std::shared_ptr<json> get_rooms_as_json() const;
};

#endif
