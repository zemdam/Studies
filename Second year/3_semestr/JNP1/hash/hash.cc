#include "hash.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef NDEBUG
bool const debug = false;
#else
bool const debug = true;
#endif

using jnp1::hash_function_t;
using std::cerr;
using std::endl;
using std::size_t;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using Sequence = vector<uint64_t>;

namespace {
    struct MyHash {
        hash_function_t hashFunction;

        explicit MyHash(hash_function_t hashFunction_) {
            hashFunction = hashFunction_;
        }

        size_t operator()(Sequence const &sequence) const noexcept {
            return hashFunction((sequence.data()), sequence.size());
        }
    };

    using HashTable = unordered_set<Sequence, MyHash>;

    // Solve for the static initialization order fiasco.
    unordered_map<unsigned long, HashTable> &hashMap() {
        static unordered_map<unsigned long, HashTable> hashMap;
        return hashMap;
    }

    // Same as above.
    unordered_set<unsigned long> &indexes() {
        static unordered_set<unsigned long> indexes;
        return indexes;
    }

    unsigned long getLowestIndex() {
        if (indexes().empty()) {
            return hashMap().size();
        }

        unsigned long lowestIndex = *(indexes().begin());
        indexes().erase(indexes().begin());
        return lowestIndex;
    }

    void printSequence(uint64_t const *seq, size_t size) {
        cerr << "\"";
        if (size > 0) {
            cerr << seq[0];
        }

        for (size_t i = 1; i < size; i++) {
            cerr << " " << seq[i];
        }

        cerr << "\"";
    }

    void startDebug(const string &functionName, hash_function_t hash_function) {
        if (!debug) {
            return;
        }

        cerr << functionName << "(" << &hash_function << ")" << endl;
    }

    void startDebug(const string &functionName, unsigned long id) {
        if (!debug) {
            return;
        }

        cerr << functionName << "(" << id << ")" << endl;
    }

    void startDebug(const string &functionName, unsigned long id,
                    uint64_t const *seq, size_t size) {
        if (!debug) {
            return;
        }

        cerr << functionName << "(" << id << ", ";
        if (seq == nullptr) {
            cerr << "NULL, " << size << ")" << endl;
            return;
        }

        printSequence(seq, size);
        cerr << ", " << size << ")" << endl;
    }

    void printFunction(const string &functionName) {
        cerr << functionName << ": ";
    }

    void printHashTable(unsigned long id) {
        cerr << "hash table #" << id;
    }

    bool isIdCorrect(const string &functionName, unsigned long id) {
        auto toCheck = hashMap().find(id);
        if (toCheck != hashMap().end()) {
            return true;
        }

        if (debug) {
            printFunction(functionName);
            printHashTable(id);
            cerr << " does not exist" << endl;
        }

        return false;
    }

    bool areParametersCorrect(const string &functionName, unsigned long id,
                              uint64_t const *seq, size_t size) {
        bool areCorrect = true;

        if (seq == nullptr) {
            if (debug) {
                printFunction(functionName);
                cerr << "invalid pointer (NULL)" << endl;
            }
            areCorrect = false;
        }

        if (size == 0) {
            if (debug) {
                printFunction(functionName);
                cerr << "invalid size (0)" << endl;
            }
            areCorrect = false;
        }

        if (areCorrect) {
            areCorrect = isIdCorrect(functionName, id);
        }

        return areCorrect;
    }

    void endDebug(const string &functionName, unsigned long id,
                  const string &ending) {
        if (!debug) {
            return;
        }

        printFunction(functionName);
        printHashTable(id);
        cerr << ending << endl;
    }

    void endDebug(const string &functionName, unsigned long id,
                  uint64_t const *seq, size_t size, bool is,
                  const string &ifTrue, const string &ifFalse) {
        if (!debug) {
            return;
        }

        printFunction(functionName);
        printHashTable(id);
        cerr << ", sequence ";
        printSequence(seq, size);
        if (is) {
            cerr << ifTrue;
        } else {
            cerr << ifFalse;
        }
        cerr << endl;
    }
} // namespace

namespace jnp1 {
    unsigned long hash_create(hash_function_t hash_function) {
        startDebug(__func__, hash_function);

        unsigned long index = getLowestIndex();
        unordered_set<Sequence, MyHash> hashTable(0, MyHash(hash_function));
        hashMap().insert({index, hashTable});

        endDebug(__func__, index, " created");

        return index;
    }

    void hash_delete(unsigned long id) {
        startDebug(__func__, id);

        if (!isIdCorrect(__func__, id)) {
            return;
        }

        auto toDelete = hashMap().find(id);
        hashMap().erase(toDelete);
        indexes().insert(id);

        endDebug(__func__, id, " deleted");
    }

    size_t hash_size(unsigned long id) {
        startDebug(__func__, id);

        if (!isIdCorrect(__func__, id)) {
            return 0;
        }

        auto toCheck = hashMap().find(id);
        size_t size = toCheck->second.size();

        endDebug(__func__, id,
                 " contains " + std::to_string(size) + " element(s)");

        return size;
    }

    bool hash_insert(unsigned long id, uint64_t const *seq, size_t size) {
        startDebug(__func__, id, seq, size);

        if (!areParametersCorrect(__func__, id, seq, size)) {
            return false;
        }

        auto toInsert = hashMap().find(id);
        Sequence sequence(seq, seq + size);
        bool isInserted = toInsert->second.insert(sequence).second;

        endDebug(__func__, id, seq, size, isInserted, " inserted",
                 " was present");

        return isInserted;
    }

    bool hash_remove(unsigned long id, uint64_t const *seq, size_t size) {
        startDebug(__func__, id, seq, size);

        if (!areParametersCorrect(__func__, id, seq, size)) {
            return false;
        }

        auto toInsert = hashMap().find(id);
        Sequence sequence(seq, seq + size);
        bool isRemoved = toInsert->second.erase(sequence);

        endDebug(__func__, id, seq, size, isRemoved, " removed",
                 " was not present");

        return isRemoved;
    }

    void hash_clear(unsigned long id) {
        startDebug(__func__, id);

        if (!isIdCorrect(__func__, id)) {
            return;
        }

        auto toClear = hashMap().find(id);
        if (toClear->second.empty()) {
            endDebug(__func__, id, " was empty");
            return;
        }

        toClear->second.clear();

        endDebug(__func__, id, " cleared");
    }

    bool hash_test(unsigned long id, uint64_t const *seq, size_t size) {
        startDebug(__func__, id, seq, size);

        if (!areParametersCorrect(__func__, id, seq, size)) {
            return false;
        }

        auto toTest = hashMap().find(id);
        Sequence sequence(seq, seq + size);
        bool isPresent = toTest->second.count(sequence);

        endDebug(__func__, id, seq, size, isPresent, " is present",
                 " is not present");

        return isPresent;
    }
} // namespace jnp1
