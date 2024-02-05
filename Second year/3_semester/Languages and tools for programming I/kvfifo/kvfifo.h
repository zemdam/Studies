#ifndef KVFIFO_KVFIFO_H
#define KVFIFO_KVFIFO_H

#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

template<typename K, typename V>
class kvfifo {
private:
    using kv_list = std::list<std::pair<K, V>>;
    using kv_kmap = std::map<K, std::list<typename kv_list::iterator>>;

    class k_iterator {
    public:
        using value_type = K;
        using difference_type = kv_kmap::iterator::difference_type;

        k_iterator() = default;

        explicit k_iterator(typename kv_kmap::iterator _it) : it(_it) {
        }

        const K &operator*() const noexcept {
            return it->first;
        }

        K *operator->() noexcept {
            return &(it->first);
        }

        k_iterator &operator++() noexcept {
            it++;
            return *this;
        }

        k_iterator operator++(int) noexcept {
            k_iterator result(*this);
            operator++();
            return result;
        }

        k_iterator &operator--() noexcept {
            it--;
            return *this;
        }

        k_iterator operator--(int) noexcept {
            k_iterator result(*this);
            operator--();
            return result;
        }

        friend bool operator==(k_iterator const &a,
                               k_iterator const &b) noexcept {
            return a.it == b.it;
        }

        friend bool operator!=(k_iterator const &a,
                               k_iterator const &b) noexcept {
            return !(a.it == b.it);
        }

    private:
        typename kv_kmap::iterator it;
    };

    static_assert(std::bidirectional_iterator<k_iterator>);

    class data {
    public:
        kv_list dataList;
        kv_kmap keyMap;

        class pushGuard {
        private:
            data *dataPtr;
            typename kv_list::iterator itDataList;
            typename kv_kmap::iterator itKeyMap;
            bool rollbackDataList;
            bool rollbackKeyMap;

        public:
            pushGuard(data *dataPtr, K const &key, V const &value)
                : dataPtr(dataPtr), rollbackDataList(false),
                  rollbackKeyMap(false) {
                dataPtr->dataList.push_back({key, value});
                itDataList = dataPtr->dataList.end();
                --itDataList;
                rollbackDataList = true;

                if (dataPtr->keyMap.count(key) == 0) {
                    dataPtr->keyMap.emplace(
                            key, std::list<typename kv_list::iterator>{});
                }
                itKeyMap = dataPtr->keyMap.find(key);
                itKeyMap->second.push_back(itDataList);
                rollbackKeyMap = true;
            }

            void dropRollback() noexcept {
                rollbackDataList = false;
                rollbackKeyMap = false;
            }

            ~pushGuard() noexcept {
                if (rollbackKeyMap) {
                    itKeyMap->second.pop_front();
                }

                if (rollbackDataList) {
                    dataPtr->dataList.pop_front();
                }
            }
        };

        class moveBackGuard {
        private:
            data *dataPtr;
            typename kv_list::iterator toErase;
            typename kv_list::iterator placed;
            typename kv_list::iterator &listElement;
            bool rollback;

        public:
            moveBackGuard(data *dataPtr, typename kv_list::iterator &iterator)
                : dataPtr(dataPtr), toErase(iterator), listElement(iterator),
                  rollback(false) {
                dataPtr->dataList.push_back(*iterator);
                placed = dataPtr->dataList.end();
                --placed;
                rollback = true;
            }

            void dropRollback() noexcept {
                rollback = false;
            }

            ~moveBackGuard() noexcept {
                if (rollback) {
                    dataPtr->dataList.erase(placed);
                } else {
                    listElement = placed;
                    dataPtr->dataList.erase(toErase);
                }
            }
        };
    };

    void detach() {
        if (dataPtr.use_count() > 1) {
            data newData;
            for (auto const &it: dataPtr.get()->dataList) {
                auto key = it.first;
                newData.dataList.push_back({key, it.second});

                if (newData.keyMap.count(key) == 0) {
                    newData.keyMap.emplace(
                            key, std::list<typename kv_list::iterator>{});
                }
                auto itKeyMap = newData.keyMap.find(key);
                auto toInsert = newData.dataList.end();
                --toInsert;
                itKeyMap->second.push_back(toInsert);
            }
            dataPtr = std::make_shared<data>(std::move(newData));
        }
    }

    void isEmpty() const {
        if (this->size() == 0) {
            throw std::invalid_argument("");
        }
    }

    void isKeyInMap(K const &k) const {
        auto it = dataPtr->keyMap.find(k);
        if (it == dataPtr->keyMap.end() || it->second.size() == 0) {
            throw std::invalid_argument("");
        }
    }

public:
    ~kvfifo() noexcept = default;
    kvfifo() : isReferenced(false), dataPtr(std::make_shared<data>()) {
    }
    kvfifo(kvfifo const &other) : isReferenced(false), dataPtr(other.dataPtr) {
        if (other.isReferenced) {
            detach();
        }
    }
    kvfifo(kvfifo &&) noexcept = default;

