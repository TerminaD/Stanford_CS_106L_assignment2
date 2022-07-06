#include <iostream>   // for cout, cin
#include <fstream>    // for ifstream
#include <sstream>    // for stringstream
#include <filesystem> // making inputting files easier
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "wikiscraper.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::priority_queue;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::unordered_set;
using std::vector;

int numCommonLinks(const unordered_set<string> &curr_set, const unordered_set<string> &target_set)
{
    cout << "Counting common links..." << endl;
    int commonLinkNum = 0;
    for (const string &link : curr_set)
    {
        if (target_set.find(link) != target_set.end())
            commonLinkNum++;
    }
    return commonLinkNum;
}

vector<string> findWikiLadder(const string &start_page, const string &end_page)
{
    cout << "Finding ladder between " << start_page << " and " << end_page << "...";
    WikiScraper w;

    /* Create alias for container backing priority_queue */
    using container = vector<vector<string>>;
    unordered_set<string> target_set = w.getLinkSet(end_page);

    auto cmp_fn = [&w, &target_set](const vector<string> &left, const vector<string> &right)
    {
        const string page1 = left.back();
        const string page2 = right.back();
        cout << "Comparing " << page1 << " with " << page2 << "...";
        const int num1 = numCommonLinks(w.getLinkSet(page1), target_set);
        const int num2 = numCommonLinks(w.getLinkSet(page2), target_set);
        return num1 < num2;
    };

    std::priority_queue<vector<string>, container, decltype(cmp_fn)> queue(cmp_fn);
    queue.push({start_page});
    unordered_set<string> visited;

    while (!queue.empty())
    {
        vector<string> curr_path = queue.top();
        queue.pop();
        string curr = curr_path.back();
        cout << "Looking at " << curr << "...";

        auto link_set = w.getLinkSet(curr);

        /*
         * Early check for whether we have found a ladder.
         * By doing this check up here we spead up the code because
         * we don't enqueue every link on this page if the target page
         * is in the links of this set.
         */
        if (link_set.find(end_page) != link_set.end())
        {
            curr_path.push_back(end_page);
            return curr_path;
        }

        for (const string &neighbour : link_set)
        {
            if (visited.find(neighbour) == visited.end())
            {
                visited.insert(neighbour);
                vector<string> new_path = curr_path;
                new_path.push_back(neighbour);
                queue.push(new_path);
            }
        }
    }
    return {};
}

int main()
{
    // a quick working directory fix to allow for easier filename inputs
    auto path = std::filesystem::current_path() / "res/";
    std::filesystem::current_path(path);
    std::string filenames = "Available input files: ";

    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        std::string filename = entry.path().string();
        filename = filename.substr(filename.rfind("/") + 1);
        filenames += filename + ", ";
    }
    // omit last ", ".
    cout << filenames.substr(0, filenames.size() - 2) << "." << endl;

    /* Container to store the found ladders in */
    vector<vector<string>> outputLadders;

    cout << "Enter a file name: ";
    string filename;
    getline(cin, filename);

    ifstream in(filename);
    int numPairs;
    // parse the first line as the number of tokens
    in >> numPairs;
    cout << "Parsed number";

    // loop through each line, parsing out page names and calling findWikiLadder
    string startPage, endPage;
    for (int i = 0; i < numPairs; i++)
    {
        // parse the start and end page from each line
        in >> startPage >> endPage;
        cout << "Parsed pair " << i << endl;
        outputLadders.push_back(findWikiLadder(startPage, endPage));
    }

    /*
     * Print out all ladders in outputLadders.
     * We've already implemented this for you!
     */
    cout << "Now outputting...";
    for (auto &ladder : outputLadders)
    {
        if (ladder.empty())
        {
            cout << "No ladder found!" << endl;
        }
        else
        {
            cout << "Ladder found:" << endl;
            cout << "\t"
                 << "{";

            std::copy(ladder.begin(), ladder.end() - 1,
                      std::ostream_iterator<string>(cout, ", "));
            /*
             * The above is an alternate way to print to cout using the
             * STL algorithms library and iterators. This is equivalent to:
             *    for (size_t i = 0; i < ladder.size() - 1; ++i) {
             *        cout << ladder[i] << ", ";
             *    }
             */
            cout << ladder.back() << "}" << endl;
        }
    }
    return 0;
}
