
#ifndef PARAMS_H
#define PARAMS_H

#include "handle.h"

bool Params_init();

struct Cursor;

bool Prepare(Cursor* cur, PyObject* pSql);
bool BindWithHandle(Cursor* cur, Handle *hndl, PyObject* original_params, bool skip_first);
bool Bind(Cursor* cur, PyObject* original_params, bool skip_first);
bool PrepareAndBind(Cursor* cur, PyObject* pSql, PyObject* params, bool skip_first);
bool ExecuteMulti(Cursor* cur, PyObject* pSql, PyObject* paramArrayObj);
bool GetParameterInfo(Cursor* cur, Py_ssize_t index, PyObject* param, ParamInfo& info, bool isTVP);
void FreeParameterData(Cursor* cur);
void FreeParameterInfo(Cursor* cur);

#endif
