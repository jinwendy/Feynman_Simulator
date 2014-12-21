//
//  object.h
//  Feynman_Simulator
//
//  Created by Kun Chen on 12/21/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//

#ifndef __Feynman_Simulator__object__
#define __Feynman_Simulator__object__

/*
   Make sure you have carefully gone through the following explaination about reference count behavior in python before touch following codes
*/
/*
   The reference count behavior of functions in the Python/C API is best explained in terms of ownership of references. Ownership pertains to references, never to objects (objects are not owned: they are always shared). “Owning a reference” means being responsible for calling Py_DECREF on it when the reference is no longer needed. Ownership can also be transferred, meaning that the code that receives ownership of the reference then becomes responsible for eventually decref’ing it by calling Py_DECREF() or Py_XDECREF() when it’s no longer needed—or passing on this responsibility (usually to its caller). When a function passes ownership of a reference on to its caller, the caller is said to RECEIVE a new reference. When no ownership is transferred, the caller is said to BORROW the reference. Nothing needs to be done for a borrowed reference.

   Conversely, when a calling function passes in a reference to an object, there are two possibilities: the function steals a reference to the object, or it does not. TakeOvering a reference means that when you pass a reference to a function, that function assumes that it now owns that reference, and you are not responsible for it any longer.
 
   Few functions steal references; the two notable exceptions are PyList_SetItem() and PyTuple_SetItem(), which steal a reference to the item (but not to the tuple or list into which the item is put!). These functions were designed to steal a reference because of a common idiom for populating a tuple or list with newly created objects. 
 */
#include <string>
#include "utility/convention.h"

// forward declare PyObject
// as suggested on the python mailing list
// http://mail.python.org/pipermail/python-dev/2003-August/037601.html
#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif
namespace Python {
void Initialize();
void Finalize();
void PrintError();
void ClearError();
void MakeSureNoPyError(ERRORCODE);
enum Ownership {
    //Object with received ownership from others has to
    //manage the reference count of PyObject
    //namely, call Py_DECREF() or Py_XDECREF()
    Received = 0,
    Borrowed
};
class Object {
public:
    Object(Ownership ownership = Received)
        : _OwnerShip(ownership)
        , _PyPtr(nullptr){};
    Object(PyObject* ptr, Ownership ownership = Received);
    Object(const Object&, Ownership ownership = Received);
    ~Object() { Destroy(); }
    void Destroy();
    PyObject* GetPtr() const { return _PyPtr; }
    Ownership GetOwnerShip() const { return _OwnerShip; }
    void Print();
    std::string PrettyString();
    void MakeSureNotNull();

protected:
    PyObject* _PyPtr;
    Ownership _OwnerShip;

private:
    Object& operator=(const Object& a);
};
}

#endif /* defined(__Feynman_Simulator__object__) */
