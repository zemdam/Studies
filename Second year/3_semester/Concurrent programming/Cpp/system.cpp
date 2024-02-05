#include "system.hpp"

#include <iostream>

using namespace std::chrono_literals;

void CoasterPager::wait() const {
    if (!ready) {
        if (!futureReady.valid()) {
            throw FulfillmentFailure();
        }
        ready = futureReady.get();
    }
}

void CoasterPager::wait(unsigned int timeout) const {
    if (!ready) {
        if (!futureReady.valid()) {
            throw FulfillmentFailure();
        }
        if (futureReady.wait_for(timeout * 1ms) == std::future_status::ready) {
            ready = futureReady.get();
        }
    }
}

unsigned int CoasterPager::getId() const {
    return id;
}

bool CoasterPager::isReady() const {
    if (!ready) {
        if (!futureReady.valid() || futureReady.wait_for(0ms) == std::future_status::ready) {
            return true;
        }
    }
    return ready;
}

std::vector<WorkerReport> System::shutdown() {
    if (closed.exchange(true)) {
        reportReady.wait(false);
        return report;
    }

    std::unique_lock lock(toPrepareMutex);
    waitingWorkers.notify_all();
    lock.unlock();
    for (auto &worker : workerThreads) {
        worker.join();
    }

    reportReady = true;
    reportReady.notify_all();

    stopMachines = true;
    for (auto &cond : getMutexes) {
        cond.second.first.lock();
        cond.second.second.notify_all();
        cond.second.first.unlock();
    }

    for (auto &cond : returnMutexes) {
        cond.second.first.lock();
        cond.second.second.notify_all();
        cond.second.first.unlock();
    }

    for (auto &thread : getMachineThreads) {
        thread.join();
    }

    for (auto &thread : returnMachineThreads) {
        thread.join();
    }

    for (auto &machine : machines) {
        machine.second->stop();
    }

    return report;
}

std::vector<std::string> System::getMenu() const {
    if (closed) {
        return {};
    }

    std::shared_lock lock(menuMutex);

    return {menu.begin(), menu.end()};
}

std::vector<unsigned int> System::getPendingOrders() const {
    std::shared_lock lock(pendingOrdersMutex);

    return {pendingOrders.begin(), pendingOrders.end()};
}

std::unique_ptr<CoasterPager> System::order(std::vector<std::string> products) {
    static unsigned int id = 0;

    if (closed) {
        throw RestaurantClosedException();
    }

    if (products.empty()) {
        throw BadOrderException();
    }

    std::shared_lock lock2(menuMapMutex);
    for (auto const &product : products) {
        if (!menuMap[product]) {
            throw BadOrderException();
        }
    }
    lock2.unlock();

    std::promise<bool> visitPromise;
    std::promise<bool> orderReady;
    std::promise<std::vector<std::unique_ptr<Product>>> promiseOrder;
    std::future<bool> futureReady = orderReady.get_future();
    std::future<std::vector<std::unique_ptr<Product>>> futureProducts = promiseOrder.get_future();

    std::unique_lock lock(pendingOrdersMutex);
    auto it = pendingOrders.insert(pendingOrders.end(), id);
    lock.unlock();

    std::unique_lock<std::mutex> lock3(toPrepareMutex);
    ordersToPrepare.emplace(id, products, std::move(orderReady), std::move(promiseOrder), visitPromise.get_future(), it);
    waitingWorkers.notify_one();

    return std::unique_ptr<CoasterPager>(new CoasterPager(id++, std::move(futureReady), std::move(futureProducts), std::move(visitPromise)));
}

std::vector<std::unique_ptr<Product>>
System::collectOrder(std::unique_ptr<CoasterPager> CoasterPager) {
    if (CoasterPager == nullptr || !CoasterPager->futureProducts.valid()) {
        throw BadPagerException();
    }

    if (CoasterPager->isReady()) {
        CoasterPager->visitPromise.set_value(true);
        return CoasterPager->futureProducts.get();
    }

    CoasterPager->visitPromise.set_value(false);
    throw OrderNotReadyException();
}

unsigned int System::getClientTimeout() const {
    return clientTimeout;
}

