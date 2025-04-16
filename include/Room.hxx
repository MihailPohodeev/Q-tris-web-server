#ifndef ROOM_HXX
#define ROOM_HXX

#include <list>
#include <memory>
#include <string>
#include <Client.hxx>
#include <RoomParams.hxx>

class Room
{
	// room's name.
	std::string roomName_;

	// list of clients for data exchanges.
	std::list< std::shared_ptr<Client> > clients_;

	// room's parameters;
	RoomParams roomParams_;
public:
	// constructor.
	Room(const std::string& roomName, const RoomParams& roomParams);

	// get room's name.
	std::string get_room_name() const;

	// get queue mode.
	QueueMode get_queue_mode() const;

	// get players count.
	int get_players_count() const;

	// get start level.
	int get_start_level() const;
};

#endif
