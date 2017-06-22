// Compile - 
// c++ searchText.cpp porter2_stemmer.cpp -I /usr.local/boost_1_59_0 -lboost_system -lboost_filesystem
// Run - 
// ./a.out ./Dataset/ OR ./a.out /path/to/your/dataset




#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include "porter2_stemmer.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <math.h>
#include <algorithm>

#define k 10

using namespace std;

struct docIdScoreMap{
	int docId;
	double score;
};

bool myfunc(struct docIdScoreMap &lhs, struct docIdScoreMap &rhs){
	return lhs.score > rhs.score;
}
// custom comparision function to sort array of structures

int main(int argc, char *argv[]){

	int numDocuments;

	map<string, vector<pair<int,double> > > termPostingListsMapping;
	// termPostingLists stores the posting lists for all the terms in the corpus.

	map<int, pair<string, string> > docIdTitleMapping;
	// docIdTitleMapping stores the title and location of the documents corresponding to a docID.

	map<string, int> termFrequencyMapping;
	// stores the freequency of each term in a document

	unordered_map<int, int> docLength;
	// to strore the length of the documents

	boost::filesystem::path path(argv[1]);
    boost::filesystem::recursive_directory_iterator itr(path);

    int assignDocId = 0;
    
    string title, fileName, token;
    while (itr != boost::filesystem::recursive_directory_iterator()){

    	title.clear();
    	termFrequencyMapping.clear();

        fileName = itr->path().string();
        // the path to the file is stored as a string in the variable fileName

        if (boost::ends_with(fileName, ".txt")){
        	// checks if the path refers to a valid corpus file(since all the files in corpus are text files)
	        
	        ifstream ifs(fileName);
	        getline(ifs, title);
	        string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	        // stores the title and content of the file in the string variables after setting up a input stream

	        docLength[assignDocId] = title.length() + content.length();
	        // stores the length of the documents for CosineScore algorithm

	        boost::tokenizer<> tok(title);
	        for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
	   			token = *beg;
	   			boost::algorithm::to_lower(token);
	   			// case folding to lower case
	   			
	   			Porter2Stemmer::stem(token);
	   			// performs stemming on the Token as per Porter's Algorithm

	   			termFrequencyMapping[token]++;
	   		}
	   		//tokenizes and stores the freequency of the tokens in the title of the file

	   		boost::tokenizer<> tok_c(content);
	   		for(boost::tokenizer<>::iterator beg=tok_c.begin(); beg!=tok_c.end();++beg){
	       		token = *beg;
	       		boost::algorithm::to_lower(token);
	       		//case folding to lower case
	       		
	       		Porter2Stemmer::stem(token);
	   			// performs stemming on the Token as per Porter's Algorithm

	       		termFrequencyMapping[token]++;
	   		}
	   		// tokenizes and stores the freequency of the tokens in the content of the file

	        docIdTitleMapping[assignDocId].first = title;
	        docIdTitleMapping[assignDocId].second = fileName;
	        // assigns docId to the documents sequentially and storing their titles in the map

	        for(map<string, int>::iterator it = termFrequencyMapping.begin(); it!= termFrequencyMapping.end(); it++){
	        	termPostingListsMapping[it->first].push_back(make_pair(assignDocId, (log10(1+ it->second))));
	        	// pushes the corresponding pair of (docId,tf) to posting list of all terms in the given document
	    	}
	        
	        assignDocId++;
	        // increments docId to assign a new docId to the next document
	    }
        ++itr;
    }
    //Index construction and preprocessing is done and the results are stored in the hash_map data-structure

    numDocuments = assignDocId;
    struct docIdScoreMap scores[numDocuments];
    
    map<string, int> queryTermFreequency;
    while(1){
    	queryTermFreequency.clear();
    	// loop infinetely for the query
		
		int flag = 0;
		cout << endl << "Enter the search query (or q to quit)" << endl << endl;
		string query;
		getline(cin, query);
		// inputs the query and stores it in the string variable query

		if(query == "q")
			return 1;
		// return if user wants to quit

		for(int i=0; i<numDocuments; i++){
			scores[i].docId = i;
			scores[i].score = 0.0;
		}
		// initialize the structure for each query

		boost::tokenizer<> tok(query);
		// query is broken into tokens in exactly the same way as the documents were broken during indexing
   		for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
   			token = *beg;
   			boost::algorithm::to_lower(token);
   			//case folding to lower case
   			
   			Porter2Stemmer::stem(token);
	   		// performs stemming on the Token as per Porter's Algorithm

   			queryTermFreequency[token]++;
   		}
   		// The above loop computes the freequency of all the tokens in the query and stores them in hash map

   		for(map<string, int>::iterator it = queryTermFreequency.begin(); it!= queryTermFreequency.end(); it++){
   			int len = termPostingListsMapping[it->first].size();
   			for(int i=0; i < len; i++){
   				flag = 1;
   				scores[termPostingListsMapping[it->first][i].first].score += termPostingListsMapping[it->first][i].second * (log10(numDocuments/len)) * log10(1+it->second);
   			}
   		}
   		// Computes the final score of all the documents using Soore[d] += w(t,q) * wg(t,d)

   		if(flag == 0){
   			cout << endl << "Sorry the query you entered does not match any of the document" << endl ;
   			continue;
   		}
   		// if the query terms do not match any of the documents then an error message is shown

   		for(int i=0; i<numDocuments; i++)
   			scores[i].score /= docLength[i];
   		// divides the documents by their lenght which is stored in the hash map docLength

   		sort(scores, scores+numDocuments, myfunc);
   		// sorts the scores in descending order

   		for(int i=0; i<k; i++){
   			cout << scores[i].docId << " -> " << docIdTitleMapping[scores[i].docId].first << " -> " << docIdTitleMapping[scores[i].docId].second << endl;
   		}
   		// prints the details of top matched documents.
    }

    return 0;
}