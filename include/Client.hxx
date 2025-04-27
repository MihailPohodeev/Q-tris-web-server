#ifndef CLIENT_HXX
#define CLIENT_HXX

#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <memory>
#include <queue>
#include <mutex>

namespace beast		= boost::beast;
namespace websocket	= beast::websocket;
namespace asio		= boost::asio;
using tcp		= boost::asio::ip::tcp;
using json		= nlohmann::json;

// declaration of Room class for pointer.
class Room;

class Client : public std::enable_shared_from_this<Client>
{
	// websocket for client-server communication.
	websocket::stream<tcp::socket> ws_;

	// buffer for data receiving.
	beast::flat_buffer buffer_;

	// username of user.
	std::string username_;

	// room where client plays. nullptr - if client isn't playing.
	std::shared_ptr<Room> room_;

	// queue of messages for correct sending.
	std::queue<std::string> messagesQueue_;

	// flag, if writing is occuring.
	std::atomic<bool> isWriting;

	// flag, if client is loose.
	std::atomic<bool> isLoose;

	// send data from messagesQueue_.
	void _start_data_sending_();
public:
	// constructor.
	Client(tcp::socket);

	// it pushs data to messagesQueue_.
	void send_data(const std::string& data);

	// start handling of client.
	void run(std::function<void(std::shared_ptr<json>)> callback);

	// main logic of data exchange between server and client.
	void input_output_handling(std::function<void(std::shared_ptr<json>)> callback);

	// set room for handling game process.
	void set_room(std::shared_ptr<Room> room);

	// return current client's room.
	std::shared_ptr<Room> get_room() const;

	// getter and setter for username_ field.
	void set_username(const std::string& username);
	std::string get_username() const;

	// set loose-value.
	void set_loose_value(bool value);
	bool is_loose() const;

	// get client's id in room.
	int get_id();
};

#endif
