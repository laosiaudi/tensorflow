#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <limits.h>
#include "tensorflow/core/common_runtime/step_stats_collector.h"
#include "tensorflow/core/framework/step_stats.pb.h"
#include "tensorflow/core/graph/graph.h"
#ifndef _GRAPH_LOG_
#define _GRAPH_LOG_

using namespace std;
namespace tensorflow {
	struct vertex {
	    typedef pair<double, vertex*> ve;
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
            bool if_recv;
            String sender;
	    int64_t minimum_gap; 
	    vertex(string s) : name(s) {
		if_recv = false;
		minimum_gap = LONG_MAX;	
	    }
	};

	class graph_logger
	{
	public:
	    typedef map<string, vertex *> vmap;
	    vmap work;
	    vector<string> recv_nodes;
	    void addvertex(const string&);
	    void addedge(const string& from, const string& to, double cost);
	    void add_step_stats(tensorflow::NodeExecStats* stats, Node *node);
        static graph_logger& getInstance();
    private:
        graph_logger();
        ~graph_logger();
	};
}

#endif


