#include "MISDistributedRouting/graph/graph.h"


Graph::Graph(size_t graph_size, size_t num_edges, size_t thread_pool_size=DEFAULT_POOL_SIZE) : thread_pool(thread_pool_size)
{

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