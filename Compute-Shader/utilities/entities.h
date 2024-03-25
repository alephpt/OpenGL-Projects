#pragma once
#include <vector>
#include <cstdint>

struct SearchResult {
    size_t position;
    int flag;
};

template<typename T>
class Entities{
    public:
        std::vector<T> instantiations;
        std::vector<uint32_t> entities;

        T& get(uint32_t entity) {
            SearchResult query = binSearch(entity);
            return instantiations[query.position];
        }

        void insert(uint32_t entity, T component) {
            // We could come up with a better insert than this.
            entities.push_back(entity);
            instantiations.push_back(component);

            size_t i = entities.size() - 1;

            while (i > 0 && entities[i - 1] > entity) {
                entities[i] = entities[i - 1];
                instantiations[i] = instantiations[i - 1];
                --i;
            }

            entities[i] = entity;
            instantiations[i] = component;
        }

        void remove(uint32_t entity) {
            SearchResult query = binSearch(entity);
            entities.erase(entities.begin() + query.position);
            instantiations.erase(instantiations.begin() + query.position);
        }

    private:
        SearchResult binSearch(__uint128_t entity) {
            SearchResult a;
            a.position = 0;
            a.flag = std::min<int>(1, std::max<int>(-1, entity));
            if (a.flag == 0) { return a; }

            SearchResult b;
            b.position = entities.size() - 1;
            b.flag = std::min<int>(1, std::max<int>(-1, entities[b.position] - entity));
            if (b.flag == 0) { return b; }

            while ((a.flag * b.flag < 0) && (b.position - a.position > 1)) {
                SearchResult c;
                c.position = (a.position + b.position) / 2;
                c.flag = std::min<int>(1, std::max<int>(-1, entities[c.position] - entity));
                if (c.flag == 0) { return c;}

                if (a.flag * c.flag < 0) { b = c; }
                else { a = c; }
            }

            if (b.flag == 0) { return b; }
            
            return a;
        }
};