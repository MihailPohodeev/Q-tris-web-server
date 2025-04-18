#include <Client.hxx>
#include <Room.hxx>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Client::Client(tcp::socket socket) : ws_(std::move(socket)), isWriting(false) {}

// send data to client.
void Client::send_data(const std::string& data)
{
	messagesQueue_.push(data);
	_start_data_sending_();
}

// send data from messagesQueue_.
void Client::_start_data_sending_()
{
	if ( messagesQueue_.empty() || isWriting )
		return;

	isWriting = true;
	std::cout << "SENDING DATA : " << messagesQueue_.front() << '\n';
	ws_.async_write(asio::buffer( messagesQueue_.front() ), [self = shared_from_this()](beast::error_code ec, size_t bytes_send)
		{
			if (ec)
			{
				std::cerr << "Can't send data to client!\n";
				return;
			}
			self->messagesQueue_.pop();
			self->isWriting = false;
			self->_start_data_sending_();
		});
}

// start handing input-output the client.
void Client::run(std::function<void(std::shared_ptr<json>)> callback)
{
	ws_.async_accept( [self = shared_from_this(), callback](beast::error_code ec)
		{
			if (ec)
			{
				std::cerr << "WebSocket can't accept new socket!\n";
				return;
			}
			self->input_output_handling(callback);
		}
	);
}

// main logic of data exchange between server and client.
void Client::input_output_handling(std::function<void(std::shared_ptr<json>)> callback)
{
	// clear buffer before reading.
	buffer_.consume(buffer_.size());

	ws_.async_read(buffer_, [self = shared_from_this(), callback](beast::error_code ec, size_t received_bytes)
	{
		if (ec)
		{
			std::cout << "Client has been disconnected!\n";
			std::shared_ptr<Room> room_ptr = self->get_room();
			if ( room_ptr.get() )
			{
				room_ptr->disconnect_client(self);
				std::shared_ptr<json> newListOfClients = std::make_shared<json>();
				(*newListOfClients)["command"] = "room_clients_info";
				(*newListOfClients)["clients"] = *( room_ptr->get_clients_info() );
				(*newListOfClients)["status"]  = "success";
				room_ptr->notify_all(newListOfClients);
				self->set_room(nullptr);
			}
			return;
		}

		self->ws_.text(self->ws_.got_text());
		
		std::string requestString = beast::buffers_to_string(self->buffer_.data());

		try
		{
			std::shared_ptr<json> request = std::make_shared<json>( json::parse(requestString) );
			callback(request);
		}
		catch(const json::parse_error& e)
		{
			std::cerr << "JSON Parse Error\n" << e.what() << '\n';
		}

		self->input_output_handling(callback);
	});
}

// set room for handling game process.
void Client::set_room(std::shared_ptr<Room> room)
{
	room_ = room;
}

// return current client's room.
std::shared_ptr<Room> Client::get_room() const
{
	return room_;
}

// setter for ID_.
void Client::set_ID(int id)
{
	ID_ = id;
}

int Client::get_ID() const
{
	return ID_;
}

void Client::set_username(const std::string& username)
{
	username_ = username;
}

std::string Client::get_username() const
{
	return username_;
}
