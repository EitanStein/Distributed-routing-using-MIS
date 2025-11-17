#pragma once

#include <random>
#include <unordered_set>

#include "MISDistributedRouting/core/node.h"



class MIS_Node : public MessagerNode
{
public:
    enum MIS_Stage{INIT, MIS_BUILDING, PATH_BUILDING, COMPLETE};
private:
    std::mt19937 rng;
protected:
    
    double rand_num;
    bool is_MIS;
    MIS_Node* my_MIS;

    MIS_Stage status;
    bool isRandNumMISCycle;

    std::unordered_map<node_id_t, MIS_Node*> active_MIS_building_neighbors;
    
    std::unordered_map<node_id_t, MIS_Node*> path_table_to_MIS_nodes; // TODO use map from node_id_t to node_id_t instead?
    std::unordered_set<node_id_t> new_entries_to_path_table;

    double GetRandNumber(double min_val = 0, double max_val = 1.0);

    void MISBuildingBroadcast(Message msg);
    void HandleMISBuildingMsg(node_id_t sender, Message msg);

    void MISBroadcast();
    void BroadcastMISStatus();
    void PostMISBroadacst();

    void BuildPathTableBroadacst();
    void PostPathTableBroadacst();
public:
    MIS_Node(node_id_t id, ThreadPool* pool) : MessagerNode(id, pool), is_MIS(false), my_MIS(nullptr), rand_num(0), 
                                                rng(std::random_device{}()), status(INIT), isRandNumMISCycle(true) {}
    ~MIS_Node() = default;

    void AddEdge(Node* other) override;

    MIS_Node* GetNeighbor(node_id_t id) const override;
    node_id_t GetMyMISID() const {return my_MIS->GetID();}

    void HandleMsg(node_id_t sender, Message msg) override;

    void PreCycle() override;
    void PostCycle() override;

    void AdvanceStage() { if (status < COMPLETE) status = static_cast<MIS_Stage>(static_cast<int>(status) + 1);}
};