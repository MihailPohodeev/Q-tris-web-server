#include <Client.hxx>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Client::Client(tcp::socket socket) : ws_(std::move(socket)) {}

// send data to client.
void Client::send_data(const std::string& data)
{
	ws_.async_write(asio::buffer(data), [](beast::error_code ec, size_t bytes_send)
	{
		if (ec)
			std::cerr << "Can't send data to client!\n";
	});
}

// start handing input-output the client.
void Client::run(std::function<void(std::shared_ptr<json>)> callback)
{
	ws_.async_accept(
		[self = shared_from_this(), callback](beast::error_code ec)
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

void Client::set_username(const std::string& username)
{
	username_ = username;
}

std::string Client::get_username() const
{
	return username_;
}
