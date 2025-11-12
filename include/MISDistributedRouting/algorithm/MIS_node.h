#pragma once

#include <random>
#include <unordered_set>

#include "MISDistributedRouting/core/node.h"


class MIS_Node : public Node {
private:
    std::mt19937 rng;
protected:
    double rand_num;
    bool is_MIS;
    MIS_Node* my_MIS;

    std::unordered_map<node_id_t, MIS_Node*> active_MIS_building_neighbors;
    
    std::unordered_map<node_id_t, MIS_Node*> path_table_to_MIS_nodes; // TODO use map from node_id_t to node_id_t instead?
    std::unordered_set<node_id_t> new_entries_to_path_table;

    double GetRandNumber(double min_val = 0, double max_val = 1.0);
public:
    MIS_Node(node_id_t id, ThreadPool* pool) : Node(id, pool), is_MIS(false), my_MIS(nullptr), rand_num(0), rng(std::random_device{}()) {}
    ~MIS_Node() = default;

    void AddEdge(Node* other) override;

    void ForwardMsg(Message msg) const;

    std::optional<Message> HandleRegularMsg(Message msg) const;

    void MISBuildingBroadcast(Message msg) const;
    void HandleMISBuildingMsg(node_id_t sender, Message msg);

    void MISBroadcast();
    void PostMISBroadacst();
    void BroadcastMISStatus();
    void PostMISStatusBroadacst();

    void BuildPathTableBroadacstStart() const;
    void BuildPathTableBroadacst() const;
    void PostPathTableBroadacst();
    bool IsNewPathTableEntriesEmpty() const;

};