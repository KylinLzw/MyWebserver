#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <cassert>

class threadPool {
public:
    explicit threadPool(size_t threadCoun = 8) : _pool(std::make_shared<Pool>()) {
        assert(threadCoun > 0);

        for(size_t i = 0; i < threadCoun; i++) {
            std::thread( [pool = _pool]{
                std::unique_lock<std::mutex> locker(pool->_mtx);
                    while(true) {
                        if(!pool->tasks.empty()) {
                            auto task = std::move(pool->tasks.front());
                            pool->tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        } 
                        else if(pool->isClosed) break;
                        else pool->_cond.wait(locker);
                    }
            }).detach();
        }
    }

    threadPool() = default;

    threadPool(threadPool&&) = default;

    ~threadPool() {
        if(static_cast<bool>(_pool)) {
            {
                std::unique_lock<std::mutex>(_pool->_mtx);
                _pool->isClosed = true;
            }
            _pool->_cond.notify_all();
        }
    }

    template<class F>
    void AddTask(F&& task) {
        {
            std::lock_guard<std::mutex> locker(_pool->_mtx);    
            _pool->tasks.emplace(std::forward<F>(task));
        }
        _pool->_cond.notify_one();
    }


private:
    struct Pool {
        bool isClosed;
        std::mutex _mtx;
        std::condition_variable _cond;
        std::queue< std::function<void()> > tasks;
    };
    std::shared_ptr<Pool> _pool;
};