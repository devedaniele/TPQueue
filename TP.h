#ifndef __DS_TP__
    #define __DS_TP__

    #include <condition_variable> 
    #include <functional>
    #include <thread>
    #include <mutex>
    #include <queue>
    #include <vector>

    //max thread size = thread::hardware_concurrency()

    using namespace std;

    class TPQueue{
        public:
            bool stop;
            
            TPQueue(){
                stop = false;

                changeTPSize(1);
            }
            TPQueue(int size){
                stop = false;

                changeTPSize(size);
            }

            void changeTPSize(int size){
                int i;

                if (size < tpsize){
                    i = tpsize - size;
                    tpsize = size;
                    size = i;

                    for (i = 0;i < size;i++){
                        workers.pop_back();
                    }

                    return;
                }

                i = size - tpsize;
                tpsize = size;
                size = i;

                for (i = 0;i < size;i++){
                    workers.emplace_back([this](){
                        while(true){
                            unique_lock<mutex> lock(locker);

                            signal.wait(lock,[this](){
                                return stop || !tasks.empty();
                            });

                            if (stop && tasks.empty())
                                return;
                    
                            auto task = move(tasks.front());
                            tasks.pop();
                            lock.unlock();

                            task();
                        }
                    });
                }
            }

            void enqueue(function<void()> task){
                unique_lock<mutex> lock(locker);
                tasks.emplace(task);
                lock.unlock();

                signal.notify_one();
            }

            ~TPQueue(){
                unique_lock<mutex> lock(locker);
                stop = true;
                lock.unlock();

                signal.notify_all();

                for (thread& worker : workers)
                    worker.join();
            }

        private:
            int tpsize;
            mutex locker;
            queue<function<void()>> tasks;
            vector<thread> workers;
            condition_variable signal;
    };
#endif