#include "llhttplus/llhttplus.hpp"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

static char data[] =
"GET /joyent/http-parser.txt HTTP/1.1\r\n"
"Host: github.com\r\n"
"DNT: 1\r\n"
"Accept-Encoding: gzip, deflate, sdch\r\n"
"Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1)"
"AppleWebKit/537.36 (KHTML, like Gecko)"
"Chrome/39.0.2171.65 Safari/537.36\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
"image/webp,*/*;q=0.8\r\n"
"Connection: keep-alive\r\n"
"Transfer-Encoding: chunked\r\n"
"Cache-Control: max-age=0\r\n\r\nb\r\nhello world\r\n0\r\n\r\n";

static char data_no_complete1[] =
"GET /joyent/http-parser.txt HTTP/1.1\r\n"
"Host: github.com\r\n"
"DNT: 1\r\n"
"Accept-Encoding: gzip, deflate, sdch\r\n"
"Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1)"
"AppleWebKit/537.36 (KHTML, like Gecko)"
"Chrome/39.0.2171.65 Safari/537.36\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
"image/webp,";

static char data_no_complete2[] =
"*/*;q=0.8\r\n"
"Connection: keep-alive\r\n"
"Transfer-Encoding: chunked\r\n"
"Cache-Control: max-age=0\r\n\r\nb\r\nhello world\r\n0\r\n\r\n";



int main(int argc, char* argv[])
{
	llhttplus::Parser  parser;
	llhttplus::Request request;

	/* complete message */
	auto rst = parser.execute(&request, data, std::strlen(data));
	if (rst != HPE_OK)
	{
		std::cout << parser.errno_name(rst) << std::endl;
		return 0;
	}

	std::cout << "url:" << request.url << std::endl;
	std::cout << "status:" << request.status << std::endl;

	for (const auto& header : request.headers)
	{
		std::cout << "key:" << header.first << " value:" << header.second << std::endl;
	}

	std::cout << "body:" << request.body << std::endl;
	parser.reset();

	/* not complete message */
	llhttplus::Request requestw;
	rst = parser.execute(&requestw, data_no_complete1, std::strlen(data_no_complete1));
	std::cout << "finish status:" << parser.finish() << std::endl;
	rst = parser.execute(&requestw, data_no_complete2, std::strlen(data_no_complete2));
	std::cout << "finish status:" << parser.finish() << std::endl;
	if (rst != HPE_OK)
	{
		std::cout << parser.errno_name(rst) << std::endl;
		return 0;
	}

	std::cout << "url:" << requestw.url << std::endl;
	std::cout << "status:" << requestw.status << std::endl;

	for (const auto& header : requestw.headers)
	{
		std::cout << "key:" << header.first << " value:" << header.second << std::endl;
	}

	std::cout << "body:" << requestw.body << std::endl;
	
	return 0;
}

