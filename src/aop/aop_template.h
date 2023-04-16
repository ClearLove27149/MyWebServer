//
// Created by 26685 on 2023/3/24.
//

#ifndef MYWEBSERVER_AOP_TEMPLATE_H
#define MYWEBSERVER_AOP_TEMPLATE_H
#include <iostream>

#define HAS_MEMBER(member) \
template<typename T, typename... Args> struct has_member_##member { \
    private:               \
    template<typename U> static auto Check(int) -> decltype(std::declval<U>(). \
        member(std::declval<Args>()...), std::true_type());        \
    template<typename U> static std::false_type Check(...);         \
    public:                \
    enum {value = std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};
// std::declval 返回右值引用类型，类的或函数返回值的等
HAS_MEMBER(Foo)
HAS_MEMBER(Before)
HAS_MEMBER(After)

#include "asio/detail/noncopyable.hpp"
template<typename Func, typename... Args>
struct Aspect : public asio::noncopyable {
    Aspect(Func&& f) : m_func(std::forward<Func>(f)) { };
    template<typename T>
    typename std::enable_if<has_member_Before<T, Args...>::value && has_member_After<T, Args...>::value>::type \
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename T>
    typename std::enable_if<has_member_Before<T, Args...>::value && !has_member_After<T, Args...>::value>::type \
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
    }

    template<typename T>
    typename std::enable_if<has_member_After<T, Args...>::value && !has_member_Before<T, Args...>::value>::type \
    Invoke(Args&&... args, T&& aspect) {
        m_func(std::forward<Args>(args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename Head, typename... Tail>
    void Invoke(Args&&... args, Head&& headAspect, Tail&&... tailAspect) {
        headAspect.Before(std::forward<Args>(args)...);
        Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
        headAspect.After(std::forward<Args>(args)...);
    }

private:
    Func m_func;
};

template<typename T> using identify_t = T;
template<typename... AP, typename... Args, typename Func>
void Invoke(Func&& f, Args&&... args) {
    Aspect<Func, Args...> asp(std::forward<Func>(f));
    asp.Invoke(std::forward<Args>(args)..., identify_t<AP>()...);
}
#endif //MYWEBSERVER_AOP_TEMPLATE_H
