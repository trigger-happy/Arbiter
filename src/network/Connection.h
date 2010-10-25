#ifndef CONNECTION_H
#define CONNECTION_H

#define HEADER_LENGTH 8

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include "Message.pb.h"

class Connection
{
public:
  Connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }
  
  virtual ~Connection()
  {
  }
  
  boost::asio::ip::tcp::socket& socket()
  {
    return socket_;
  }
  
  template <typename Handler>
  void async_write(const NetworkMessage& message, Handler handler)
  {
    // Serialize the data first so we know how large it is.
    message.SerializeToString(&outbound_data_);

    // Format the header.
    std::ostringstream header_stream;
    header_stream << std::setw(HEADER_LENGTH)
      << std::hex << outbound_data_.size();
    if (!header_stream || header_stream.str().size() != HEADER_LENGTH)
    {
      // Something went wrong, inform the caller.
      boost::system::error_code error(boost::asio::error::invalid_argument);
      socket_.io_service().post(boost::bind(handler, error));
      return;
    }
    outbound_header_ = header_stream.str();

    // Write the serialized data to the socket. We use "gather-write" to send
    // both the header and the data in a single write operation.
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_header_));
    buffers.push_back(boost::asio::buffer(outbound_data_));
    boost::asio::async_write(socket_, buffers, handler);
  }
  
  template <typename Handler>
  void async_read(NetworkMessage& message, Handler handler)
  {
    // Issue a read operation to read exactly the number of bytes in a header.
    void (Connection::*f)(
	const boost::system::error_code&,
	NetworkMessage&, boost::tuple<Handler>)
      = &Connection::handle_read_header<Handler>;
    boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
	boost::bind(f,
	  this, boost::asio::placeholders::error, boost::ref(message),
	  boost::make_tuple(handler)));
  }
  
  template <typename Handler>
  void handle_read_header(const boost::system::error_code& e,
      NetworkMessage& message, boost::tuple<Handler> handler)
  {
    if (e)
    {
      boost::get<0>(handler)(e);
    }
    else
    {
      // Determine the length of the serialized data.
      std::istringstream is(std::string(inbound_header_, HEADER_LENGTH));
      std::size_t inbound_data_size = 0;
      if (!(is >> std::hex >> inbound_data_size))
      {
	// Header doesn't seem to be valid. Inform the caller.
	boost::system::error_code error(boost::asio::error::invalid_argument);
	boost::get<0>(handler)(error);
	return;
      }

      // Start an asynchronous call to receive the data.
      inbound_data_.resize(inbound_data_size);
      void (Connection::*f)(
	  const boost::system::error_code&,
	  NetworkMessage&, boost::tuple<Handler>)
	= &Connection::handle_read_data<Handler>;
      boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
	boost::bind(f, this,
	  boost::asio::placeholders::error, boost::ref(message), handler));
    }
  }
  
  template <typename Handler>
  void handle_read_data(const boost::system::error_code& e,
      NetworkMessage& message, boost::tuple<Handler> handler)
  {
    if (e)
    {
      boost::get<0>(handler)(e);
    }
    else
    {
      // Extract the data structure from the data just received.
      try
      {
	std::string archive_data(&inbound_data_[0], inbound_data_.size());
	message.ParseFromString(archive_data);
      }
      catch (std::exception& e)
      {
	// Unable to decode data.
	boost::system::error_code error(boost::asio::error::invalid_argument);
	boost::get<0>(handler)(error);
	return;
      }

      // Inform caller that data has been received ok.
      boost::get<0>(handler)(e);
    }
  }
  
private:
  /// The actual socket
  boost::asio::ip::tcp::socket socket_;
  
  /// Holds an outbound header.
  std::string outbound_header_;

  /// Holds the outbound data.
  std::string outbound_data_;

  /// Holds an inbound header.
  char inbound_header_[HEADER_LENGTH];

  /// Holds the inbound data.
  std::vector<char> inbound_data_;
};

typedef boost::shared_ptr<Connection> connection_ptr;

#endif // CONNECTION_H
