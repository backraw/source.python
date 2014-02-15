/**
* =============================================================================
* Source Python
* Copyright (C) 2012 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/
#ifndef _WRAP_MACROS_H
#define _WRAP_MACROS_H

//---------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------
#include "boost/python.hpp"

//---------------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------------
using namespace boost::python;

//---------------------------------------------------------------------------------
// Define checks
//---------------------------------------------------------------------------------

#if !defined(SOURCE_ENGINE)
#error("SOURCE_ENGINE define must be globally defined, did we forget?");
#endif

#if !defined(SOURCE_GAME)
#error("SOURCE_GAME define must be globally defined, did we forget?");
#endif

//---------------------------------------------------------------------------------
// Surround boost python statements with this macro in order to handle exceptions.
//---------------------------------------------------------------------------------
#define BEGIN_BOOST_PY() \
	try {

#define END_BOOST_PY( ... ) \
	} catch( ... ) { \
		PyErr_Print(); \
		PyErr_Clear(); \
		return __VA_ARGS__; \
	}

#define END_BOOST_PY_NORET( ... ) \
	} catch( ... ) { \
		PyErr_Print(); \
		PyErr_Clear(); \
	}

//---------------------------------------------------------------------------------
// This macro will turn input into a string.
//---------------------------------------------------------------------------------
#define XSTRINGIFY(s) STRINGIFY(s)
#define STRINGIFY(s) #s


//---------------------------------------------------------------------------------
// This macro returns the given overloaded method
//---------------------------------------------------------------------------------
#define GET_METHOD(return_type, class_name, method, ...) \
	static_cast< return_type (class_name::*)( __VA_ARGS__ ) >(&class_name::method)

//---------------------------------------------------------------------------------
// This macro returns the given overloaded function
//---------------------------------------------------------------------------------
#define GET_FUNCTION(return_type, function, ...) \
	static_cast< return_type(*)( __VA_ARGS__ ) >(&function)

//---------------------------------------------------------------------------------
// Use this to wrap a method. Variable arg is for any return value policies
// you might have. Since we're in the namespace, we have access to the class.
//---------------------------------------------------------------------------------
#define CLASS_METHOD( classname, methodname, ... ) \
	.def(XSTRINGIFY(methodname), &classname::methodname, ##__VA_ARGS__ )

//---------------------------------------------------------------------------------
// Use this macro to generate function overloads for classes that have methods
// with default parameters.
//---------------------------------------------------------------------------------
#define DECLARE_CLASS_METHOD_OVERLOAD( classname, method, minargs, maxargs ) \
	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(classname##_##method, method, minargs, maxargs)

//---------------------------------------------------------------------------------
// Use this macro inside a BOOST_*_CLASS block to create a class method with an
// overload. Make sure you have DECLARE_CLASS_METHOD_OVERLOAD'd the overload
// first!
//---------------------------------------------------------------------------------
#define CLASS_METHOD_OVERLOAD( classname, methodname, ... ) \
	.def(XSTRINGIFY(methodname), &classname::methodname, classname##_##methodname( __VA_ARGS__ ))

//---------------------------------------------------------------------------------
// Use this macro for boost class overloads that have a return policy.
//---------------------------------------------------------------------------------
#define CLASS_METHOD_OVERLOAD_RET( classname, methodname, docstring, args, retpol ) \
	.def(XSTRINGIFY(methodname), &classname::methodname, \
		 classname##_##methodname( args, docstring )[retpol])

//---------------------------------------------------------------------------------
// Use this macro to define a function or class method that raises a
// NotImplementedError. This is quite hacky, but saves a lot work!
//---------------------------------------------------------------------------------
#define NOT_IMPLEMENTED_RAW() \
	eval( \
		"lambda *args, **kw: exec('raise NotImplementedError(\"Not implemented on this engine.\")')", \
		import("__main__").attr("__dict__") \
	)

#define NOT_IMPLEMENTED(name) \
	def( \
		name, \
		NOT_IMPLEMENTED_RAW(), \
		"\nNot implemented on this engine.\n" \
	)

#define NOT_IMPLEMENTED_ATTR(name) \
	add_property(name, NOT_IMPLEMENTED_RAW(), NOT_IMPLEMENTED_RAW(), "\nNot implemented on this engine.\n")

//---------------------------------------------------------------------------------
// Use this macro to expose a variadic class method. Don't forget to call
// DEFINE_CLASS_METHOD_VARIADIC after that.
//---------------------------------------------------------------------------------
#define CLASS_METHOD_VARIADIC(classname, method, ...) \
	.def("__" #method, &classname::method, ##__VA_ARGS__)

//---------------------------------------------------------------------------------
// Use this macro to define a variadic class method.
//---------------------------------------------------------------------------------
#define DEFINE_CLASS_METHOD_VARIADIC(classname, method) \
	scope().attr(#classname).attr(#method) = eval("lambda self, *args: self.__" #method "(args)")

//---------------------------------------------------------------------------------
// Use this macro to raise a Python exception.
//---------------------------------------------------------------------------------
#define BOOST_RAISE_EXCEPTION( exceptionName, exceptionString ) \
    { \
		PyErr_SetString(exceptionName, exceptionString); \
		throw_error_already_set(); \
    }

//---------------------------------------------------------------------------------
// Use this macro to add a specialization for a class to hold back-references.
//---------------------------------------------------------------------------------
#define BOOST_SPECIALIZE_HAS_BACK_REFERENCE( classname ) \
	namespace boost { namespace python { \
		template<> \
		struct has_back_reference<classname> : mpl::true_ \
		{}; \
	}}

//---------------------------------------------------------------------------------
// These macros allow us to includ engine / game specific files
//---------------------------------------------------------------------------------

// Has to be a C++ macro so we can use another define
// directly after the call to concatenate the objects
#define GET_ENGINE_VALUE() engine

// Current working directory macros
#define JOIN_PATH(folder, file_path)		XSTRINGIFY(folder/file_path)
#define GAME_INCLUDE_PATH(file_path)		JOIN_PATH(SOURCE_GAME, file_path)
#define ENGINE_INCLUDE_PATH(file_path)		JOIN_PATH(GET_ENGINE_VALUE()SOURCE_ENGINE, file_path)

//---------------------------------------------------------------------------------
// These typedefs save some typing. Use this policy for any functions that return
// a newly allocated instance of a class which you need to delete yourself.
//---------------------------------------------------------------------------------
typedef return_value_policy<manage_new_object> manage_new_object_policy;

//---------------------------------------------------------------------------------
// Use this policy for objects that someone else will free.
//---------------------------------------------------------------------------------
typedef return_value_policy<reference_existing_object> reference_existing_object_policy;

//---------------------------------------------------------------------------------
// Use this policy for functions that return const objects.
//---------------------------------------------------------------------------------
typedef return_value_policy<copy_const_reference> copy_const_reference_policy;

#endif // _WRAP_MACROS_H
