#include "MISDistributedRouting/core/inbox.h"

void Inbox::ReceiveMsg(node_id_t src, Message msg)
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue.push({src, std::move(msg)});
}

std::optional<std::pair<node_id_t, Message>> Inbox::ReadMsg()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (queue.empty())
        return std::nullopt;

    std::pair<node_id_t, Message> top_msg = std::move(queue.front());
    queue.pop();

    return top_msg;

}

void Inbox::EmptyInbox()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    std::queue<std::pair<node_id_t, Message>> empty_queue;
    queue.swap(empty_queue);
}

bool Inbox::IsEmpty()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    return queue.empty();
}