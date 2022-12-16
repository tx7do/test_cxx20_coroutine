# test_cxx20_coroutine

GCC 10以及以上支持，以下的版本不支持。

需要包含`-fcoroutines`标识符，否则不能使用协程功能。

cmake当中加入标识符有以下方式：

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_compile_options(<target> "-fcoroutines")

target_compile_options(<target> PRIVATE "-fcoroutines")

target_add_compile_flags_if_supported(<target> PRIVATE -fcoroutines)
```

## 参考资料

- [Your first coroutine](https://blog.panicsoftware.com/your-first-coroutine/)
