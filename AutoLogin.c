#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <curl/curl.h>
#include<windows.h>
#define PATH1 "user.ini"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

char url[600];

int getipv4(char *);
int generate_url(char *, char *, char *, char *, char *, char *);
int read_ini();
int login();
int createini();
int confirm();
HANDLE hout;
void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}


struct user
{
	char yys[60];
	char user_account[60];
	char user_password[60];
}user;

int main(void)
{
	char a;
	int b = 0;
	char ip_address[INET6_ADDRSTRLEN];
	char u1[100] = { "http://172.21.255.105:801/eportal/?c=Portal&a=login&callback=dr1004&login_method=1&user_account=" };
	char u2[100] = { "%40" };
	char u3[100] = { "&user_password=" };
	char u4[100] = { "&wlan_user_ip=" };
	char u5[100] = { "&wlan_user_ipv6=&wlan_user_mac=000000000000&wlan_ac_ip=&wlan_ac_name=&jsVersion=3.3.3&v=2621" };
	printf("欢迎使用");
	color(11);
	printf("ECUT_AutoLogin_Beta1.0");
	color(15);
	printf("!\n此程序可以用来登录东华理工大学校园网，如果想实现开机自动登录，请自行添加到启动项\n\n");
	printf("目前这个程序还在内测阶段，如果遇到问题可以联系我！");
	color(14);
	printf("\n\n作者：Seeker\nQQ:1619356139\n邮箱：1619356139@qq.com\n\n");
	color(15);
	getipv4(ip_address);
	while (-1 == read_ini())
	{
		color(12);
		printf("配置文件加载失败！\n");
		color(15);
		printf("无法打开user.ini，这可能是因为你是第一次运行此程序或者不小心修改或删除了user.ini\n\n请输入相关信息，以便程序自动生成配置文件.\n\n");
		re:
		createini();
		read_ini();
		color(10);
		printf("运营商：");
		for (b = 0; (a = user.yys[b]) != '\0'; b++)
			putchar(a);
		printf("\n账号：");
		for (b = 0; (a = user.user_account[b]) != '\0'; b++)
			putchar(a);
		printf("\n密码：");
		for (b = 0; (a = user.user_password[b]) != '\0'; b++)
			putchar(a);
		color(15);
		printf("\n以上信息是否正确？按y确认，按任意键重新输入");
		getchar();
		if (getchar() != 'y')
			goto re;

	}
	generate_url(u1, u2, u3, u4,u5, ip_address);
	login();
}

int getipv4(char *ip_address)
{
	
	// 初始化Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("初始化Winsock失败\n");
		return 1;
	}

	// 获取接口信息
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;

	// 第一次调用GetAdaptersAddresses获取需要的缓冲区大小
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
	if (dwRetVal != ERROR_BUFFER_OVERFLOW) {
		printf("无法获取适配器信息\n");
		return 1;
	}

	pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
	if (pAddresses == NULL) {
		printf("分配内存失败\n");
		return 1;
	}

	// 第二次调用GetAdaptersAddresses获取接口信息
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
	if (dwRetVal != NO_ERROR) {
		printf("获取适配器信息失败\n");
		free(pAddresses);
		return 1;
	}

	// 遍历接口信息并找到所有WLAN接口
	PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
	int numWlanInterfaces = 0;
	while (pCurrAddresses) {
		if (pCurrAddresses->IfType == IF_TYPE_IEEE80211 && pCurrAddresses->OperStatus == IfOperStatusUp) {
			// 获取IP地址
			IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
			while (pUnicast) {
				if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
					struct sockaddr_in* pIPV4Addr = (struct sockaddr_in*) pUnicast->Address.lpSockaddr;
					inet_ntop(AF_INET, &pIPV4Addr->sin_addr, ip_address, INET6_ADDRSTRLEN);
					printf("你的内网ipv4地址是: ");
					color(10);
					printf("%s\n", ip_address);
					color(15);
					numWlanInterfaces++;
				}
				pUnicast = pUnicast->Next;
			}
		}
		pCurrAddresses = pCurrAddresses->Next;
	}

	if (numWlanInterfaces == 0) {
		printf("没有找到有效WLAN连接，请连接上wifi后再启动程序.\n");
	}

	// 释放资源
	if (pAddresses != NULL) {
		free(pAddresses);
	}
	WSACleanup();

	//getchar();
	//getchar();

	color(15);

	return 1;
}

int read_ini()
{
	char ch;
	char check[200];
	char check1[200] = {"#CONFIGURATION_FILES_FOR_AUTOLOGIN_V1.0\n"};
	char read;
	FILE *fp = NULL;
	int a = 0, b = 0;
	for (b = 0; b <= 59; b++)
	{
		user.user_account[b] = '\0';
		user.user_password[b] = '\0';
		user.yys[b] = '\0';
	}
	fp = fopen(PATH1, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//ch = getc(fp);
	if ((ch=getc(fp)) != '#')
	{
		fclose(fp);
		return -1;
	}
	while ((check[a] = getc(fp)) != '\n')
		a++;
	for (a = 0; a <= 38; a++)
	{
		//printf("%d",a);
		if (check[a] != check1[a])
		{
			fclose(fp);
			return -1;
		}
	}
	while (getc(fp) != '!')
		continue;
	for (int a = 0; (read = getc(fp)) != '\n'; a++)
	{
		user.yys[a] = read;
	}
	
	while (getc(fp) != '@')
		continue;
	for (int a = 0; (read = getc(fp)) != '\n'; a++)
	{
		user.user_account[a] = read;
	}
	while (getc(fp) != '$')
		continue;
	for (int a = 0; (read = getc(fp)) != '\n'; a++)
	{
		user.user_password[a] = read;
	}

	fclose(fp);

	return 1;

}

int generate_url(char *u1, char *u2, char *u3, char *u4, char *u5, char *ipv4)
{
	int a = 0, b = 0;
	for (b = 0; (url[a] = *(u1 + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(user.user_account + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(u2 + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(user.yys + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(u3 + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(user.user_password + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(u4 + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(ipv4 + b)) != '\0'; a++, b++);
	for (b = 0; (url[a] = *(u5 + b)) != '\0'; a++, b++);

	return 1;
}

int login()
{
	CURL *curl;
	CURLcode res;

	color(13);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			fprintf(stderr, "GET请求发送失败： %s\n", curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	return 0;
}

int createini()
{
	
	char aa[60];
	FILE *fp = NULL;
	fp = fopen(PATH1, "w");
	printf("请输入运营商（如果你的运营商是中国移动，则请输入cmcc，其他的我暂时也不知道（^-^））然后按回车\n");
	color(5);
	printf("PS.电信可能是telecom，联通可能是unicom，不过我暂时没测试过\n");
	color(15);
	scanf("%s", aa);
	fprintf(fp, "##CONFIGURATION_FILES_FOR_AUTOLOGIN_V1.0\n");
	putc('!', fp);
	fprintf(fp, "%s\n", aa);
	printf("请先输入账号后按回车\n");
	scanf("%s", aa);
	putc('@', fp);
	fprintf(fp, "%s\n", aa);
	printf("请输入密码后按回车\n");
	scanf("%s", aa);
	putc('$', fp);
	fprintf(fp, "%s\n", aa);

	fclose(fp);
	
	return 0;
}

