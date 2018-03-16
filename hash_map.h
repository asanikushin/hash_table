#include <vector>
#include <list>
#include <initializer_list>
#include <iostream>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
private:
    using Elems = typename std::pair<const KeyType, ValueType>;
    using MyList = typename std::list<Elems>;

    Hash hasher;
    std::size_t sz = 0;
    std::size_t alloc = 2;
    MyList data;
    std::vector<typename MyList::iterator> baskets;

    void rebuild() {
        HashMap tmp(hasher);
        tmp = *this;
        *this = tmp;
    }

    bool check(typename MyList::iterator place, std::size_t basket) const {
        if (place == data.end()) {
            return false;
        }
        return hasher(place->first) % alloc == basket;
    }

    void full_check(std::string text = "") const {
        std::size_t cnt = 0;
        for (std::size_t bask = 0; bask < alloc; ++bask) {
            typename MyList::iterator cur = baskets[bask];
            while (check(cur, bask)) {
                cnt++;
                cur++;
            }
        }
        if (cnt != sz) {
            std::cout << "Incorrect size:\nsize = " << sz << "; found = " << cnt << "\n";
            std::cout << text;
            std::cerr << "Incorrect size:\nsize = " << sz << "; found = " << cnt << "\n";
            std::cerr << text;
            exit(0);
        }
    }

public:
    HashMap(Hash _hasher) : hasher(_hasher), sz(0), alloc(2) {
        baskets.resize(alloc, data.end());
    }

    HashMap(std::size_t _alloc = 2, Hash _hasher = Hash()) : hasher(_hasher), sz(0), alloc(_alloc) {
        baskets.resize(alloc, data.end());
    }

    std::size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    using iterator = typename MyList::iterator;
    using const_iterator = typename MyList::const_iterator;

    const_iterator begin() const {
        return data.cbegin();
    }

    iterator begin() {
        return data.begin();
    }

    const_iterator end() const {
        return data.cend();
    }

    iterator end() {
        return data.end();
    }

    const_iterator find(KeyType key) const {
        std::size_t pos = hasher(key) % alloc;
        iterator cur = baskets[pos];
        while (check(cur, pos) && !(cur->first == key)) {
            cur++;
        }
        if (check(cur, pos)) {
            return cur;
        } else {
            return end();
        }
    }

    iterator find(KeyType key) {
        std::size_t pos = hasher(key) % alloc;
        iterator cur = baskets[pos];
        while (check(cur, pos) && !(cur->first == key)) {
            cur++;
        }
        if (check(cur, pos)) {
            return cur;
        } else {
            return end();
        }
    }

    void insert(std::pair<KeyType, ValueType> elem) {
        if (find(elem.first) == end()) {
            std::size_t pos = hasher(elem.first) % alloc;
            iterator cur = baskets[pos];
            baskets[pos] = data.insert(cur, elem);
            sz++;
        }
        if (sz * 2 > alloc) {
            rebuild();
        }
    }

    void erase(KeyType key) {
        iterator cur = find(key);
        if (cur != end()) {
            std::size_t pos = hasher(key) % alloc;
            if (baskets[pos] == cur) {
                baskets[pos] = data.erase(cur);
            } else {
                data.erase(cur);
            }
            if (!check(baskets[pos], pos)) {
                baskets[pos] = data.end();
            }
            sz--;
        }
    }

    ValueType &operator[](KeyType key) {
        insert({key, ValueType()});
        iterator cur = find(key);
        return cur->second;
    }

    const ValueType &at(KeyType key) const {
        const_iterator cur = find(key);
        if (cur == end()) {
            std::__throw_out_of_range("");
        }
        return cur->second;
    }

    void clear() {
        sz = 0;
        data.clear();
        baskets.assign(alloc, data.end());
    }


    template<class Iter>
    HashMap(Iter start, Iter end, Hash _hasher = Hash()) : hasher(_hasher), sz(0), alloc(2) {
        baskets.resize(2, data.end());
        for (; start != end; ++start) {
            insert({start->first, start->second});
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> d_, Hash _hasher = Hash()) : HashMap(d_.begin(),
                                                                                                      d_.end(),
                                                                                                      _hasher) {}

    HashMap(const HashMap &other) : HashMap(other.begin(), other.end(), other.hasher) {}

    HashMap &operator=(const HashMap &other) {
        if (this == &other) {
            return *this;
        }
        clear();
        alloc = std::min(other.alloc * 2 + 1, other.sz * 6);
        if (alloc == 0)
            alloc = 2;
        baskets.assign(alloc, data.end());
        for (auto x : other) {
            insert(x);
        }
        return *this;
    }
};
