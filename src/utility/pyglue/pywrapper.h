/*      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 *      
 *      Author: 
 *      Matias Fontanini
 * 
 */

#ifndef PYWRAPPER_H
#define PYWRAPPER_H

#include "utility/abort.h"
#include "type_cast.h"
#include "object.h"

namespace Python {

struct PyObjectDeleter {
    void operator()(PyObject* obj)
    {
        Py_XDECREF(obj);
    }
};
// unique_ptr that uses Py_XDECREF as the destructor function.
typedef std::unique_ptr<PyObject, PyObjectDeleter> pyunique_ptr;
class AnyObject : public Object {
public:
    AnyObject()
        : Object()
    {
    }
    AnyObject(const Object& obj)
        : Object(obj)
    {
    }
    AnyObject& operator=(const AnyObject& obj)
    {
        Object::operator=(obj);
        return *this;
    }
    template <typename T>
    AnyObject(T value)
        : Object(CastToPy(value))
    {
    }
    template <class T>
    bool Convert(T& value)
    {
        return Python::Convert(*this, value);
    }
    template <typename T>
    T As()
    {
        T value;
        if (!Convert(value))
            ERRORCODEABORT(ERR_VALUE_INVALID, "Fail to convert PyObject!");
        return value;
    }
    /**
         * \brief Constructs a Python::Object from a script string.
         * 
         * The returned Object will be the evaluation of the
         * script. If any errors are encountered while loading this 
         * script, an ERRORCODE is thrown.
         * 
         * \param script The string of the script to be evaluated.
         * \return Object representing the evaluated script.
         */
    void EvalScript(const std::string& script);
};

class ModuleObject : Object {
public:
    /**
         * \brief Constructs a default python object
         */
    ModuleObject()
        : Object()
    {
    }
    ModuleObject(const Object& obj)
        : Object(obj)
    {
    }
    ModuleObject& operator=(const ModuleObject& obj)
    {
        Object::operator=(obj);
        return *this;
    }

    /**
         * \brief Calls the callable attribute "name" using the provided
         * arguments.
         * 
         * This function might throw a ERRORCODE if there is
         * an error when calling the function.
         * 
         * \param name The name of the attribute to be called.
         * \param args The arguments which will be used when calling the
         * attribute.
         * \return Python::Object containing the result of the function.
         */
    //    template <typename... Args>
    //    Object CallFunction(const std::string& name, const Args&... args)
    //    {
    //        Object func = load_function(name);
    //        func.MakeSureNotNull();
    //        // Create the tuple argument
    //        Object tup = Object::Steal(PyTuple_New(sizeof...(args)));
    //        add_tuple_vars(tup, args...);
    //        // Call our object
    //        tup.Print();
    //        PyObject* result = PyObject_CallObject(func.Borrow(), tup.Borrow());
    //        Object ret = Object::Steal(result);
    //        MakeSureNoPyError(ERR_GENERAL);
    //        return ret;
    //    }
    //
    //    /**
    //         * \brief Calls a callable attribute using no arguments.
    //         *
    //         * This function might throw a ERRORCODE if there is
    //         * an error when calling the function.
    //         *
    //         * \sa Python::Object::CallFunction.
    //         * \param name The name of the callable attribute to be executed.
    //         * \return Python::Object containing the result of the function.
    //         */
    Object CallFunction(const std::string& name);
    //
    //    /**
    //         * \brief Finds and returns the attribute named "name".
    //         *
    //         * This function might throw a ERRORCODE if an error
    //         * is encountered while fetching the attribute.
    //         *
    //         * \param name The name of the attribute to be returned.
    //         * \return Python::Object representing the attribute.
    //         */
        Object GetAttr(const std::string& name);
    //
    //    /**
    //         * \brief Checks whether this object contains a certain attribute.
    //         *
    //         * \param name The name of the attribute to be searched.
    //         * \return bool indicating whether the attribute is defined.
    //         */
        bool HasAttr(const std::string& name);

