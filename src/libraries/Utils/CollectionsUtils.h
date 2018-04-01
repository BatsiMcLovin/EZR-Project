//
// Created by rheidrich on 08.08.17.
//

#ifndef EZR_PROJECT_COLLECTIONSUTILS_H
#define EZR_PROJECT_COLLECTIONSUTILS_H


#include <vector>
#include <map>
#include <algorithm>
#include "debug.h"

namespace ezr
{
    class CollectionsUtils
    {
    public:
        template<typename T, typename V>
        static std::vector<V> getAllValues(std::map<T,V>& inputMap);
    };

    template<typename T, typename V>
    std::vector<V> CollectionsUtils::getAllValues(std::map<T, V>& inputMap)
    {
        std::vector<V> output;
        for(auto value : inputMap)
        {
            output.push_back(value.second);
        }
/*        std::for_each(std::begin(inputMap), std::end(inputMap),
                      [&output](std::map<T, V>::value_type& p)
                      {
                          output.push_back(p.second);
                      }
        );*/

        return output;
    }
}


#endif //EZR_PROJECT_COLLECTIONSUTILS_H
