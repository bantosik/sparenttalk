#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

class object_t;

void draw(const int& x, ostream& out, size_t position)
{
    out << string(position, ' ') << x << endl;
}

// by creating classes that behave like regular objects we can make them more reusable
class object_t
{
public:
    object_t(int x) : self_(make_unique<int_model_t>(move(x))) 
    {
    }
    //this time we have to write our own copy ctor and assign operator as the compiler constructed is not 
    //enough (copies won't be disjoint if copied using compile supplied ctor)
    object_t(const object_t& o) : self_(make_unique<int_model_t>(*o.self_))
    {
    }
    //not all c++11 compliant compilers will treat operator=(object_t) as a cause to elide copy when object_t is copied in enclosing data structure
    object_t& operator=(const object_t& x)
    {
        return *this = object_t(x);
    }
    object_t& operator=(object_t&&) noexcept = default;
    //to benefit from move semantics related algos optimization we have to supply our own move constructor as it is not 
    //created
    object_t(object_t&& x) noexcept = default;
    friend void draw(const object_t& x, ostream& out, size_t position)
    { x.self_->draw_(out, position); }
private:
    struct int_model_t {
        int_model_t(int x) : data_(move(x)) {}
        void draw_(ostream& out, size_t position) const
        {
            draw(data_, out, position);
        }
        int data_;
    };
    unique_ptr<int_model_t> self_;
};

using document_t = vector<object_t>;

void draw(const document_t& x, ostream& out, size_t position)
{
    out << string(position, ' ') << "<document>" << endl;
    for(const auto& e: x) draw(e, out, position + 2);
    out << string(position, ' ') << "</document>" << endl;
}

struct some_t
{
    object_t member_;
};

some_t func() 
{
    return { 5 };
}

int main()
{
    cout << "some_t test start" << endl;
    some_t x = { 0 };
    x = func();    
    cout << "some_t test end" << endl;
    document_t document;
    document.reserve(5);
       
    document.emplace_back(0);
    document.emplace_back(1);
    document.emplace_back(2);
    document.emplace_back(3);
    reverse(document.begin(), document.end());

    draw(document, cout, 0);
}