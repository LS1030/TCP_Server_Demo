#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <opencv2/opencv.hpp>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 2333
#define RECEIVE_BUFFER_SIZE (512 * 512 * 3 * 2) //786,432*2=1,572,864 Bytes = 1536 KB = 1.5MB

#define IMAGE_HEIGHT 512
#define IMAGE_WIDTH 512
#define IMAGE_CHANNEL 3
#define IMAGE_SIZE (IMAGE_HEIGHT * IMAGE_WIDTH * IMAGE_CHANNEL)

int main()
{
  setvbuf(stdout, nullptr, _IONBF, 0);

  // server config
  struct sockaddr_in server_address = { 0 };
  server_address.sin_family = AF_INET; // 协议族,IPV4
  server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); // ip地址
  server_address.sin_port = htons(SERVER_PORT); // 端口, host to network，本地字节序转换成网络字节序
  int server_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (bind(server_listen, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
  {
	printf("ERROR: bind listen socket to %s:%d failed!\n", SERVER_ADDRESS, SERVER_PORT);
	return 0;
  }
  if (listen(server_listen, SOMAXCONN) != 0)
  {
	printf("ERROR: listen %s:%d failed!\n", SERVER_ADDRESS, SERVER_PORT);
	return 0;
  }
  printf("INFO: Server is listening %s:%d ...\n", SERVER_ADDRESS, SERVER_PORT);

  // accept client config
  struct sockaddr_in client_address = { 0 };
  socklen_t length = sizeof(client_address);
  //进程阻塞在accept上，成功返回非负描述字，出错返回-1
  int server_accept = accept(server_listen, (struct sockaddr *)&client_address, &length);
  if (server_accept < 0)
  {
	printf("ERROR: Connect Accept Fail!\n");
	return 0;
  }
  printf("INFO: Connect client %s:%d success, Start Receive...\n",
		 inet_ntoa(client_address.sin_addr), client_address.sin_port);

  uint8_t receive_buffer[RECEIVE_BUFFER_SIZE] = { 0 };
  uint64_t receive_count = 0; //all receive data count
  int64_t receive_len = 0; // once receive data len
  uint8_t image[IMAGE_HEIGHT * IMAGE_WIDTH * IMAGE_CHANNEL] = { 0 };
  int8_t receive_status = 0; // 0-Ready to receive, 1-receive success, continue to image process, -1-something wrong in receive process
  uint8_t receive_image_count = 0;

  while (1)
  {
	if (receive_status != 1)
	{
	  receive_count = 0;
	  receive_len = 0;
	  memset(receive_buffer, 0, sizeof(receive_buffer));

	  printf("INFO: Server is receiving...\n");
	  while (receive_count != IMAGE_SIZE)
	  {
		receive_len = recv(server_accept, (receive_buffer + receive_count), sizeof(receive_buffer) - receive_count, 0);
		if (receive_len < 0)
		{
		  printf("ERROR: Receive failed！Receive Restart!\n");
		  receive_count = 0;
		  receive_len = 0;
		  memset(receive_buffer, 0, sizeof(receive_buffer));
		  sleep(1);
		  continue;
		}
		else if (receive_len > 0)
		{
		  printf("DEBUG: Server is received %lu Bytes.\n", receive_len);
		  // printf("DEBUG: %s", (receive_buffer + receive_count));
		  receive_count += receive_len;
		  printf("DEBUG: Server has received %lu Bytes totally...%.2f%%\n", receive_count,
				 100 * ((double)receive_count / IMAGE_SIZE));
		}
		else
		{
		  sleep(1); //waiting next receive
		}

		if (receive_count > sizeof(receive_buffer))
		{
		  printf("ERROR: Receive failed, too more data. Receive Restart!\n");
		  receive_count = 0;
		  receive_len = 0;
		  memset(receive_buffer, 0, sizeof(receive_buffer));
		  sleep(1);
		  continue;
		}
	  }

	  receive_status = 1; //Receive finish
	  memcpy(image, receive_buffer, IMAGE_SIZE);
	  // cv::Mat image_tcp = cv::Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, image);
	  printf("INFO: Receive Finish! Server receive %lu Bytes\n", receive_count);
	}
	else
	{
	  receive_image_count++;
	  printf("INFO: Receive Image %d Success!\n", receive_image_count);
	  printf("INFO: Send image back...\n");
	  send(server_accept, image, sizeof(image), 0);
	  printf("INFO: Send image finish!\n");
	  if (receive_image_count > 10)
	  {
		break;
	  }
	  receive_status = 0; //Image process finish, update status
	}
	sleep(2);
  }

  close(server_accept);
  close(server_listen);

  return 0;
}