    /**
         * \brief Returns the internal PyObject*.
         * 
         * No reference increment is performed on the PyObject* before
         * returning it, so any DECREF applied to it without INCREF'ing
         * it will cause undefined behaviour.
         * \return The PyObject* which this Object is representing.
         */

    /**
         * \brief Constructs a Python::Object from a script file.
         * 
         * The returned Object will be the representation of the loaded
         * script. If any errors are encountered while loading this 
         * script, an ERRORCODE is thrown.
         * 
         * \param script_path The path of the script to be loaded.
         * \return Object representing the loaded script.
         */
    void LoadModule(const std::string& script_path);
    //
    //private:
    //    Object load_function(const std::string& name);
    //
    //    // Variadic template method to add items to a tuple
    //    template <typename First, typename... Rest>
    //    void add_tuple_vars(Object& tup, const First& head, const Rest&... tail)
    //    {
    //        add_tuple_var(
    //            tup,
    //            PyTuple_Size(tup.Borrow()) - sizeof...(tail)-1,
    //            head);
    //        add_tuple_vars(tup, tail...);
    //    }
    //
    //    void add_tuple_vars(Object& tup, Object arg)
    //    {
    //        add_tuple_var(tup, PyTuple_Size(tup.Borrow()) - 1, arg);
    //    }
    //
    //    // Base case for add_tuple_vars
    //    template <typename Arg>
    //    void add_tuple_vars(Object& tup, const Arg& arg)
    //    {
    //        add_tuple_var(tup,
    //                      PyTuple_Size(tup.Borrow()) - 1, CastToPy(arg));
    //    }
    //
    //    // Adds a Object to the tuple object
    //    void add_tuple_var(Object& tup, Py_ssize_t i, Object pobj)
    //    {
    //        PyTuple_SetItem(tup.Borrow(), i, pobj.Steal());
    //    }
    //
    //    // Adds a PyObject* to the tuple object
    //    template <class T>
    //    void add_tuple_var(Object& tup, Py_ssize_t i,
    //                       const T& data)
    //    {
    //        Object item = CastToPy(data);
    //        PyTuple_SetItem(tup.Borrow(), i, item.Steal());
    //    }

    template <typename... Args>
    Object CallFunction(const std::string& name, const Args&... args)
    {
        pyunique_ptr func(load_function(name));
        // Create the tuple argument
        pyunique_ptr tup(PyTuple_New(sizeof...(args)));
        add_tuple_vars(tup, args...);
        // Call our object
        PyObject* result(PyObject_CallObject(func.get(), tup.get()));
        if (!result)
            throw std::runtime_error("Failed to call function " + name);
        Object ret = Object::Steal(result);
        return { ret };
    }

private:
    PyObject* load_function(const std::string& name);

    // Variadic template method to add items to a tuple
    template <typename First, typename... Rest>
    void add_tuple_vars(pyunique_ptr& tup, const First& head, const Rest&... tail)
    {
        add_tuple_var(
            tup,
            PyTuple_Size(tup.get()) - sizeof...(tail)-1,
            head);
        add_tuple_vars(tup, tail...);
    }

    void add_tuple_vars(pyunique_ptr& tup, PyObject* arg)
    {
        add_tuple_var(tup, PyTuple_Size(tup.get()) - 1, arg);
    }

    // Base case for add_tuple_vars
    template <typename Arg>
    void add_tuple_vars(pyunique_ptr& tup, const Arg& arg)
    {
        PyObject* item = CastToPy(arg).Steal();
        add_tuple_var(tup,
                      PyTuple_Size(tup.get()) - 1, item);
    }

    // Adds a PyObject* to the tuple object
    void add_tuple_var(pyunique_ptr& tup, Py_ssize_t i, PyObject* pobj)
    {
        PyTuple_SetItem(tup.get(), i, pobj);
    }

    // Adds a PyObject* to the tuple object
    template <class T>
    void add_tuple_var(pyunique_ptr& tup, Py_ssize_t i,
                       const T& data)
    {
        PyObject* item = CastToPy(data).Steal();
        PyTuple_SetItem(tup.get(), i, item);
    }
};
};

#endif // PYWRAPPER_H
