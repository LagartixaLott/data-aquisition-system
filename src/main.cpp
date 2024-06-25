#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
bool id_novo(const std::string& id){
}
bool log(const std::string& message){
}
bool get(const std::string& message){
}
class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void start()
  {
    read_message();
  }

private:
  void read_message()
  {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, "\r\n",
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::istream is(&buffer_);
            std::string message(std::istreambuf_iterator<char>(is), {});
           // std::cout << "Received: " << message << std::endl;
           // write_message(message);
           std::string id=message.substr(2,3);
              
           if(log(message)){
            if(!id_novo(id)){
            //Escrita no arquivo
            }
            if(id_novo(id)){
            //Cria arquivo para o novo sensor
            //Escrita no arquivo
            }
           }
           if(get(message)){
            if(!id_novo(id)){//Leitura do arquivo
            //Envio da informação para o cliente
            }
            
            if(id_novo(id)){
            //Envio de mensagem de erro para o cliente
            }
           }
          
          }
      
        });
  }

  void write_message(const std::string& message)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [this, self, message](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            read_message();
          }
        });
  }

  tcp::socket socket_;
  boost::asio::streambuf buffer_;
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    accept();
  }

private:
  void accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket))->start();
          }

          accept();
        });
  }

  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: chat_server <port>\n";
    return 1;
  }

  boost::asio::io_context io_context;

  server s(io_context, std::atoi(argv[1]));

  io_context.run();

  return 0;
}
