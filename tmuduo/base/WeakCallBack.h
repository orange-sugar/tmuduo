#ifndef TMUDUO_BASE_WEAKCALLBACK_H
#define TMUDUO_BASE_WEAKCALLBACK_H

#include <functional>
#include <memory>

#include "tmuduo/base/Logging.h"

namespace tmuduo
{

template <typename C, typename... Args>
class WeakCallback
{
  public:
    WeakCallback(const std::weak_ptr<C>& object,
                 const std::function<void(C*, Args...)>& function)
      : object_(object), func_(function)
    { }

    void operator()(Args&&... args) const
    {
      std::shared_ptr<C> ptr(object_.lock());
      if (ptr)
      {
        func_(ptr.get(), std::forward<Args>(args)...);
      }
      // else 
      // {
      //   LOG_TRACE << "expired";
      // }
    }

  private:
    std::weak_ptr<C> object_;
    std::function<void(C*, Args...)> func_;
};

template<typename C, typename... Args>
WeakCallback<C, Args...> makeWeakCallback(const std::shared_ptr<C>& object,
                                          void (C::*funtion)(Args...))
{
  return WeakCallback<C, Args...>(object, funtion);
}

template<typename C, typename... Args>
WeakCallback<C, Args...> makeWeakCallback(const std::shared_ptr<C>& object,
                                          void (C::*function)(Args...) const)
{
  return WeakCallback<C, Args...>(object, function);
}         

} // namespace tmuduo                                 

#endif	// TMUDUO_BASE_WEAKCALLBACK_H