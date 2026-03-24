1) network / server core 2) irc protocol / parsing 3) irc exec

1) server / socket / poll 2) parser command dispatcher 3) channel user commands 
//


1) 내 할 일 Network / Server Core / server / socket / poll
: 연결을 만들고 유지하는 레벨
서버 소켓 생성(socket, bind, listen)
클라이언트 접속 처리 (accept)
여러 클라 동시 처리 (poll, select, epoll)
클라 연결 상태 관리
데이터 송수신(recv, send)
-> 누가 접속했는지 관리, 데이터가 들어왔는지 감지, 아직 메세지 의미는 모름(raw string)
--> 네트워크 입출력 담당(OSI 4~5 느낌)

2) irc protocol, parsing / parser command dispatcher
: 문자열을 IRC 명령어로 해석하는 레벨
NICK USER JOIN PRIVMSG 같은 명령 파싱
메시지 포맷 분석
:nick!user@host PRIVMSG #chan :hello
토큰화 / 구조화
어떤 명령인지 판단
해당 핸들러로 전달 (dispatcher)
-> raw text->structured command 반환, 문법 오류 체크
--> 문자열을 의미 있는 명령으로 변환

3)IRC Exec (Business Logic) / channel user commands
: 실제 IRC 동작을 수행하는 레벨
유저 생성 / 삭제 / 상태 관리
채널 생성 및 관리
명령 실행
- JOIN 채널 입장 처리
- PRIVMSG 메시지 전달
- QUIT 연결 종료 처리
권한 관리 (op, voice 등)
-> 무슨 일이 일어나야 하는가를 결정, 실제 상태 변화 발생
--> 명령을 실행해서 서버 상태를 바꾸는 곳


흐름
클라이언트가 -> 네트워크 코어 (raw string) -> 파서 프로토콜(structured command) -> Exec/로직(response)->다시 네트워크 코어가 : send

클래스가 나뉘는 건 구현 분리이고, 팀플엣 중요한건 인터페이스임


// sockets tutorial : https://www.linuxhowtos.org/C_C++/socket.htm


합의 시 :
[Network]
  - recv → Client.buffer에 누적
  - \r\n 기준으로 메시지 분리

→ [Parser]
  - Message 구조로 변환

→ [Exec]
  - 상태 변경

경계 설계가 필요햐다 :
데이터 형태 통일하기 - 파서가 뭘 반환하는지 통일
클라 모델 - 버퍼를 누가 들고 있느냐
흐름 책임 - 누가 메시지를 끊어주는가? 누가 클라를 생성하는가? 누가 디스커낵트 처리를 하는가?

반드시 합의할 것 :
메시지 단위 (\r\n 처리)
클라 구조
커맨드 구조
에러 처리 방식
함수 인터페이스

//
객체의 캡슐화 / 역할 나누기
Server 객체가 Client 와 Channel 객체 내부 데이터를 직접 다루는 등.. 은 의존도가 높아 다른 객체 조작하는 부분을 찾아야 함

Server - 네트워크 관련 부분 처리
Client - 실제 클라 대응해 관련된 데이터 관리, 받은 메세지 파싱 보낼 메세지 저장 등
Channel - 클라 프로그램의 채널에 대응되는 객체 채널과 관련된 책임들을 수행
Command - irc 프로토콜 메시의 명령어 로직 처리 역할 담당
Protocol - 클라에게 보낼 irc 프로토콜 메시지 만드는 역할 담당

인터페이스 정의하고 할당하기
: 다른 객체에게 요청을 해야할 때 필요한 인터페이스 정의, 기능을 수행하기에 

필요한 인터페이스 정의 -> 적절한 객체에 할당 -> 멤버함수 구현 ->필요한 인터..

//
//

// https://velog.io/@yeshyungseok/C%EB%A1%9C-IRC-%EA%B5%AC%ED%98%84%ED%95%98%EA%B8%B0-%EC%99%84
// 플로우 차트 참고하기

//

// You must not develop an IRC client. 
// : IRC 서버만 만들면 되구 채팅 프로그램은 만들 필요 없으니 있는 클라이언트 쓰기 nc telent irssi 등
// ex - nc localhost 6667 머 이런식으로..
// You must not implement server-to-server communication.
// : IRC 는 원래 서버 여러개가 연결된 네트워크 구조
// 해당 과제는 단일 서버로 이용하겠다 한 서버 안에서만 유저 관리 채널 관리 메시지 전달


// vector 는 순서 기반 데이터 관리, 짭 배열 / map 은 키 식별자 기반 관리
// 98 백터 

/*
vector<int> v;
vector<int> v(4);
v.assign(5, 10);
v.begin();
v.end();
v.rbegin();
v.rend();

vector<int> v;
vector<int> v(4);

vector<int> v2;
v2.push_back(1);
v2.push_back(2);
v2.push_back(3);

vector<vector<int> > vv;


//

std::map<Key, Value> 
std::map<int, int> client_ids;
cleint_ids[fd] = id; 하면 -> 없으면 새로 생성, 있으면 값 덮어쓰기(업뎃)
int id = client_ids[fd];
없는 키 접근하면 자동 생성됨 값 0으로 이닛 되어서
if (client_ids.find(fd) != client_ids.end())
	int id = client_ids[fd];

삭제 :
client_ids.erase(fd);
적다가 맒
*/