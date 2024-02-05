#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <exception>
#include <utility>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>
#include <queue>
#include <shared_mutex>
#include <list>
#include <limits>

#include "machine.hpp"

class FulfillmentFailure : public std::exception
{
};

class OrderNotReadyException : public std::exception
{
};

class BadOrderException : public std::exception
{
};

class BadPagerException : public std::exception
{
};

class OrderExpiredException : public std::exception
{
};

class RestaurantClosedException : public std::exception
{
};

struct WorkerReport
{
    std::vector<std::vector<std::string>> collectedOrders;
    std::vector<std::vector<std::string>> abandonedOrders;
    std::vector<std::vector<std::string>> failedOrders;
    std::vector<std::string> failedProducts;
};

class CoasterPager
{
public:
    void wait() const;

    void wait(unsigned int timeout) const;

    [[nodiscard]] unsigned int getId() const;

    [[nodiscard]] bool isReady() const;

protected:
    explicit CoasterPager(unsigned int id_, std::future<bool> futureReady_, std::future<std::vector<std::unique_ptr<Product>>> futureProducts_, std::promise<bool> visitPromise_) : id(id_), futureReady(std::move(futureReady_)), futureProducts(std::move(futureProducts_)), visitPromise(std::move(visitPromise_)) {}

private:
    unsigned int id;
    mutable std::future<bool> futureReady;
    std::future<std::vector<std::unique_ptr<Product>>> futureProducts;
    std::promise<bool> visitPromise;
    mutable bool ready = false;

    friend class System;
};

class System
{
public:
    typedef std::unordered_map<std::string, std::shared_ptr<Machine>> machines_t;
    
    System(machines_t machines, unsigned int numberOfWorkers, unsigned int clientTimeout);

    std::vector<WorkerReport> shutdown();

    std::vector<std::string> getMenu() const;

    std::vector<unsigned int> getPendingOrders() const;

    std::unique_ptr<CoasterPager> order(std::vector<std::string> products);

    std::vector<std::unique_ptr<Product>> collectOrder(std::unique_ptr<CoasterPager> CoasterPager);

    unsigned int getClientTimeout() const;

private:
    void work(unsigned int reportId);

    void machineGet(const std::shared_ptr<Machine>& machine, std::list<std::string>::iterator iterator, std::string productName);

    void machineReturn(const std::shared_ptr<Machine>& machine);

    struct orderToPrepare {
        unsigned int id;
        std::vector<std::string> products;
        std::promise<bool> orderReady;
        std::promise<std::vector<std::unique_ptr<Product>>> promiseOrder;
        std::future<bool> futureVisit;
        std::list<unsigned int>::iterator pendingIt;

        orderToPrepare(unsigned int id_, std::vector<std::string> &products, std::promise<bool> orderReady_, std::promise<std::vector<std::unique_ptr<Product>>> promiseOrder_, std::future<bool> futureVisit_, std::list<unsigned int>::iterator pendingIt_) : id(id_), products(products), orderReady(std::move(orderReady_)), promiseOrder(std::move(promiseOrder_)), futureVisit(std::move(futureVisit_)) {
            pendingIt = pendingIt_;
        }
    };

    machines_t machines;
    std::vector<std::thread> workerThreads;
    std::vector<std::thread> getMachineThreads;
    std::vector<std::thread> returnMachineThreads;
    std::atomic_bool closed = false;
    std::atomic_bool stopMachines = false;
    unsigned int numberOfWorkers;
    unsigned int clientTimeout;
    std::atomic_bool reportReady = false;
    std::vector<WorkerReport> report;

    std::condition_variable waitingWorkers;

    mutable std::shared_mutex menuMutex;
    mutable std::list<std::string> menu;

    mutable std::mutex toPrepareMutex;
    std::queue<orderToPrepare> ordersToPrepare;

    mutable std::shared_mutex pendingOrdersMutex;
    std::list<unsigned int> pendingOrders;

    std::unordered_map<std::shared_ptr<Machine>, std::pair<std::mutex, std::condition_variable>> getMutexes;
    std::unordered_map<std::shared_ptr<Machine>, std::queue<std::promise<std::unique_ptr<Product>>>> getQueues;

    std::unordered_map<std::shared_ptr<Machine>, std::pair<std::mutex, std::condition_variable>> returnMutexes;
    std::unordered_map<std::shared_ptr<Machine>, std::queue<std::unique_ptr<Product>>> returnQueues;

    mutable std::shared_mutex menuMapMutex;
    std::unordered_map<std::string, bool> menuMap;
};

#endif // SYSTEM_HPP