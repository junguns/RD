#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT_NUMBER 5000
#define SERVER_ADDRESS "192.9.85.251"
#define FILENAME "/home/RD/a.png"

#define DEBUG_PRINT
#define BUF_SIZE	4096

using namespace cv;

int main(int argc, char** argv )
{
	int server_socket;
	ssize_t len;
	struct sockaddr_in remote_addr;
	char buffer[BUF_SIZE];
	int file_size;
	FILE *received_file;
	int remain_data = 0;
	Mat image;
	int temp = 0;

	/* Zeroing remote_addr struct */
	memset(&remote_addr, 0, sizeof(remote_addr));

	/* Construct remote_addr struct */
	remote_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr));
	remote_addr.sin_port = htons(PORT_NUMBER);

	/* Create server socket */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}


	/* Connect to the client */
	if (connect(server_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}


	while(1){
		do {
			/* Receiving Marker*/
			recv(server_socket, buffer, 256, 0);
			file_size = atoi(buffer);
		}while(file_size != 999999);
		file_size = 0;
		memset(buffer, 0, sizeof(buffer));

		/* Receiving file size */
		recv(server_socket, buffer, 256, 0);
		file_size = atoi(buffer);
		fprintf(stdout, "\nFile size : %d\n", file_size);
		memset(buffer, 0, sizeof(buffer));
		received_file = fopen(FILENAME, "w");
		if (received_file == NULL)
		{
			fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

			exit(EXIT_FAILURE);
		}

		remain_data = file_size;
		temp = BUF_SIZE;

		while (((len = recv(server_socket, buffer, temp, 0)) > 0) && (remain_data > 0))
		{
			fwrite(buffer, sizeof(char), len, received_file);
			remain_data -= len;
			if ( remain_data > BUF_SIZE )
				temp = BUF_SIZE;
			else
				temp = remain_data;
#ifdef DEBUG_PRINT
			fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
#endif
		}
		fflush(received_file);
		fclose(received_file);


		image = imread( FILENAME, 1 );
		if ( !image.data )
		{
			printf("No image data \n");
			return -1;
		}
		namedWindow("Display Image", WINDOW_AUTOSIZE );
		imshow("Display Image", image);
		waitKey(10);
	}
	close(server_socket);

	return 0;
}


