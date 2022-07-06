#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <unordered_map>
#include "wikiscraper.h"
#include "error.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::unordered_set;

bool valid_wikilink(const string& link)
{
    return std::all_of(link.begin(), link.end(), [](char c){return c != '#' && c != ':';});
}

unordered_set<string> findWikiLinks(const string& inp)
{
    cout << "Parsing pulled page..." << endl;
	static const string delim = "href=\"/wiki/";
	unordered_set<string> ret;

	auto url_start = inp.begin();

	while (true)
	{
		url_start = std::search(url_start, inp.end(), delim.begin(), delim.end());
		if (url_start == inp.end())
			break;
		else
			url_start -= 3;
		
		auto url_end = std::find(url_start, inp.end(), '>');
		
		string link(url_start + 15, url_end - 1);
		
		if (valid_wikilink(link))
			ret.insert(link);

		url_start = url_end;
	}

    return ret;
}

/*
 * ==================================================================================
 * |                Don't edit anything below here, but take a peek!                |
 * ==================================================================================
 */
unordered_set<string> WikiScraper::getLinkSet(const string &page_name)
{
    cout << "Getting all links for " << page_name << "..." << endl;
    if (linkset_cache.find(page_name) == linkset_cache.end())
    {
        auto links = findWikiLinks(getPageSource(page_name));
        linkset_cache[page_name] = links;
    }
    return linkset_cache[page_name];
}

WikiScraper::WikiScraper()
{
    (void)getPageSource("Main_Page");
}

string createPageUrl(const string &page_name)
{
    cout << "Getting URL for " << page_name << "..." << endl;
    return "https://en.wikipedia.org/wiki/" + page_name;
}

void notFoundError(const string &msg, const string &page_name, const string &url)
{
    const string title = "    AN ERROR OCCURED DURING EXECUTION.    ";
    const string border(title.size() + 4, '*');
    cerr << endl;
    cerr << border;
    cerr << "* " + title + " *";
    cerr << border;
    cerr << endl;
    cerr << "Reason: " + msg;
    cerr << endl;
    cerr << "Debug Information:";
    cerr << endl;
    cerr << "\t- Input parameter: " + page_name;
    cerr << "\t- Attempted url: " + url;
    cerr << endl;
}

string WikiScraper::getPageSource(const string &page_name)
{
    const static string not_found = "Wikipedia does not have an article with this exact name.";
    cout << "Pulling contents of " << page_name << "..." << endl;
    if (page_cache.find(page_name) == page_cache.end())
    {
        string url = createPageUrl(page_name);
        // using the cpr library to get the HTML content of a webpage!
        // we do so by aking a GET REST request to a wikipedia webpage, which
        // returns the content of the webpage. when this assignment was on QtCreator,
        // we had a whole separate assignment for making sure an alternate Internet Library
        // (not cpr) was working on your personal pc. look how simple it is now!
        cout << "Calling cpr..." << endl;
        cpr::Response r = cpr::Get(cpr::Url{url});
        cout << "Got a cpr response!" << endl;

        string ret = r.text;
        if (r.status_code != 200)
        {
            notFoundError("Couldn't get page source. Have you entered a valid link?", page_name, url);
            return "";
        }
        if (std::search(ret.begin(), ret.end(), not_found.begin(), not_found.end()) != ret.end())
        {
            notFoundError("Page does not exist!", page_name, url);
            return "";
        }
        size_t indx = ret.find("plainlinks hlist navbar mini");
        if (indx != string::npos)
        {
            return ret.substr(0, indx);
        }
        page_cache[page_name] = ret;
    }
    return page_cache[page_name];
}
