#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

class object_t;

template<typename T>
void draw(const T& x, ostream& out, size_t position)
{
    out << string(position, ' ') << x << endl;
}

// by creating classes that behave like regular objects we can make them more reusable
class object_t
{
public:
    template <typename T>
    object_t(T x) : self_(make_unique<model<T>>(move(x))) 
    {
    }
    //this time we have to write our own copy ctor and assign operator as the compiler constructed is not 
    //enough (copies won't be disjoint if copied using compile supplied ctor)
    //when going to many types we have to virtualize copy on the concept
    object_t(const object_t& o) : self_(o.self_->copy_())
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
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void draw_(ostream&, size_t) const = 0;
        virtual unique_ptr<concept_t> copy_() const = 0;
    };
    template <typename T>
    struct model final : concept_t {
        model(T x) : data_(move(x)) {}
        unique_ptr<concept_t> copy_() const override
        {
            return make_unique<model>(*this);
        }
        
        void draw_(ostream& out, size_t position) const override
        {
            draw(data_, out, position);
        }
        T data_;
    };

    unique_ptr<concept_t> self_;
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

class my_class_t {

};

void draw(const my_class_t&, ostream& out, size_t position)
{
    out << string(position, ' ') << "my_class_t" << endl;
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
    //after adding string_model_t we can  add string to the structure
    document.emplace_back(string("Hello!"));
    document.emplace_back(2);
    document.emplace_back(my_class_t());
    document.emplace_back(document);
    reverse(document.begin(), document.end());

    draw(document, cout, 0);
}