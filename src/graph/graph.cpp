#include "MISDistributedRouting/graph/graph.h"

#include <ranges>
#include <random>
#include <iterator>

Graph::Graph(size_t graph_size, size_t num_edges, size_t thread_pool_size) : thread_pool(thread_pool_size)
{
    nodes.reserve(graph_size);
    for(auto i : std::views::iota(size_t{0}, graph_size))
        AddNode();

    size_t max_num_edges = graph_size*(graph_size - 1)/2;
    if(num_edges > max_num_edges)
        num_edges = max_num_edges;

    std::vector<std::pair<node_id_t, node_id_t>> all_edges;
    all_edges.reserve(graph_size*(graph_size-1)/2);
    for (int i = 0; i < graph_size; ++i)
        for (int j = i + 1; j < graph_size; ++j)
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