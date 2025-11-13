#include "MISDistributedRouting/algorithm/MIS_graph.h"

#include <ranges>

void MIS_Graph::AddNode()
{
    nodes.emplace_back(std::make_unique<MIS_Node>(nodes.size(), &thread_pool));
}

MIS_Node* MIS_Graph::GetNode(node_id_t node_id) const
{
    return static_cast<MIS_Node*>(Graph::GetNode(node_id));
}

void MIS_Graph::BuildMIS()
{
    size_t graph_size = GetGraphSize();
    size_t num_iterations = 10*(size_t)log2(graph_size);

    for(auto i : std::views::iota(size_t{0}, num_iterations))
    {
        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->MISBroadcast();
        });

        TransferPendingMessages();

        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->PostMISBroadacst();
        });

        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->BroadcastMISStatus();
        });

        TransferPendingMessages();

        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->PostMISStatusBroadacst();
        });
    }
}

bool MIS_Graph::IsPathTableDone() const
{
    for(node_id_t id=0; id < nodes.size() ; ++id)
    {
        if (GetNode(id)->IsNewPathTableEntriesEmpty() == false)
            return false;
    }

    return true;
}

void MIS_Graph::BuildPathTable()
{
    RunTaskOnAllNodes([this](node_id_t id){
        GetNode(id)->BuildPathTableBroadacstStart();
    });

    TransferPendingMessages();

    RunTaskOnAllNodes([this](node_id_t id){
        GetNode(id)->PostPathTableBroadacst();
    });

    while(!IsPathTableDone())
    {
        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->BuildPathTableBroadacst();
        });

        TransferPendingMessages();

        RunTaskOnAllNodes([this](node_id_t id){
            GetNode(id)->PostPathTableBroadacst();
        });
    }
}


void MIS_Graph::InitMIS()
{
    BuildMIS();
    BuildPathTable();
}