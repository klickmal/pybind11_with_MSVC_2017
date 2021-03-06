// test.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <numeric>
#include <vector>

#include <pybind11\pybind11.h>
#include <pybind11\embed.h>
#include <pybind11\iostream.h>
#include <pybind11\numpy.h>
#include <pybind11\stl.h>

namespace py = pybind11;

void test_numpy_1D()
{
    try
    {
        auto a = py::array_t<int>(10);
        int* data = (int*)a.request().ptr;

        for (py::ssize_t i = 0; i < a.shape(0); ++i)
        {
            data[i] = (int) i;
        }

        auto np = py::module::import("numpy");
        auto sum = np.attr("sum")(a).cast<int>();
        assert(sum = 45);

    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void test_numpy_2D()
{
    try
    {
        int x = 10;
        int y = 8;

        auto a = py::array_t<int>({x,y});
        assert(a.ndim() == 2);
        assert(a.shape(0) == 10);
        assert(a.shape(1) == 8);
        assert(a.size() == 80);

        // direct access
        auto r = a.mutable_unchecked<2>();

        for (py::ssize_t i = 0; i < r.shape(0); ++i)
        {
            for (py::ssize_t j = 0; j < r.shape(1); ++j)
            {
                r(i, j) = 1;
            }
        }

        auto np = py::module::import("numpy");
        auto sum = np.attr("sum")(a).cast<int>();
        assert(sum = 80);
    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }
}

template<typename It>
void fill_me(It it, It end)
{
    for (; it != end; ++it)
    {
        (*it) = 42;
    }
}

void test_numpy()
{
    try
    {
        auto np = py::module::import("numpy");
        auto arange_func = np.attr("arange");
        auto reshape_func = np.attr("reshape");

        auto a = (py::array) arange_func(15);
        auto sum = np.attr("sum")(a).cast<int>();
        
        assert(a.ndim() == 1);
        assert(a.shape(0) == 15);
        assert(a.size() == 15);
        assert(sum == 105);

        py::tuple new_shape = py::make_tuple(3, 5);
        a = reshape_func(a, new_shape);
        sum = np.attr("sum")(a).cast<int>();

        assert(a.ndim() == 2);
        assert(a.shape(0) == 3);
        assert(a.shape(1) == 5);
        assert(a.size() == 15);
        assert(sum == 105);
    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void test_numpy_stl()
{
    try
    {
        auto np = py::module::import("numpy");

        py::array_t<int> b(10);
        int* it = (int*) b.request().ptr;
        auto end = (int*) (b.request().ptr) + b.size();
        std::fill(it, end, 1);
        auto sum_b = std::accumulate(it, end, 0);
    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void test_matrix()
{
    try
    {
        auto np = py::module::import("numpy");
    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void test_raw_memory()
{
    std::vector<std::int64_t> a(10, 20);

    py::buffer_info buf(a.data()
        , sizeof(std::int64_t)
        , py::format_descriptor<std::int64_t>::format()
        , 1
        , { a.size() }
        , { sizeof(std::int64_t) });

    py::array aa(buf);

    auto np = py::module::import("numpy");
    auto sum = np.attr("sum")(aa).cast<int>();
}

void test_module();


int main()
{
    test_module();

    return 0;

    py::scoped_interpreter guard{};

    auto g = py::globals();
    for (auto it = g.begin(); it != g.end(); ++it)
    {
        std::cout << it->first << " ---- " << it->second << std::endl;
    }

    auto fast_calc = py::module::import("fast_calc");
    auto result = fast_calc.attr("add")(1, 2).cast<int>();
    assert(result == 3);

    try
    {
        //fast_calc.attr("decipher_args")(1.0);

        py::exec(R"(
            decipher_args(1.0)
        )");
    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }



    /////////////////////////////

    auto chh_module = py::module::import("chh");

    py::print("Hello, World!");

    try
    {
        py::exec(R"(
            import chh
            import pickle
            import numpy as np
            m = chh.Matrix(4,4)
            print(m.sum())

            s = pickle.dumps(m)            
            print(s)

            n = pickle.loads(s)
        )", py::globals());

        g = py::globals();
        for (auto it = g.begin(); it != g.end(); ++it)
        {
            std::cout << it->first << " ---- " << it->second << std::endl;
        }

    }
    catch (const py::error_already_set& e)
    {
        std::cout << e.what() << std::endl;
    }





    test_numpy_1D();
    test_numpy_2D();
    test_numpy();
    test_numpy_stl();
    test_raw_memory();

    return 0;
}

