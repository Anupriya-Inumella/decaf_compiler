#include <vector>
using namespace std;
template<class Elem>
class Array {
    public:
    vector<Elem> elems;
    Array() {}
    int size() const {
        return elems.size();
    }
    void append(const Elem &elem) {
        elems.push_back(elem);
    }
    void pushAtFront(const Elem &elem) {
        elems.insert(elems.begin(), elem);
    }
    Elem elementAt(int index) const {
        return elems[index];
    }
};
