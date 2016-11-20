#include "tensorflow/core/common_runtime/graph_logger.h"
#include "tensorflow/core/framework/step_stats.pb.h"
namespace tensorflow {
graph_logger& graph_logger::getInstance() {
    static graph_logger instance;
    return instance;
}


vertex *graph_logger::addvertex(const string &name)
{
    if (work.count(name) == 0) {
	vertex *v;
        v = new vertex(name);
        work[name] = v;
	return v;
    } else {
	cout << "\nVertex already exists!";
	return work[name];
    }
}

void graph_logger::addedge(const string &from, const string &to, double cost)
{
    vertex *f = addvertex(from);
    vertex *t = addvertex(to);
    pair<double, vertex *> edge = make_pair(cost, t);
    f->adj.push_back(edge);
}

void graph_logger::add_step_stats(NodeExecStats* nt, Node *node)
{
    if (nt) {
        FILE *file = fopen("/tmp/step_stat.log", "a+");
        fprintf(file, "node_name: ");
    	fprintf(file, nt->node_name().c_str());
    	fprintf(file, "\n");

    	fprintf(file, "all_start_micros: ");
    	fprintf(file, "%ld", nt->all_start_micros());
    	fprintf(file, "\n");

    	fprintf(file, "op_start_rel_micros: ");
    	fprintf(file, "%ld", nt->op_start_rel_micros());
    	fprintf(file, "\n");

    	fprintf(file, "op_end_rel_micros: ");
    	fprintf(file, "%ld", nt->op_end_rel_micros());
    	fprintf(file, "\n");

    	fprintf(file, "all_end_rel_micros: ");
    	fprintf(file, "%ld", nt->all_end_rel_micros());
    	fprintf(file, "\n");

    	fprintf(file, "scheduled_micros: ");
    	fprintf(file, "%ld", nt->scheduled_micros());
    	fprintf(file, "\n");

    	fprintf(file, "schedule_start_skew: ");
    	fprintf(file, "%ld", nt->all_start_micros()- nt->scheduled_micros());
    	fprintf(file, "\n");

    	fprintf(file, "thread_id: ");
    	fprintf(file, "%ld", nt->thread_id());
    	fprintf(file, "\n");

    	fprintf(file, "timeline_label: ");
    	fprintf(file, nt->timeline_label().c_str());
    	fprintf(file, "\n");

    	fprintf(file, "\n");
    	fflush(file);

    	// update the information
   	vertex *v = addvertex(nt->node_name());
    	v->all_start_micros = nt->all_start_micros();
    	v->op_start_rel_micros = nt->op_start_rel_micros();
    	v->op_end_rel_micros = nt->op_end_rel_micros();
    	v->all_end_rel_micros = nt->all_end_rel_micros();
    	//v->total_bytes = ;
    	//v->peak_bytes;
    	v->timeline_label = nt->timeline_label();
   	v->scheduled_micros = nt->scheduled_micros();
    	v->thread_id = nt->thread_id();

	// add informaion in node for 
	// 1. sender inforamtion of receiver
	// 2. consum operation for receiver 
	if (IsRecv(node)) {
		const NodeDef& def = node->def();
		string send_device;
    		TF_CHECK_OK(GetNodeAttr(def, "send_device", &send_device));
		v->is_recv = true;
		v->sender = send_device;
		recv_nodes.push_back(nt->node_name());
	}
	std::vector<StringPiece> inputs = std::vector<StringPiece>(def.input().begin(), def.input().end());
	for (auto& str : inputs) {
		string new_str = str.ToString();
		if (work.count(new_str) == 0) {
			fprintf(file, "[fatal]: child node done but parent haven't recorded yet\n");
			continue;
		}
		vertex *v_parent = addvertex(new_str);
		v_parent->minimum_gap = min(v->all_start_micros - v_parent->all_start_micros, v_parent->minimum_gap);
	}
	fclode(file);
    }
}
}
