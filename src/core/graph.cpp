#include "MISDistributedRouting/core/graph.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <ranges>
#include <random>
#include <iterator>
#include <thread>
#include <chrono>

void Graph::InitGraph(size_t graph_size)
{
    InitGraphNodes(graph_size);
}

void Graph::InitGraphNodes(size_t graph_size)
{
    nodes.clear();

    nodes.reserve(graph_size);
    for(auto i : std::views::iota(size_t{0}, graph_size))
        AddNode();
}

void Graph::AddNode()
{
    nodes.emplace_back(std::make_unique<Node>(nodes.size()));
}

void Graph::AddEdge(node_id_t node1, node_id_t node2)
{
    nodes[node1]->AddEdge(nodes[node2].get());
    nodes[node2]->AddEdge(nodes[node1].get());
}

Node* Graph::GetNode(node_id_t node_id) const
{
    if(node_id >= GetGraphSize())
    {
        LOG_ERROR("node id {} is larger than graph size", node_id);
        return nullptr;
    }

    return nodes[node_id].get();
}

void SyncedGraph::InitGraph(size_t graph_size)
{
    thread_pool.StopAllThreads();
    thread_pool.InitPool();
    Graph::InitGraph(graph_size);
}

void SyncedGraph::AddNode()
{
    nodes.emplace_back(std::make_unique<MessagerNode>(nodes.size(), &thread_pool));
}


void SyncedGraph::WaitForInactiveThreadPool()
{
    thread_pool.WaitForEmptyQueue();
}

MessagerNode* SyncedGraph::GetNode(node_id_t node_id) const
{
    return static_cast<MessagerNode*>(Graph::GetNode(node_id));
}

void SyncedGraph::TransferPendingMessages()
{
    size_t graph_size = GetGraphSize();
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        thread_pool.AddTask([this, id](){
            GetNode(id)->SendAllOutboxMessages();
        });
    }

    thread_pool.WaitForEmptyQueue();
}

bool SyncedGraph::AreMessagesPending()
{
    size_t graph_size = GetGraphSize();
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        if(!GetNode(id)->IsOutboxEmpty())
            return true;
    }
    return false;
}


void SyncedGraph::PreCycleAllNodes()
{
    size_t graph_size = GetGraphSize();
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        thread_pool.AddTask([this, id](){
            GetNode(id)->PreCycle();
        });
    }

    WaitForInactiveThreadPool();
}


void SyncedGraph::PostCycleAllNodes()
{
    size_t graph_size = GetGraphSize();
    for(node_id_t id=0; id < graph_size ; ++id)
    {
        thread_pool.AddTask([this, id](){
            GetNode(id)->PostCycle();
        });
    }

    WaitForInactiveThreadPool();
}


bool SyncedGraph::RunCycle()
{
    
    PreCycleAllNodes();

    bool are_messages_pending = AreMessagesPending();

    TransferPendingMessages();

    PostCycleAllNodes();

    return !are_messages_pending;
}