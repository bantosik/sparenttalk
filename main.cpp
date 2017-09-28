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
    object_t(const int& x) : self_(make_unique<int_model_t>(x)) 
    {
        cout << "ctor" << endl;
    }
    //this time we have to write our own copy ctor and assign operator as the compiler constructed is not 
    //enough (copies won't be disjoint if copied using compile supplied ctor)
    object_t(const object_t& o) : self_(make_unique<int_model_t>(*o.self_))
    {
        cout << "copy" << endl;
    }
    object_t& operator=(object_t o) noexcept
    {
        self_ = move(o.self_);
        return *this;
    }
    //to benefit from move semantics related algos optimization we have to supply our own move constructor as it is not 
    //created
    object_t(object_t&& x) noexcept = default;
    friend void draw(const object_t& x, ostream& out, size_t position)
    { x.self_->draw_(out, position); }
private:
    struct int_model_t {
        int_model_t(const int& x) : data_(x) {}
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

int main()
{
    document_t document;
    document.reserve(5);
       
    document.emplace_back(0);
    document.emplace_back(1);
    document.emplace_back(2);
    document.emplace_back(3);
    reverse(document.begin(), document.end());

    draw(document, cout, 0);
}