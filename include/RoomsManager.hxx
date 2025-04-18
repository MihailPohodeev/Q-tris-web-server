#ifndef ROOMS_MANAGER_HXX
#define ROOMS_MANAGER_HXX

#include <mutex>
#include <memory>
#include <string>
#include <Room.hxx>
#include <Client.hxx>
#include <unordered_map>
#include <RoomParams.hxx>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class RoomsManager
{
	// list with waiting rooms.
	std::unordered_map< unsigned int, std::shared_ptr<Room> > waitingRooms_;
	
	// list with processed rooms.
	std::unordered_map< unsigned int, std::shared_ptr<Room> > processedRooms_;
	
	// mutex for rooms_ list guarding.
	mutable std::mutex waitingRoomsMutex_;
	mutable std::mutex processedRoomsMutex_;

	// current ID value for new Room
	unsigned int currentRoomID_;
public:
	// constructor.
	RoomsManager() = default;

	// create new room.
	std::shared_ptr<Room> create_room(std::shared_ptr<Client> client_ptr, const std::string& roomName, const RoomParams& rooms);

	// connect user to the room.
	std::shared_ptr<Room> connect_user_to_room( std::shared_ptr<Client> client_ptr, unsigned int roomID );

	// get rooms with all parameters.
	std::shared_ptr<json> get_rooms_as_json() const;
};

#endif
