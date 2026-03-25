// #ifndef STPOLL_HPP
// #define STPOLL_HPP

// struct stPoll
// {
// 	// ? poll list
// 	// ? poll delete list

// 	int 
// 	short events; // 발생된 이벤트
// 	short renvents; // 돌려 받은 이벤트
// };

// #endif

// // poll 은 변수 하나 외에도 배열 받을 수 있음
// // 링크드 리스트 쓸까?
// // poll(&vec[0], vec.size(), timeout);
// // size_t size = 현재 원소 개수
// // struct stdPoll p_fds[N]; // 연속된 배열 크기 N
// // std::vector<pollfd> pfds; 폴 용 진짜 배열 data() 로 poll 에 바로 전달되고
// // delete list 에 모아서 loop 끝나고 erase 혹은 swap-pop

// using on pollfd, if needed re write.. 