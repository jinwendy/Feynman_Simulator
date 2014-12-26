//
//  serialization_test.cpp
//  Feynman_Simulator
//
//  Created by Kun Chen on 12/17/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//
#include <initializer_list>
#include <iostream>
#include <limits>
#include "utility/sput.h"
#include "utility/complex.h"
#include "utility/pyglue/pyarraywrapper.h"
#include "dictionary.h"

using namespace std;
using namespace Python;

void Test_Ref();
void Test_Cast();
void Test_Dict();
int TestDictionary()
{
    sput_start_testing();
    sput_enter_suite("Test Python wrapper layer");
    sput_run_test(Test_Ref);
    sput_run_test(Test_Cast);
    sput_run_test(Test_Dict);
    sput_finish_testing();
    return sput_get_return_value();
}

void Test_Ref()
{
    PyObject* iptr = PyInt_FromLong(1);
    auto i_ref = iptr->ob_refcnt;
    AnyObject i1 = iptr;
    sput_fail_unless(i1.RefCount() == i_ref, "create AnyObject from PyObject* who owns the reference");
    AnyObject i2 = Object(iptr, NoRef);
    sput_fail_unless(i2.RefCount() == i_ref + 1, "create AnyObject from PyObject* who does not own the reference");
    AnyObject i3 = i2;
    sput_fail_unless(i3.RefCount() == i_ref + 2, "create AnyObject with copy constructor");
    i2.Destroy();

    AnyObject j = 2;
    auto j_ref = j.RefCount();
    i3 = j;
    //all new reference to iptr has been destroyed except i1
    sput_fail_unless(iptr->ob_refcnt == i_ref, "the old AnyObject has to call Py_DEREF");
    sput_fail_unless(i3.RefCount() == j_ref + 1, "the new AnyObject own an new reference");
    i3.Destroy();

    PyObject* jptr = j.Get(NewRef);
    sput_fail_unless(jptr->ob_refcnt == j_ref + 1, "get PyObject* with new reference");
    Py_XDECREF(jptr);
    PyObject* jjptr = j.Get(NoRef);
    sput_fail_unless(jjptr->ob_refcnt == j_ref, "get PyObject* witout reference");
}

void Test_Cast()
{
    //    Complex a[] = { { 1.0, 2.0 }, { 3.0, 1.0 }, { 1.0, 1.0 }, { 2.0, 1.0 } };
    //    uint Shape[] = { 2, 2 };
    //    ArrayObject array(a, Shape, 2);
    //    array.Print();
    //    cout << array.RefCount() << endl;
    //    auto t = (Complex*)array.Data();
    //    cout << t[0] << ", " << t[2] << endl;
    //    cout << array.RefCount() << endl;
    //    array.Destroy();
    //    cout << a[0] << ", " << a[2] << endl;
    //
    AnyObject value;
    int IntMax = std::numeric_limits<int>::max();
    value = IntMax;
    sput_fail_unless(IntMax == value.As<int>(), "Int Max");
    int IntMin = std::numeric_limits<int>::min();
    value = IntMin;
    sput_fail_unless(IntMin == value.As<int>(), "Int Min");

    long long lliMax = std::numeric_limits<long long>::max();
    value = lliMax;
    sput_fail_unless(lliMax == value.As<long long>(), "Long Long Int Max");
    long long lliMin = std::numeric_limits<long long>::min();
    value = lliMin;
    sput_fail_unless(lliMin == value.As<long long>(), "Long Long Int Min");
    unsigned long long biggest = std::numeric_limits<unsigned long long>::max();
    value = biggest;
    sput_fail_unless(biggest == value.As<unsigned long long>(),
                     "Unsigned Long Long Int Max");
}

void Test_Dict()
{
    Dictionary Port;
    unsigned long long biggest = std::numeric_limits<unsigned long long>::max();
    Port["biggest"] = biggest;
    vector<int> v = { 1, 2, 3 };
    Port["Vec"] = v;
    sput_fail_unless(Port["biggest"].As<unsigned long long>() == biggest,
                     "check unsigned long long integer type");
    auto vect = Port["Vec"].As<vector<int> >();
    sput_fail_unless(std::equal(v.begin(), v.end(),
                                vect.begin()),
                     "check vector<int> type");
    Complex ca = { 1.0, 2.0 };
    Complex cb = { 4.0, 2.1 };
    vector<Complex> vc = { ca, cb };
    Port["cVec"] = vc;
    Complex cc = (Port["cVec"].As<vector<Complex> >())[1];
    sput_fail_unless(Equal(cc, cb), "check vector<Complex> type");
    Dictionary SubPort;
    SubPort.LoadFromString("{'b':11,'c':22, 'r':False}");
    Port["dict"] = SubPort;
    sput_fail_unless((Port["dict"].As<Dictionary>())["b"].As<int>() == 11,
                     "check dict type");
    Port.Print();
    Port.Save("test.txt", "w");
    Port.Clear();
    Port.Load("test.txt");
    sput_fail_unless(Equal((Port["cVec"].As<vector<Complex> >())[1], cb),
                     "check vector<Complex> type");
    sput_fail_unless((Port["dict"].As<Dictionary>())["b"].As<int>() == 11,
                     "check dict IO");
    system("rm test.txt");
    SubPort.Clear();
    SubPort = Port["dict"].As<Dictionary>();
    SubPort.Print();
}