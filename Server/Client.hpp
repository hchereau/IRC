#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
// socket -> connect -> write(give to read of serv) and read(get from write of serv) in cpp -> close

// connect() is only for the client

class Client {

	private:

	public:
	std::string nickname;
	int Client_fd;
};


#endif

// 클라 주소는 os 가 accept 할 때 채워줌
// 서버 주소는 내가 설정
// 클라 주소는 accept 이후에 앎 (목적 상대 식별)
// 서버 주소는 bind 전에 이미 앎 (목적 바인딩)