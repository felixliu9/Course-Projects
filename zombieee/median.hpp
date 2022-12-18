// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// felixliu

#include <vector>
#include <queue>

using namespace std;

class Median
{
private:
    priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> big;
    priority_queue<uint32_t> small;

public:
    void push(uint32_t i)
    {
        if (empty())
        {
            small.push(i);
        }
        else if (i > small.top())
        {
            big.push(i);
        }
        else
        {
            small.push(i);
        }
        if (small.size() > big.size() + 1)
        {
            big.push(small.top());
            small.pop();
        }
        else if (big.size() > small.size() + 1)
        {
            small.push(big.top());
            big.pop();
        }
    }

    bool empty()
    {
        return big.empty() && small.empty();
    }
    uint32_t get_median()
    {
        if (small.size() > big.size())
        {
            return small.top();
        }
        else if (big.size() > small.size())
        {
            return big.top();
        }
        else
        {
            return (small.top() + big.top()) / 2;
        }
    }
};