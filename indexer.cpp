#include "indexer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

void Indexer::AddSite(string url, string contents)
{
    cout << "Indexing : " << url << endl;
    //split the contents into word
    map<string, uint> word_counts;
    stringstream str(contents);
    string word;
    //get word accurance count
    while(str >> word)
    {
        if(word_counts.find(word) == word_counts.end())
        {
            word_counts[word] = 0;
        }
        else
        {
             word_counts[word]++; // imacanq inchqan angama bare handipum
        }
    } 
    //store in indexer
    for(auto i = word_counts.begin(); i != word_counts.end(); ++i)
    {
        auto word = i->first;
        auto count = i->second;
            pthread_mutex_lock(&_mutex);
            _index[word][count] = url;
             pthread_mutex_unlock(&_mutex);
    }
}

vector<string> Indexer::GetRelevantURLs(string word, uint count)
{
    //checking if the word is present
    vector<string> matches;
    int i = 0;
    for(auto iter = _index[word].begin(); iter != _index[word].end() && i < count; ++i, ++iter)
    {
        matches.push_back(iter->second);
    }
    return matches;
}
