#include "MISDistributedRouting/algorithm/MIS_graph.h"

#include <ranges>


MIS_Graph::MIS_Graph(size_t graph_size, size_t num_edges, size_t thread_pool_size) : Graph(thread_pool_size)
{
    this->graph_size = graph_size;
    nodes.reserve(graph_size);
    for(auto i : std::views::iota(size_t{0}, graph_size))
        AddNode();

    AddRandEdges(num_edges);
}

void MIS_Graph::AddNode()
{
    nodes.emplace_back(std::make_unique<MIS_Node>(nodes.size(), &thread_pool));
}

void MIS_Graph::AddEdge(node_id_t node1, node_id_t node2)
{
    static_cast<MIS_Node*>(nodes[node1].get())->AddEdge(nodes[node2].get());
    static_cast<MIS_Node*>(nodes[node2].get())->AddEdge(nodes[node1].get());
}

void MIS_Graph::BuildMIS()
{
    size_t num_iterations = 10*(size_t)log2(graph_size);

    for(auto i : std::views::iota(size_t{0}, num_iterations))
    {
        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->MISBroadcast();
        });

        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->PostMISBroadacst();
        });

        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->BroadcastMISStatus();
        });

        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->PostMISStatusBroadacst();
        });
    }
}

bool MIS_Graph::IsPathTableDone() const
{
    for(node_id_t id=0; id < nodes.size() ; ++id)
    {
        if (static_cast<MIS_Node*>(nodes[id].get())->IsNewPathTableEntriesEmpty() == false)
            return false;
    }

    return true;
}

void MIS_Graph::BuildPathTable()
{
    RunTaskOnAllNodes([this](node_id_t id){
        static_cast<MIS_Node*>(nodes[id].get())->BuildPathTableBroadacstStart();
    });

    RunTaskOnAllNodes([this](node_id_t id){
        static_cast<MIS_Node*>(nodes[id].get())->PostPathTableBroadacst();
    });

    while(!IsPathTableDone())
    {
        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->BuildPathTableBroadacst();
        });

        RunTaskOnAllNodes([this](node_id_t id){
            static_cast<MIS_Node*>(nodes[id].get())->PostPathTableBroadacst();
        });
    }
}