#include "MISDistributedRouting/core/message_box.h"


void MessageBox::AddMsg(node_id_t node_id, Message msg)
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue.push({node_id, std::move(msg)});
}

std::optional<std::pair<node_id_t, Message>> MessageBox::PopMsg()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (queue.empty())
        return std::nullopt;

    std::pair<node_id_t, Message> top_msg = std::move(queue.front());
    queue.pop();

    return top_msg;
}

void MessageBox::Clear()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    std::queue<std::pair<node_id_t, Message>> empty_queue;
    queue.swap(empty_queue);
}

bool MessageBox::IsEmpty()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    return queue.empty();
}