void System::work(unsigned int reportId) {
    while (true) {
        std::unique_lock lock(toPrepareMutex);
        waitingWorkers.wait(lock, [&] { return !ordersToPrepare.empty() || closed; });
        if (ordersToPrepare.empty() && closed) {
            return;
        }
        orderToPrepare currentOrder = std::move(ordersToPrepare.front());
        ordersToPrepare.pop();

        std::vector<std::pair<std::future<std::unique_ptr<Product>>, std::string>> futureProducts;
        for (auto const &product : currentOrder.products) {
            std::promise<std::unique_ptr<Product>> promise;
            auto machine = machines.at(product);
            futureProducts.emplace_back(promise.get_future(), product);
            getMutexes.at(machine).first.lock();
            getQueues.at(machine).emplace(std::move(promise));
            getMutexes.at(machine).first.unlock();
            getMutexes.at(machine).second.notify_one();
        }
        lock.unlock();

        bool returnProducts = false;
        std::vector<std::pair<std::unique_ptr<Product>, std::shared_ptr<Machine>>> products;
        for (auto &futureProduct : futureProducts) {
            try {
                products.emplace_back(futureProduct.first.get(), machines.at(futureProduct.second));
            } catch (const MachineFailure& e) {
                report[reportId].failedProducts.push_back(futureProduct.second);
                returnProducts = true;
            }
        }

        if (returnProducts) {
            currentOrder.orderReady.set_exception(std::make_exception_ptr(FulfillmentFailure()));
            currentOrder.promiseOrder.set_exception(std::make_exception_ptr(FulfillmentFailure()));
            for (auto &product : products) {
                returnMutexes.at(product.second).first.lock();
                returnQueues.at(product.second).push(std::move(product.first));
                returnMutexes.at(product.second).first.unlock();
                returnMutexes.at(product.second).second.notify_one();
            }

            report[reportId].failedOrders.push_back(currentOrder.products);
            continue;
        }

        currentOrder.orderReady.set_value(true);

        if (currentOrder.futureVisit.wait_for(clientTimeout * 1ms) == std::future_status::timeout || !currentOrder.futureVisit.get()) {
            std::unique_lock lock2(pendingOrdersMutex);
            pendingOrders.erase(currentOrder.pendingIt);
            currentOrder.promiseOrder.set_exception(std::make_exception_ptr(OrderExpiredException()));
            lock2.unlock();

            for (auto &product : products) {
                returnMutexes.at(product.second).first.lock();
                returnQueues.at(product.second).push(std::move(product.first));
                returnMutexes.at(product.second).first.unlock();
                returnMutexes.at(product.second).second.notify_one();
            }

            report[reportId].abandonedOrders.push_back(currentOrder.products);
            continue;
        }

        std::unique_lock lock2(pendingOrdersMutex);
        pendingOrders.erase(currentOrder.pendingIt);
        lock2.unlock();

        std::vector<std::unique_ptr<Product>> correctProducts;

        correctProducts.reserve(products.size());
        for (auto &product : products) {
            correctProducts.push_back(std::move(product.first));
        }

        currentOrder.promiseOrder.set_value(std::move(correctProducts));
        report[reportId].collectedOrders.push_back(currentOrder.products);
    }
}

System::System(System::machines_t machines_, unsigned int numberOfWorkers_,
               unsigned int clientTimeout_) : machines(std::move(machines_)), numberOfWorkers(numberOfWorkers_), clientTimeout(clientTimeout_) {
    for (auto const &machine : machines) {
        menuMap[machine.first] = true;
        getMutexes.emplace(std::piecewise_construct, std::make_tuple(machine.second), std::make_tuple());
        getQueues.emplace(std::piecewise_construct, std::make_tuple(machine.second), std::make_tuple());
        returnMutexes.emplace(std::piecewise_construct, std::make_tuple(machine.second), std::make_tuple());
        returnQueues.emplace(std::piecewise_construct, std::make_tuple(machine.second), std::make_tuple());
        machine.second->start();
        getMachineThreads.emplace_back(&System::machineGet, this, machine.second, menu.insert(menu.end(), machine.first), machine.first);
        returnMachineThreads.emplace_back(&System::machineReturn, this, machine.second);
    }

    for (unsigned int i = 0; i < numberOfWorkers; i++) {
        report.emplace_back();
        workerThreads.emplace_back(&System::work, this, i);
    }
}

void System::machineGet(const std::shared_ptr<Machine>& machine, std::list<std::string>::iterator iterator, std::string productName) {
    bool erased = false;
    while (true) {
        std::unique_lock lock(getMutexes.at(machine).first);
        getMutexes.at(machine).second.wait(lock, [&] { return !getQueues.at(machine).empty() || stopMachines; });
        if (getQueues.at(machine).empty() && stopMachines) {
            return;
        }
        auto tmp = std::move(getQueues.at(machine).front());
        getQueues.at(machine).pop();
        lock.unlock();
        try {
            tmp.set_value(machine->getProduct());
        } catch (const MachineFailure &e) {
            std::unique_lock lock2(menuMutex);
            std::unique_lock lock3(menuMapMutex);
            menuMap[productName] = false;
            if (!erased) {
                menu.erase(iterator);
                erased = true;
            }
            tmp.set_exception(std::current_exception());
        }
    }
}

void System::machineReturn(const std::shared_ptr<Machine>& machine) {
    while (true) {
        std::unique_lock lock(returnMutexes.at(machine).first);
        returnMutexes.at(machine).second.wait(lock, [&] { return !returnQueues.at(machine).empty() || stopMachines; });
        if (returnQueues.at(machine).empty() && stopMachines) {
            return;
        }
        auto tmp = std::move(returnQueues.at(machine).front());
        returnQueues.at(machine).pop();
        lock.unlock();
        machine->returnProduct(std::move(tmp));
    }
}
