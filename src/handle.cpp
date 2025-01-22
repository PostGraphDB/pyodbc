// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Note: This project has gone from C++ (when it was ported from pypgdb) to C, back to C++ (where it will stay).  If
// you are making modifications, feel free to move variable declarations from the top of functions to where they are
// actually used.

#include "pyodbc.h"
#include "wrapper.h"
#include "textenc.h"
#include "cursor.h"
#include "pyodbcmodule.h"
#include "connection.h"
#include "row.h"
#include "params.h"
#include "errors.h"
#include "getdata.h"
#include "dbspecific.h"
#include <datetime.h>
#include "handle.h"

extern PyTypeObject HandleType;

static char messages_doc[] =
    "This read-only attribute is a list of all the diagnostic messages in the\n" \
    "current result set.";


/*
static PyMemberDef Cursor_members[] =
{
    {"rowcount",    T_INT,       offsetof(Cursor, rowcount),        READONLY, rowcount_doc },
    {"description", T_OBJECT_EX, offsetof(Cursor, description),     READONLY, description_doc },
    {"arraysize",   T_INT,       offsetof(Cursor, arraysize),       0,        arraysize_doc },
    {"connection",  T_OBJECT_EX, offsetof(Cursor, cnxn),            READONLY, connection_doc },
    {"fast_executemany",T_BOOL,  offsetof(Cursor, fastexecmany),    0,        fastexecmany_doc },
    {"messages",    T_OBJECT_EX, offsetof(Cursor, messages),        READONLY, messages_doc },
    { 0 }
};

*/



static char enter_doc[] = "__enter__() -> self.";
static PyObject* Handle_enter(PyObject* self, PyObject* args)
{
    UNUSED(args);
    Py_INCREF(self);
    return self;
}

static char exit_doc[] = "__exit__(*excinfo) -> None.  Commits the connection if necessary..";
static PyObject* Handle_exit(PyObject* self, PyObject* args)
{
    //Cursor* cursor = Cursor_Validate(self, CURSOR_REQUIRE_OPEN | CURSOR_RAISE_ERROR);
    //if (!cursor)
    //    return 0;


    Py_RETURN_NONE;
}


static PyMethodDef Handle_methods[] =
{
    {"__enter__",        Handle_enter,                         METH_NOARGS,                enter_doc            },
    {"__exit__",         Handle_exit,                          METH_VARARGS,               exit_doc             },
    {0, 0, 0, 0}
};

static char cursor_doc[] =
    "TODO";
PyTypeObject CursorType =
{
    PyVarObject_HEAD_INIT(0, 0)
    "pyodbc.Handle",                                        // tp_name
    sizeof(Handle),                                         // tp_basicsize
    0,                                                      // tp_itemsize
    (destructor)Handle_dealloc,                             // destructor tp_dealloc
    0,                                                      // tp_print
    0,                                                      // tp_getattr
    0,                                                      // tp_setattr
    0,                                                      // tp_compare
    0,                                                      // tp_repr
    0,                                                      // tp_as_number
    0,                                                      // tp_as_sequence
    0,                                                      // tp_as_mapping
    0,                                                      // tp_hash
    0,                                                      // tp_call
    0,                                                      // tp_str
    0,                                                      // tp_getattro
    0,                                                      // tp_setattro
    0,                                                      // tp_as_buffer
    0,
    0,                                             // tp_doc
    0,                                                      // tp_traverse
    0,                                                      // tp_clear
    0,                                                      // tp_richcompare
    0,                                                      // tp_weaklistoffset
    0,                               // tp_iter
    0,                          // tp_iternext
    0,                                         // tp_methods
    0,                                         // tp_members
    0,                                      // tp_getset
    0,                                                      // tp_base
    0,                                                      // tp_dict
    0,                                                      // tp_descr_get
    0,                                                      // tp_descr_set
    0,                                                      // tp_dictoffset
    0,                                                      // tp_init
    0,                                                      // tp_alloc
    0,                                                      // tp_new
    0,                                                      // tp_free
    0,                                                      // tp_is_gc
    0,                                                      // tp_bases
    0,                                                      // tp_mro
    0,                                                      // tp_cache
    0,                                                      // tp_subclasses
    0,                                                      // tp_weaklist
};

Handle*
Handle_New(Cursor* cur)
{
    // Exported to allow the connection class to create cursors.

#ifdef _MSC_VER
#pragma warning(disable : 4365)
#endif
    Handle* hndl = PyObject_NEW(Handle, &HandleType);
#ifdef _MSC_VER
#pragma warning(default : 4365)
#endif

    if (hndl)
    {
        Connection cnxn = cur->cnxn;
        hndl->cur              = cur;
        hndl->hstmt             = SQL_NULL_HANDLE;


        SQLRETURN ret;
        Py_BEGIN_ALLOW_THREADS
        ret = SQLAllocHandle(SQL_HANDLE_STMT, cnxn->hdbc, &hndl->hstmt);
        Py_END_ALLOW_THREADS

        if (!SQL_SUCCEEDED(ret))
        {
            RaiseErrorFromHandle(cnxn, "SQLAllocHandle", cnxn->hdbc, SQL_NULL_HANDLE);
            Py_DECREF(cur);
            return 0;
        }

        if (cnxn->timeout)
        {
            Py_BEGIN_ALLOW_THREADS
            ret = SQLSetStmtAttr(cur->hstmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)(uintptr_t)cnxn->timeout, 0);
            Py_END_ALLOW_THREADS

            if (!SQL_SUCCEEDED(ret))
            {
                RaiseErrorFromHandle(cnxn, "SQLSetStmtAttr(SQL_ATTR_QUERY_TIMEOUT)", cnxn->hdbc, cur->hstmt);
                Py_DECREF(cur);
                return 0;
            }
        }

        TRACE("handle.new cnxn=%p hdbc=%d cursor=%p hstmt=%d\n", (Connection*)cur->cnxn, ((Connection*)cur->cnxn)->hdbc, cur, cur->hstmt);
    }

    return hndl;
}

