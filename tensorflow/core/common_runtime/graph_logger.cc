#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

struct vertex {
    typedef pair<int, vertex*> ve;
    vector<ve> adj; //cost of edge, destination vertex
    string name;
    int64_t all_start_micros;
    int64_t op_start_rel_micros;
    int64_t op_end_rel_micros;
    int64_t all_end_rel_micros;
    int64_t total_bytes;
    int64_t peak_bytes;
    string timeline_label;
    int64_t scheduled_micros;
    uint32_t thread_id;
	
    vertex(string s) : name(s) {}
};

class graph_logger
{
public:
    typedef map<string, vertex *> vmap;
    vmap work;
    void addvertex(const string&);
    void addedge(const string& from, const string& to, double cost);
};

void graph_logger::addvertex(const string &name)
{
    vmap::iterator itr = work.find(name);
    if (itr == work.end())
    {
        vertex *v;
        v = new vertex(name);
        work[name] = v;
        return;
    }
    cout << "\nVertex already exists!";
}

void graph_logger::addedge(const string& from, const string& to, double cost)
{
    vertex *f = (work.find(from)->second);
    vertex *t = (work.find(to)->second);
    pair<int, vertex *> edge = make_pair(cost, t);
    f->adj.push_back(edge);
}

