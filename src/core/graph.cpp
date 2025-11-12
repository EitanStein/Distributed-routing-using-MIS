#include "MISDistributedRouting/core/graph.h"

#include <ranges>
#include <random>
#include <iterator>

void Graph::InitGraphNodes(size_t graph_size)
{
    nodes.clear();

    nodes.reserve(graph_size);
    for(auto i : std::views::iota(size_t{0}, graph_size))
        AddNode();
}

void Graph::AddNode()
{
    nodes.emplace_back(std::make_unique<Node>(nodes.size(), &thread_pool));
}

void Graph::AddEdge(node_id_t node1, node_id_t node2)
{
    nodes[node1]->AddEdge(nodes[node2].get());
    nodes[node2]->AddEdge(nodes[node1].get());
}

void Graph::RunTaskOnAllNodes(std::function<void(node_id_t)> task, bool wait)
{
    size_t graph_size = GetGraphSize();
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        thread_pool.AddTask([id, task](){
            task(id);
        });
    }

    if(wait)
        thread_pool.WaitForEmptyQueue();
}