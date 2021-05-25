#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

class Indexer
{
public:
    void AddSite(string url, string contents);
    vector<string> GetRelevantURLs(string word, uint count = 10);
private:
    //word -> (relevance, URL)
    map<string, map<float, string>> _index;

};