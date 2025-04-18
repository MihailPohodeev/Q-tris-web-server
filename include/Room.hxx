#ifndef ROOM_HXX
#define ROOM_HXX

#include <list>
#include <mutex>
#include <memory>
#include <string>
#include <Client.hxx>
#include <RoomParams.hxx>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

namespace asio  = boost::asio;
using json	= nlohmann::json;

class Room
{
	// room's name.
	std::string roomName_;

	// room's ID.
	unsigned int ID_;

	// list of clients for data exchanging.
	std::list< std::shared_ptr<Client> > clients_;

	// mutex for clients guarding.
	std::mutex clientsMutex_;

	// room's parameters;
	RoomParams roomParams_;

	// time delay for data exchanging.
	//asio::steady_timer timer;
	
public:
	// constructor.
	Room(const std::string& roomName, unsigned int id, const RoomParams& roomParams);

	// connect client to the room.
	void connect_client(std::shared_ptr<Client> client_ptr);
	// disconnect client from room.
	void disconnect_client(std::shared_ptr<Client> client_ptr);

	// get info about connected users.
	std::shared_ptr<json> get_clients_info() const;

	// notify all users about smtng.
	void notify_all(std::shared_ptr<json> json);

	// get room's name.
	std::string get_room_name() const;

	// get queue mode.
	QueueMode get_queue_mode() const;

	// get players count.
	int get_players_count() const;

	// get start level.
	int get_start_level() const;

	// get room's ID.
	unsigned int get_id() const;
};

#endif
