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

    MIS_Stage stage;
    bool isRandNumMISCycle;

    std::unordered_map<node_id_t, MIS_Node*> active_MIS_building_neighbors;
    
    std::unordered_map<node_id_t, MIS_Node*> path_table_to_MIS_nodes;
    std::unordered_set<node_id_t> new_entries_to_path_table;

    double GetRandNumber(double min_val = 0, double max_val = 1.0);

    void HandleMISBuildingMsg(node_id_t sender, MsgType::MISBuildingMessage&& msg);
    void HandlePathBuildingMsg(node_id_t sender, MsgType::PathBuildingMessage&& msg);
    void HandleRegularMessage(node_id_t sender, MsgType::RoutedMessage&& msg);

    void MISBuildingBroadcast(Message msg);

    void MISBroadcast();
    void BroadcastMISStatus();

    void BuildPathTableBroadacst();
public:
    MIS_Node(node_id_t id, ThreadPool* pool);
    ~MIS_Node();

    void AddEdge(Node* other) override;

    [[nodiscard]] MIS_Node* GetNeighbor(node_id_t id) const override;
    [[nodiscard]] node_id_t GetMyMISID() const {return my_MIS->GetID();}

    void HandleMsg(node_id_t sender, Message&& msg) override;
    void HandleSendingNewMessages() override;

    void AdvanceStage();
};