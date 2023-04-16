#include "ioc_template.h"
struct IX {
    virtual ~IX() { };
    virtual void g() = 0;
};

class X : public IX {
public:
     void g() override{
        cout << " it is a test in x" <<endl;
    }
};

class Y : public IX {
public:
    Y(int a) : m_a(a) { };
    void g() override{
        cout << "it is a test in y" <<endl;
    }
    int m_a;
};

struct MyStructA {
    MyStructA(IX* x) :  m_x(x){ };
    ~MyStructA() {
        if (m_x != nullptr) {
            delete m_x;
            m_x = nullptr;
        }
    }
    void Func() {
        m_x->g();
    }
private:
    IX* m_x;
};

int main() {
    // 直接创建
//    MyStructA* pa = new MyStructA(new X());
//    delete pa;
// ioc 创建
    IocContainer ioc;
    ioc.RegisterType<MyStructA, X>("A");
    auto pa = ioc.ResolveShared<MyStructA>("A");
    pa->Func();
}