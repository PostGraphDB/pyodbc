/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HANDLE_H
#define HANDLE_H

struct Cursor;

struct Handle
{
    PyObject_HEAD

    // The Cursor object (which is a PyObject) that created this handle.
    Cursor* cur;

    // Set to SQL_NULL_HANDLE when the cursor is closed.
    HSTMT hstmt;

    void *vector;
};

void Handle_init();

Handle* Handle_New(Cursor* cur);

extern PyTypeObject HandleType;

#define Handle_Check(op) PyObject_TypeCheck(op, &HandleType)
#define Handle_CheckExact(op) (Py_TYPE(op) == &HandleType)


#endif
