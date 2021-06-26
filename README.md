# ToySTL

## Introduction

项目是一个基于 C++ 11 的 ToySTL，目的是为了学习。项目参考了《STL源码剖析》和 SGI STL 源码，基本完成了STL 的六大组件，并在此基础上增加了一些 C++ 11 的接口。使用 gtest 进行测试。

## Development Plan

- [x] 空间配置器
- [x] 容器
  - [x] vector
  - [x] list
  - [x] deque
  - [x] set/multiset
  - [x] map/multimap
  - [x] unordered_set/unordered_multiset
  - [x] unordered_map/unordered_multimap
- [x] 算法
- [x] 迭代器
- [x] 仿函数
- [x] 适配器

## Requirements

OS : Linux

G++ >=  5.4.0.

CMake >= 3.17.1.

## Build

下载:

```shell
$ git clone git@github.com:keep99/toystl.git
$ cd toystl
```

编译：

```shell
$ ./build.sh
```

or

```shell
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Test

### Test

- [x] vector
- [x] list
- [x] deque

......

### Performance

- [x] vector
- [ ] list
- [ ] deque

......

## Code Style

遵循 Google 代码规范，并且用 cpplint.py 来进行检查。

```shell
$ python cpplint.py + 待检测的文件
```

## TO DO

完善测试用例

## Reference

《STL 源码剖析》

[Alinshans/MyTinySTL: Achieve a tiny STL in C++11 (github.com)](https://github.com/Alinshans/MyTinySTL)

[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)