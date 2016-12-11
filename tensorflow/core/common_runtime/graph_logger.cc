#include "tensorflow/core/common_runtime/graph_logger.h"
#include "tensorflow/core/framework/step_stats.pb.h"
namespace tensorflow {


    vertex *GraphLogger::addvertex(const string &name)
    {
        mutex_lock l(moo_);
	FILE *ff = fopen("/tmp/addvertex-log", "a+");
	fprintf(ff, "in addvertex\n");
	fflush(ff);
	fprintf(ff, "in addvertex:after require lock %s\n", name.c_str());
       fflush(ff);
        
        auto it = work.find(name);
        if (it == work.end()) {
        //if (work.count(name) == 0) {
	    fprintf(ff, "in addvertex: name not exist\n");
            fflush(ff);
            vertex *v = new vertex(name);
	   work[name] = v;
        } else {
	    fprintf(ff, "in addvertex: name exists\n");
            fflush(ff);
            //cout << "\nVertex already exists!";
        }
        fclose(ff);
        return work[name];
    }

    void GraphLogger::addedge(const string &from, const string &to, double cost)
    {
        vertex *f = addvertex(from);
        vertex *t = addvertex(to);
        std::pair<double, vertex *> edge = std::make_pair(cost, t);
        //f->adj_mtx.lock();
        //f->adj.push_back(edge);
        //f->adj_mtx.unlock();
        addadj(f, edge);
    }

    void GraphLogger::addadj(vertex *v, std::pair<double, vertex *> edge) {
        //v->adj_mtx.lock();
        v->adj.push_back(edge);
        //v->adj_mtx.unlock();
    }

  //  void GraphLogger::addnodes(string node_name) {
        //mutex_lock l(recv_nodes_mtx);
  //      recv_nodes.push_back(node_name);
   // }

    size_t GraphLogger::get_memory() {
        memory_mtx.lock();
        mutex_lock l(recv_nodes_mtx);
        memory += sizeof(work);
        memory += sizeof(recv_nodes);
        for (auto item : work) {
            item.second->adj_mtx.lock();
            memory += sizeof(*(item.second));
            item.second->adj_mtx.unlock();
        }
        size_t result = memory;
        memory_mtx.unlock();
        return result;
    }

    void GraphLogger::add_step_stats(NodeExecStats* nt, const Node *node)
    { 
//	moo_.lock();
        //mutex_lock l(moo_);
        if (nt) {
            //log_mtx.lock();
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
            fprintf(file, "WE DID COMPILE ");
            fprintf(file, "\n");
            //fflush(file);

            // update the information
            fflush(file);
            vertex *v = addvertex(nt->node_name());
            fflush(file);
            v->all_start_micros = nt->all_start_micros();
            v->op_start_rel_micros = nt->op_start_rel_micros();
            v->op_end_rel_micros = nt->op_end_rel_micros();
            v->all_end_rel_micros = nt->all_end_rel_micros();
            //v->total_bytes = ;
            //v->peak_bytes;
            v->timeline_label = nt->timeline_label();
            v->scheduled_micros = nt->scheduled_micros();
            v->thread_id = nt->thread_id();
            fflush(file);
            // add informaion in node for
            // 1. sender inforamtion of receiver
            // 2. consum operation for receiver
	    if (node == NULL) {
		fprintf(file, "node is NULL");
	    }
            const NodeDef& def = node->def();
            if (IsRecv(node)) {
                string send_device;
                TF_CHECK_OK(GetNodeAttr(def, "send_device", &send_device));
                v->is_recv = true;
                v->sender = send_device;
                //currently no mutex lock is needed now
                //recv_nodes_mtx.lock();
		recv_nodes.push_back(v);
                //recv_nodes_mtx.unlock();
            }
            std::vector<StringPiece> inputs = std::vector<StringPiece>(def.input().begin(), def.input().end());
            for (auto& str : inputs) {
                string new_str = str.ToString();
                //work_mtx.lock();
                if (work.count(new_str) == 0) {
                    fprintf(file, "[fatal]: child node done but parent haven't recorded yet\n");
                    //work_mtx.unlock();
                    continue;
                }
                //work_mtx.unlock();
                vertex *v_parent = addvertex(new_str);
                //no gap_mtx is needed here currently
                //v_parent->gap_mtx.lock();
                v_parent->minimum_gap = std::min(v->all_start_micros - v_parent->all_start_micros, v_parent->minimum_gap);
                fprintf(file, "Minimum gap %s %d", new_str.c_str(), v_parent->minimum_gap);
                //v_parent->gap_mtx.unlock();
            }
            fclose(file);
//            log_mtx.unlock();
//	   moo_.unlock();
     }
    }
}
