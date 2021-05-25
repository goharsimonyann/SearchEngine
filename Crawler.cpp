#include <iostream>
#include <stdexcept>
#include <regex.h>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include "easyhttpcpp/EasyHttp.h"
#include "linkQueue.h"
#include "repo_in_memory.hpp"
#include "indexer.hpp"
using namespace std;

std::string GetContents(std::string url)
{
// configure http cache and connection pool instance (optional but recommended)
	easyhttpcpp::EasyHttp::Builder httpClientBuilder;
// create http client
	easyhttpcpp::EasyHttp::Ptr pHttpClient = httpClientBuilder.build();
// create a new request and execute synchronously
	easyhttpcpp::Request::Builder requestBuilder;
	easyhttpcpp::Request::Ptr pRequest = requestBuilder.setUrl(url).build();
	easyhttpcpp::Call::Ptr pCall = pHttpClient->newCall(pRequest);
	easyhttpcpp::Response::Ptr pResponse = pCall->execute();

	if (!pResponse->isSuccessful())
	{
		std::cerr << "HTTP GET Error: (" << pResponse->getCode() << ")" << std::endl;
		return "";
	}
		else
		{
			std::cout << "HTTP GET Success!" << std::endl;
			const std::string contentType = pResponse->getHeaderValue("Content-Type", "");
			if (Poco::isubstr<std::string>(contentType, "text/html") != std::string::npos)
			{
				return pResponse->getBody()->toString();
			}
			return "";
		}
	return url;
}

const int MAX_MATCHES = 10;
linkQueue Linkqueue;
Repo * repo;
Indexer indexer;

void linkAdded(std::string link)
{
	static int count = 0;
	std::cout<<"Crawler:: Link added!! " << link << std::endl;
	if(!( link.find("http")) == 0)
	{
		return;
	}
	//Saves link
	repo->SaveLink(link);    
	std::string contents = GetContents(link);
	
	if(contents == "" || contents.size() < 100)
	{
		return;
	}
	//Saves site
	repo->SaveSite(link, contents);
	//cout << "Contents size : "<<contents.size()<<endl;
	//indexer activation
	indexer.AddSite(link, contents);

	//stop after 50 sites
	if(count++ > 100)		//funkciai kancheri qanaknenq hashvum
	{
		cout << "Stopping crawler!!!!!!!!" << endl;
		return;
	}

	const std::string link_prefix = "href=";
	size_t pos = 0;

	while((pos < contents.size()) && (pos = contents.find(link_prefix, pos + 1)) != std::string::npos)
	{		
		pos += link_prefix.size() +1;
	    char quot = contents[pos -1];
		if(quot != '\'' && quot != '\"')
		{
			continue;
		}

		size_t end_pos = contents.find(quot, pos);

		if(end_pos == std::string::npos)
		{
			break;
		}
		std::string link = contents.substr(pos, end_pos - pos + 1);
		if(!( link.find("http") == 0))
		{
			//std::cout << "Skipping link " << link << std::endl;
			continue;
		}
		//std::cout << "Position = " << pos << "\tQuot = " << quot << "\tLink = " << link << std::endl;
		Linkqueue.addLink(link);
	}
}

void testHandler(std::string link)
{
	std::cout << "Link recieved " << link << std::endl;
}

void testHandler2(std::string link)
{
	std::cout << "Link recieved in test 2" << link << std::endl;
}
int run_server();

int main()
{
	//linkQueue LinkQueue;
	std::cout << "Running crawler " << std::endl;
	/*linkQueue.registerHandler(testHandler);
	linkQueue.registerHandler(testHandler2);
	linkQueue.addLink("https://blablabla");
	linkQueue.addLink("https://asdf asdf");*/
	
	repo = new RepoInMemory();
	Linkqueue.registerHandler(linkAdded);
	linkAdded("https://www.topuniversities.com/student-info/choosing-university/worlds-top-100-universities");
	sleep(2);

/*	auto matches = indexer.GetRelevantURLs("university");

	cout <<"================================="<<endl;

	std::copy(matches.begin(), matches.end(), ostream_iterator<string>(std::cout, ",\n "));*/
	cout<<"Running Server : "<<endl;
	run_server();
	cout<<"Running crawler - Done! "<<endl;

	//Linkqueue.registerHandler(repo->SaveLink);
	return 0;
}
int main_regex()
{
	std::string contents = GetContents("https://www.topuniversities.com/student-info/choosing-university/worlds-top-100-universities");
	//std::string regex = "<a\\s+(?:[^>]*?\\s+)?href=([\"'])(.*?)\\1"; //([-+.[:alnum:]]+://)?([-[:alnum:]]+.)*myURL.net(:[[:digit:]]+)?(/[[:graph:]]*)?"; //;
	std::string regex = "(\\s)*(\t)*Mem([0-9]*) (\\s,\t)*= (\\s,\t)*[0-9]*(.)*[0-9]*" ;
	regex_t re;
	const int MAX_MATCHES = 10;
	if(regcomp(&re, regex.c_str(), REG_EXTENDED) != 0 )
	{
		std::cerr << "Cannot compile regex" << std::endl;
		return 1;
	}
	regmatch_t matches[MAX_MATCHES];
	if(regexec(&re, contents.c_str(), sizeof(matches)/sizeof(matches[0]), (regmatch_t*)&matches,0) == REG_NOMATCH)
	{
		std::cerr << "No links found" << std::endl;
		return 1;
	}
	
	for (int i = 0, offset = 0; i < MAX_MATCHES && offset < contents.size(); ++i)
	{
		std::cout << "Matches" << i << "->" << contents.substr(matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so + 1) << std::endl;
		offset = matches[i].rm_eo + 1;
	}
		//	std::cout << GetContents("shttps://www.google.com/") << std::endl;
	
	return 0;
}




