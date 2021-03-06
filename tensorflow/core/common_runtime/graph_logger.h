#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <limits.h>
#include "tensorflow/core/common_runtime/step_stats_collector.h"
#include "tensorflow/core/framework/step_stats.pb.h"
#include "tensorflow/core/framework/node_def_util.h"
#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/platform/mutex.h"
#ifndef _GRAPH_LOG_
#define _GRAPH_LOG_

using namespace std;
namespace tensorflow {
    struct vertex {
        typedef pair<double, vertex*> ve;
        vector<ve> adj; //cost of edge, destination vertex
        mutex adj_mtx;
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
        bool is_recv;
        string sender;
        int64_t minimum_gap;
        mutex gap_mtx;
        vertex(string s) : name(s) {
            is_recv = false;
            minimum_gap = LONG_MAX;
        }
    };

    class GraphLogger
    {
        public:
            typedef map<string, vertex *> vmap;
            vmap work;
            mutex work_mtx;
            vector<string> recv_nodes;
            mutex recv_nodes_mtx;
            mutex log_mtx;
            vertex *addvertex(const string&);
            void addedge(const string& from, const string& to, double cost);
            void addadj(vertex *v, pair<double, vertex *> edge);
            void addnodes(string node_name);
            void add_step_stats(tensorflow::NodeExecStats* stats, const Node *node);
            size_t get_memory();
            GraphLogger() {memory = 0;}
        private:
            mutex memory_mtx;
            size_t memory;
    };
}

#endif


