#include <Client.hxx>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Client::Client(tcp::socket socket) : ws_(std::move(socket)) {}

// start handing input-output the client.
void Client::run()
{
	ws_.async_accept(
		[self = shared_from_this()](beast::error_code ec)
		{
			if (ec)
			{
				std::cerr << "WebSocket can't accept new socket!\n";
				return;
			}
			self->input_output_handling();
		}
	);
}

// main logic of data exchange between server and client.
void Client::input_output_handling()
{
	// clear buffer before reading.
	buffer_.consume(buffer_.size());

	ws_.async_read(buffer_, [self = shared_from_this()](beast::error_code ec, size_t received_bytes)
	{
		if (ec)
		{
			std::cout << "Client has been disconnected!\n";
			return;
		}

		self->ws_.text(self->ws_.got_text());
		
		std::string requestString = beast::buffers_to_string(self->buffer_.data());

		try
		{
			json request = json::parse(requestString);

			std::string command = request.at("command");

			if (command == "set-username")
			{
				self->set_username(request.at("username"));
				std::cout << "New username : " << self->get_username() << '\n';
			}
			
		}
		catch(const json::parse_error& e)
		{
			std::cerr << "JSON Parse Error\n" << e.what() << '\n';
		}
		catch(const json::type_error& e)
		{
			std::cerr << "JSON Type Error\n" << e.what() << '\n';
		}
		catch(const json::out_of_range& e)
		{
			std::cerr << "JSON Out of Range Error\n" << e.what() << '\n';
		}

		self->input_output_handling();
	});
}

void Client::set_username(const std::string& username)
{
	username_ = username;
}

std::string Client::get_username() const
{
	return username_;
}
