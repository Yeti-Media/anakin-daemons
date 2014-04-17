#ifndef BLOCKINGMAP_HPP
#define BLOCKINGMAP_HPP

#include <map>
#include <semaphore.h>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

namespace Anakin
{

template <typename K, typename V> class BlockingMap
{

public:

    BlockingMap(V defaultValue)
    {
        if (sem_init (&this->wsem, 0, 1) != 0)
        {
            std::cout << "BlockingMap#BlockingMap: error initializing semaphore\n";
            exit(-1);
        }
        if (sem_init (&this->ssem, 0, 1) != 0)
        {
            std::cout << "BlockingMap#BlockingMap: error initializing semaphore\n";
            exit(-1);
        }
        this->internalMap = *new std::map<K, V>();
        this->defaultValue = defaultValue;
    }

    void setOverridingKey(K key)
    {
        sem_wait(&this->ssem);
        this->overridingKey = key;
        this->useOverridingKey = true;
        sem_post(&this->ssem);
    }

    void put(K key, V value)
    {
        sem_wait(&this->ssem);
        this->internalMap[key] = value;
        sem_post(&this->wsem);
        sem_post(&this->ssem);
    }

    V get(K key)
    {
        V value;
        while (!retrieveValue(key, &value))
        {
            sem_wait(&this->wsem);
        }
        return value;
    }

    bool exist(K key)
    {
        sem_wait(&this->ssem);
        return findKey(key);
        sem_post(&this->ssem);
    }

    std::vector<K> getKeys()
    {
        sem_wait(&this->ssem);
        std::vector<K> keys;
        std::pair<K,V> me;
        BOOST_FOREACH(me, this->internalMap)
        {
            keys.push_back(me.first);
        }
        sem_post(&this->ssem);
        return keys;
    }

    void unblock()
    {
        sem_wait(&this->ssem);
        this->blocking = false;
        sem_post(&this->wsem);
        sem_post(&this->ssem);
    }

protected:
private:
    bool retrieveValue(K key, V * value)
    {
        sem_wait(&this->ssem);
        if (findKey(key))
        {
            *value = this->internalMap.find(key)->second;
            sem_post(&this->ssem);
            return true;
        }
        else if (this->useOverridingKey && findKey(this->overridingKey))
        {
            *value = this->internalMap.find(this->overridingKey)->second;
            this->internalMap.erase(this->overridingKey);
            sem_post(&this->ssem);
            return true;
        }
        else
        {
            bool result = !this->blocking;
            if (result)
            {
                this->blocking = true;
            }
            *value = this->defaultValue;
            sem_post(&this->ssem);
            return result;
        }
    }

    bool findKey(K key)
    {
        if ( this->internalMap.find(key) == this->internalMap.end() )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    std::map<K, V> internalMap;
    sem_t wsem;
    sem_t ssem;
    V defaultValue;
    bool blocking = true;
    K overridingKey;
    bool useOverridingKey = false;
};
};

#endif // BLOCKINGMAP_HPP
