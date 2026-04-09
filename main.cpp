#include <iostream>
#include <atomic>
#include <vector>

template<typename T>
class SPSCQueue{
    private:
        static constexpr size_t N = 10;
        std::vector<T> m_buffer;
        std::atomic<size_t> m_head{0}; // consumer
        std::atomic<size_t> m_tail{0}; // producer

    public:
        SPSCQueue() : m_buffer(N) {}

        bool enqueue(const T& element) { // producer writes
            const auto current_tail = m_tail.load(std::memory_order_acquire);
            const auto current_head = m_head.load(std::memory_order_acquire);
            const size_t next_tail = (current_tail + 1) % N;

            if(next_tail == current_head) {
                return false;
            }

            //std::cout << "Adding element to queue successfully" << std::endl;
            m_buffer[current_tail] = element;
            m_tail.store(next_tail, std::memory_order_release);

            return true;
        }

        bool dequeue() { // consumer reads
            const auto current_tail = m_tail.load(std::memory_order_acquire);
            const auto current_head = m_head.load(std::memory_order_acquire);

            if(current_head == current_tail) {
                return false;
            }

            //std::cout << "Removing element from queue successfully" << std::endl;
            m_buffer[current_head] = 0;
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

        void printQueue() const {
            for(const auto elem : m_buffer)
            {
                std::cout << "element : " << elem << std::endl;
            }
        }

        void size() const {
            std::cout << "SPSC queue size is : " << m_buffer.size() << std::endl;
        }
};

int main() {
    SPSCQueue<int> queue;
    //queue.size();
    //queue.printQueue();
    queue.enqueue(1);
    queue.enqueue(2);
    const int x = 3;
    queue.enqueue(x);

    std::cout << "After enqueying 3 elements" << std::endl;
    //queue.size();
    queue.printQueue();

    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";

    queue.dequeue();
    queue.dequeue();
    queue.dequeue();
    queue.printQueue();
    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";

    queue.enqueue(10);
    //std::cout << "Head after dequing " << head << "\n";
    queue.printQueue();
    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";

    /*std::cout << "bool 1 : " << bool1 << "\n";
    std::cout << "bool 2 : " << bool2 << "\n";
    std::cout << "bool 3 : " << bool3 << "\n";

    for(size_t i=0; i<10; ++i) {
        auto bool5 = queue.enqueue(4);
        std::cout << "bool 5 : " << bool5 << "\n";
    }

    std::cout << "After all said and done\n";
    std::cout << "Tail is : " << queue.tail() << "\n";
    std::cout << "Head is : " << queue.head() << "\n";*/
}