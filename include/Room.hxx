#ifndef ROOM_HXX
#define ROOM_HXX

#include <list>
#include <vector>
#include <mutex>
#include <memory>
#include <string>
#include <Client.hxx>
#include <RoomParams.hxx>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

namespace asio  = boost::asio;
using json	= nlohmann::json;

class Room : public std::enable_shared_from_this<Room>
{
	// room's name.
	std::string roomName_;

	// room's ID.
	unsigned int ID_;

	// vector of clients for data exchanging.
	std::list< std::shared_ptr<Client> > clients_;
	// vector of boolean values that means : is client ready?
	std::vector< bool > readyClients_;

	// mutex for clients guarding.
	mutable std::mutex clientsMutex_;

	// room's parameters;
	RoomParams roomParams_;

	// time delay for data exchanging.
	asio::steady_timer timer_;
	
	// get client position.
	int _get_client_position_in_list_(std::shared_ptr<Client> client_ptr);

	// handle waiting or game process flag.
	bool isGameProcessOccuringFlag_;

	// JSON with data for clients.
	json intermediateJSON_ExchangeFrame_;

	// mutex for guarding exchange frame.
	mutable std::mutex exchangeFrameMutex_;
public:
	// handle room's game process.
	void start_handle_room();
	
	// constructor.
	Room(const std::string& roomName, unsigned int id, const RoomParams& roomParams);

	// add JSON data frame for exchanging.
	void add_package(std::shared_ptr<Client> client_ptr, std::shared_ptr<json> data);

	// connect client to the room.
	void connect_client(std::shared_ptr<Client> client_ptr);
	// disconnect client from room.
	void disconnect_client(std::shared_ptr<Client> client_ptr);

	// get info about connected users.
	std::shared_ptr<json> get_clients_info() const;

	// notify all users about smtng.
	void notify_all(std::shared_ptr<json> json);

	// set ready-flag for client.
	bool set_client_ready_flag(std::shared_ptr<Client> client_ptr, bool value);

	// get client's position in list.
	int get_client_position_in_list(std::shared_ptr<Client> client_ptr);

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

	// get current value of real player in the room.
	unsigned int real_players_count() const;
};

#endif
