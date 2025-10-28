#include "MISDistributedRouting/core/graph.h"

#include <ranges>
#include <random>
#include <iterator>

Graph::Graph(size_t graph_size, size_t num_edges, size_t thread_pool_size) : thread_pool(thread_pool_size), graph_size(graph_size)
{
    nodes.reserve(graph_size);
    for(auto i : std::views::iota(size_t{0}, graph_size))
        AddNode();

    AddRandEdges(num_edges);
}

void Graph::AddRandEdges(size_t num_edges)
{
    size_t num_max_edges = graph_size*(graph_size - 1)/2;
    if(num_edges > num_max_edges)
        num_edges = num_max_edges;

    std::vector<std::pair<node_id_t, node_id_t>> all_edges;
    all_edges.reserve(num_max_edges);
    for (node_id_t i = 0; i < graph_size; ++i)
        for (node_id_t j = i + 1; j < graph_size; ++j)
            all_edges.emplace_back(i, j);

    std::vector<std::pair<node_id_t, node_id_t>> chosen;
    chosen.reserve(num_edges);
    std::sample(all_edges.begin(), all_edges.end(),
                std::back_inserter(chosen),
                num_edges, std::mt19937{std::random_device{}()});


    for (auto [i, j] : chosen)
        AddEdge(i, j);
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
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        thread_pool.AddTask([id, task](){
            task(id);
        });
    }

    if(wait)
        thread_pool.WaitForEmptyQueue();
}