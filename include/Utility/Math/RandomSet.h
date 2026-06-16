#pragma once

#include <vector>
#include <unordered_map>

namespace bc
{
    template<typename T>
    class RandomSet
    {
    public:
        void Insert(T* e)
        {
            //assert(!Contains(e));
            mMap.emplace(e, mVec.size());
            mVec.push_back(e);
        }

        void Erase(T* e)
        {
            auto target = mMap.find(e);
            //assert(target != mMap.end());
            auto eIdx = target->second;
            if (eIdx != mVec.size() - 1)
            {
                mMap[mVec.back()] = eIdx;
                mVec[eIdx] = mVec.back();
            }
            mVec.pop_back();
            mMap.erase(target);
        }

        auto Begin() noexcept { return mVec.begin(); }
        auto Begin() const noexcept { return mVec.begin(); }
        auto End() noexcept { return mVec.end(); }
        auto End() const noexcept { return mVec.end(); }

        T* operator[](std::size_t i) const noexcept { return mVec[i]; }

        T* At(std::size_t i) const { return mVec.at(i); }

        std::size_t Size() const noexcept { return mVec.size(); }

        void Reserve(std::size_t n) {
            mVec.reserve(n);
            mMap.reserve(n);
        }
        void Clear() noexcept {
            mVec.clear();
            mMap.clear();
        }

        const std::vector<T*>& Vec() const noexcept { return mVec; }

        std::size_t Idx(T* e) const { return mMap.at(e); }

        bool Contains(T* e) const { return mMap.find(e) != mMap.end(); }

        bool Empty() const noexcept { return mVec.empty(); }
    private:
        std::unordered_map<T*, size_t> mMap;
        std::vector<T*> mVec;
    };
}

