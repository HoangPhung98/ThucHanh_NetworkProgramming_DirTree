#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <fileapi.h>

WSADATA wsa;
SOCKET server;
SOCKADDR_IN serveraddr, clientaddr;

int main()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serveraddr.sin_addr.s_addr = htonl(ADDR_ANY);
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_family = AF_INET;
	bind(server, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	listen(server, 5);

	char buff[1024], request[5], link[128], http11[6];
	int clientaddr_length = sizeof(clientaddr);
	int ret = 0;
	while (true) {
		SOCKET client = accept(server, NULL, NULL);
		printf("New client accepted: %d\n", client);
		ret = recv(client, buff, sizeof(buff), 0);
		if (ret > 0) {
			buff[ret] = 0;
			sscanf(buff, "%s %s %s", request, link, http11);
			char  dir[128] = "C:";
			char dir_file[128] = "C:";
			// hanlde request
			if (strcmp(link, "/") == 0) {
				printf("%s", buff);
				//convert 
				for (int i = 0; i < strlen(link); i++) {
					if(link[i]=='/') link[i]='\\';
				}
				// send list dir and file
				
				char html[2048] = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\n\n\r\n\r\n<html><body>";
				WIN32_FIND_DATAA DATA;
				HANDLE h = FindFirstFileA("C:\\*.*", &DATA);
				do {
					if ((DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (strcmp(DATA.cFileName,".")!=0)) {
						strcat(html, "<a href=\"");
						strcat(html, DATA.cFileName);
						strcat(html, "\">");
						strcat(html, DATA.cFileName);
						strcat(html, "</a></br>");
					}
					else {
						INT64 size = (DATA.nFileSizeHigh * (MAXDWORD + 1)) + DATA.nFileSizeLow;
						strcat(html, "<a href=\"");
						strcat(html, DATA.cFileName);
						strcat(html, "\"><i>");
						strcat(html, DATA.cFileName);
						strcat(html, "<i/></a></br>");
					}
				} while (FindNextFileA(h, &DATA));
				strcat(html, "</body></html>");
				send(client, html, strlen(html), 0);

			}
			else if (strcmp(link,"/favicon.ico")!=0){
				printf("%s", buff);
				char suyet[4] = "\\";
				char link2[128];
				int i = 0;
				while(i<strlen(link)){
					if (link[i] == '/') {
						strncpy(link2, link, i);
						link2[i] = '\0';
						strcat(link2, suyet);
						strcat(link2, link + i + 1);
						strcpy(link, link2);
					}
					i++;
				}
				i = 0;
				while (i < strlen(link)) {
					if (link[i] == '%' && link[i+1] == '2' && link[i+2] == '0') {
						strncpy(link2, link, i);
						link2[i] = '\0';
						strcat(link2, " ");
						strcat(link2, link + i + 3);
						strcpy(link, link2);
					}
					i++;
				}
				strcat(dir, link2);
				strcat(dir_file, link2);
				printf("***********link: %s\n", link2);
				printf("***********dir: %s\n", dir);

				strcat(dir, "\\*.*");
				char html[INT16_MAX] = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\n\n\r\n\r\n<html><body>";
				WIN32_FIND_DATAA DATA;
				HANDLE h = FindFirstFileA(dir, &DATA);
				int num_of_file = 0;
			
				while (FindNextFileA(h, &DATA)) {
						if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							num_of_file++;
							if (strcmp(DATA.cFileName, ".") == 0) {
								strcat(html, "<a href=\"");
								strcat(html, link2);
								strcat(html, "\"\>");
								strcat(html, DATA.cFileName);
								strcat(html, "</a></br>");
							}
							else if (strcmp(DATA.cFileName, "..") == 0) {
								strcat(html, "<a href=\"");
								//return link2 befor
								char linkBefor[128];
								strcpy(linkBefor, link2);
								int i = strlen(linkBefor) - 1;
								while (i >= 0) {
									if (linkBefor[i] == '\\') {
										if (i != 0) linkBefor[i] = '\0';
										else linkBefor[1] = '\0';
										break;
									}
									i--;
								}
								strcat(html, linkBefor);
								strcat(html, "\"\>");
								strcat(html, DATA.cFileName);
								strcat(html, "</a></br>");
							}
							else {
								strcat(html, "<a href=\"");
								strcat(html, link2);
								strcat(html, "\\");
								strcat(html, DATA.cFileName);
								strcat(html, "\"\>");
								strcat(html, DATA.cFileName);
								strcat(html, "</a></br>");
							}
						}
						else {
							num_of_file++;
							INT64 size = (DATA.nFileSizeHigh * (MAXDWORD + 1)) + DATA.nFileSizeLow;
							strcat(html, "<a href=\"");
							strcat(html, link2);
							strcat(html, "\\");
							strcat(html, DATA.cFileName);
							strcat(html, "\"\><i>");
							strcat(html, DATA.cFileName);
							strcat(html, "<i/></a></br>");
						}
				}
				if (num_of_file == 0) {
					FILE* f = fopen(dir_file, "r");
					char buf[1024];
					int ret;
					while ((ret = fread(buf,1, 1024,f)) > 0) {
						if (ret < 1024) buf[ret] = 0;
						strcat(html, buf);
					}
					fclose(f);
				}
				strcat(html, "</body></html>");
				send(client, html, strlen(html), 0);
			}
		}
		else continue;
		closesocket(client);
	}
	
	closesocket(server);
	WSACleanup();
}