    kvfifo &operator=(kvfifo other) noexcept {
        std::swap(other.dataPtr, dataPtr);

        return *this;
    }

    void push(K const &k, V const &v) {
        isReferenced = false;

        detach();

        typename data::pushGuard insert(dataPtr.get(), k, v);
        insert.dropRollback();
    }

    void pop() {
        isReferenced = false;

        isEmpty();

        detach();

        auto it = dataPtr->dataList.front();
        K key = it.first;
        if (dataPtr->keyMap.find(key)->second.size() <= 1) {
            dataPtr->keyMap.erase(key);
        } else {
            dataPtr->keyMap.find(key)->second.pop_front();
        }
        dataPtr->dataList.pop_front();
    }

    void pop(K const &k) {
        isReferenced = false;

        isKeyInMap(k);

        detach();

        auto it = dataPtr->keyMap.find(k);
        ;
        auto toDelete = it->second.front();
        dataPtr->dataList.erase(toDelete);
        it->second.pop_front();
        if (it->second.size() == 0) {
            dataPtr->keyMap.erase(it);
        }
    }

    void move_to_back(K const &k) {
        isReferenced = false;

        isKeyInMap(k);

        detach();

        auto toMove = dataPtr->keyMap.find(k);
        std::list<typename data::moveBackGuard> guards;
        for (auto &it: toMove->second) {
            guards.emplace_back(dataPtr.get(), it);
        }

        for (auto &guard: guards) {
            guard.dropRollback();
        }
    }

    std::pair<K const &, V &> front() {
        isEmpty();

        detach();

        std::pair<K const &, V &> toReturn(dataPtr->dataList.front().first,
                                           dataPtr->dataList.front().second);
        isReferenced = true;
        return toReturn;
    }

    std::pair<K const &, V const &> front() const {
        isEmpty();

        std::pair<K const &, V const &> toReturn(
                dataPtr->dataList.front().first,
                dataPtr->dataList.front().second);
        return toReturn;
    }

    std::pair<K const &, V &> back() {
        isEmpty();

        detach();

        std::pair<K const &, V &> toReturn(dataPtr->dataList.back().first,
                                           dataPtr->dataList.back().second);
        isReferenced = true;
        return toReturn;
    }

    std::pair<K const &, V const &> back() const {
        isEmpty();

        std::pair<K const &, V const &> toReturn(
                dataPtr->dataList.back().first,
                dataPtr->dataList.back().second);
        return toReturn;
    }

    std::pair<K const &, V &> first(K const &key) {
        isKeyInMap(key);

        detach();

        auto it = dataPtr->keyMap.find(key);
        ;
        std::pair<K const &, V &> toReturn((*it->second.front()).first,
                                           (*it->second.front()).second);
        isReferenced = true;
        return toReturn;
    }

    std::pair<K const &, V const &> first(K const &key) const {
        isKeyInMap(key);

        auto it = dataPtr->keyMap.find(key);
        ;
        std::pair<K const &, V const &> toReturn((*it->second.front()).first,
                                                 (*it->second.front()).second);
        return toReturn;
    }

    std::pair<K const &, V &> last(K const &key) {
        isKeyInMap(key);

        detach();

        auto it = dataPtr->keyMap.find(key);
        std::pair<K const &, V &> toReturn((*it->second.back()).first,
                                           (*it->second.back()).second);
        isReferenced = true;
        return toReturn;
    }

    std::pair<K const &, V const &> last(K const &key) const {
        isKeyInMap(key);

        auto it = dataPtr->keyMap.find(key);
        std::pair<K const &, V const &> toReturn((*it->second.back()).first,
                                                 (*it->second.back()).second);
        return toReturn;
    }

    std::size_t size() const noexcept {
        return dataPtr.get()->dataList.size();
    }

    bool empty() const noexcept {
        return dataPtr->keyMap.empty();
    }

    std::size_t count(K const &k) const {
        if (dataPtr->keyMap.count(k) == 0) {
            return 0;
        }
        return dataPtr->keyMap.find(k)->second.size();
    }

    void clear() {
        detach();
        dataPtr.get()->dataList.clear();
        dataPtr.get()->keyMap.clear();
    }

    k_iterator k_begin() const {
        return k_iterator(dataPtr->keyMap.begin());
    }

    k_iterator k_end() const {
        return k_iterator(dataPtr->keyMap.end());
    }

private:
    bool isReferenced;
    std::shared_ptr<data> dataPtr;
};

#endif //KVFIFO_KVFIFO_H
