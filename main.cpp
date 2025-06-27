#include <iostream>
#include <atomic>
#include <vector>

template<typename T>
class SPSCQueue{
    private:
        std::vector<T> m_buffer{N};
        std::atomic<size_t> m_head{0}; // consumer
        std::atomic<size_t> m_tail{0}; // producer
        static constexpr size_t N = 10;

    public:
        bool enqueue(const T& element) {
            const auto current_tail = m_tail.load(std::memory_order_acquire);
            const auto current_head = m_head.load(std::memory_order_acquire);
            const size_t next_tail = (current_tail + 1) % N;

            if(next_tail == current_head) {
                return false;
            }

            m_buffer[current_tail] = element;
            m_tail.store(next_tail, std::memory_order_release);

            return true;
        }

        bool dequeue(T& head_value) {
            const auto current_tail = m_tail.load(std::memory_order_acquire);
            const auto current_head = m_head.load(std::memory_order_acquire);

            if(current_head == current_tail) {
                return false;
            }

            head_value = m_buffer[current_head];
            const size_t prev_head = (current_head + 1) % N;
            m_head.store(prev_head, std::memory_order_release);

            return true;
        }

        size_t head() const {
            return m_head.load(std::memory_order_acquire);
        }

        size_t tail() const {
            return m_tail.load(std::memory_order_acquire);
        }
};

int main() {
    SPSCQueue<int> queue;
    queue.enqueue(4);
    queue.enqueue(8);
    const int x = 1;
    queue.enqueue(x);

    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";

    int head;
    queue.dequeue(head);

    std::cout << "Head after dequing " << head << "\n";

    auto bool1 = queue.dequeue(head);
    auto bool2 = queue.dequeue(head);
    auto bool3 = queue.dequeue(head);

    std::cout << "bool 1 : " << bool1 << "\n";
    std::cout << "bool 2 : " << bool2 << "\n";
    std::cout << "bool 3 : " << bool3 << "\n";

    for(size_t i=0; i<10; ++i) {
        auto bool5 = queue.enqueue(4);
        std::cout << "bool 5 : " << bool5 << "\n";
    }

    std::cout << "After all said and done\n";
    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";
}