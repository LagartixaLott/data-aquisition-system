#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <vector>
#include <fstream>

using boost::asio::ip::tcp;
std::vector<int> ids;
std::vector<std::string> split_string(std::string message, const char* op){

}
bool id_novo(std::string id){
  for(int i=0;i<ids.size();i++){
    if(stoi(id)==ids[i]){
      return true;
    }
  }
  return false;
}
void write_in_file(std::string id,std::string message){

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
            std::vector<std::string> new_message=split_string(message,"|");
           std::string msg_type=new_message[1];
           std::string id=new_message[2];
           if(msg_type=="LOG"){
            if(!id_novo(id)){
            //Escrita no arquivo
            write_in_file(id,message);
            }
            if(id_novo(id)){
            //Cria arquivo para o novo sensor
            //Escrita no arquivo
            std::string filename = id + ".txt";
            std::fstream output_fstream;
            output_fstream.open(filename, std::ios_base::out);
            write_in_file(id,message);
            }
          }
           if(msg_type=="GET"){
            if(!id_novo(id)){
            //Leitura do arquivo
            //Envio da informação para o cliente
            std::string aux=new_message[3];
            int n_infos =stoi(aux.substr(0,aux.size()-4));
            std::string filename = id + ".txt";
            std::fstream file(filename, std::fstream::out | std::fstream::in | std::fstream::binary 
																	 | std::fstream::app); 
          	// Caso não ocorram erros na abertura do arquivo
	          if (file.is_open()){
            int file_size = file.tellg();
            int n = file_size/sizeof(message);
            //Conferir se o número de registros é maior ou menor que o requisitado
            if(n_infos > n){
              std::string rec = std::to_string(n) + ";";
              std::string aux_1;
              while (file.read(reinterpret_cast<char*>(&aux_1), sizeof(file))) {
                //adiciona o registro lido ao string
                rec = rec + aux_1 + ";";
               }
               file.close();
               write_message(rec);

            }
            if(n_infos <= n){

            }
            }
            }
            
            if(id_novo(id)){
            //Envio de mensagem de erro para o cliente
            std::string error = "ERROR|INVALID_SENSOR_" + id + "\r\n";
            write_message(error);
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
