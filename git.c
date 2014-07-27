/*
 * Author:     Long Changjin <admin@longchangjin.cn>
 */

#include <stdio.h>
#include <git2.h>
#include <Python.h>

typedef struct {
    PyObject_HEAD
    git_repository *rep;
} lcGitRepository;

static void Repository_dealloc(lcGitRepository *self);
int m_repository_status_cb(char* path, git_status_t status, void* obj);

static PyObject* m_repository_status(PyObject *self);

static PyObject* m_git_threads_shutdown(PyObject *self);
static PyObject* m_git_threads_init(PyObject *self);


static PyMethodDef repository_methods[] = {
    {"status", (PyCFunction)m_repository_status, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};


static int Repository_init(lcGitRepository *self, PyObject *args, PyObject *kwds)
{
    int error;
    char *path;


    if (!PyArg_ParseTuple(args, "s", &path)) {
        printf("git_repository_open need a path arg！\n");
        return -1;
    }

    error = git_repository_open(&self->rep, path);
    if (error < 0) {
        const git_error *e = giterr_last();
        printf("Error %d/%d: %s\n", error, e->klass, e->message);
        return -1;
    }

    return 0;
}


static void Repository_dealloc(lcGitRepository* self)
{
    lcGitRepository *rep = (lcGitRepository *)self;
    if (rep->rep) {
        git_repository_free(rep->rep);
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

int m_repository_status_cb(char* path, git_status_t status, void* obj)
{
    PyDict_SetItemString((PyObject *)obj, path, PyInt_FromLong(status));
    return 0;
}

static PyObject* m_repository_status(PyObject *self)
{
    lcGitRepository *rep = (lcGitRepository *)self;
    PyObject *dict = NULL;
    int error;

    if (!rep->rep) {
        Py_RETURN_NONE;
    }

    dict = PyDict_New();
    if (!dict) {
        Py_RETURN_NONE;
    }

    error = git_status_foreach(rep->rep, m_repository_status_cb, dict);
    if (error < 0) {
        const git_error *e = giterr_last();
        printf("Error %d/%d: %s\n", error, e->klass, e->message);
        Py_RETURN_NONE;
    }
    return dict;
}

static PyTypeObject lcgit_RepositoryType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lcgit.Repository",      /*tp_name*/
    sizeof(lcGitRepository), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Repository_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /*tp_flags*/
    "Git Repository",          /*tp_doc*/
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    repository_methods,        /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Repository_init,      /* tp_init */
    0,                         /* tp_alloc */
};



static PyMethodDef lcgit_methods[] = {
    {"git_threads_init", (PyCFunction)m_git_threads_init, METH_NOARGS, ""},
    {"git_threads_shutdown", (PyCFunction)m_git_threads_shutdown, METH_NOARGS, ""},
    {NULL}  /* Sentinel */
};

PyMODINIT_FUNC initlcgit(void)
{
    PyObject *m;
    m = Py_InitModule3("lcgit", lcgit_methods, "");

    PyModule_AddIntMacro(m, GIT_STATUS_CURRENT);
    PyModule_AddIntMacro(m, GIT_STATUS_IGNORED);
    PyModule_AddIntMacro(m, GIT_STATUS_INDEX_DELETED);
    PyModule_AddIntMacro(m, GIT_STATUS_INDEX_MODIFIED);
    PyModule_AddIntMacro(m, GIT_STATUS_INDEX_NEW);
    PyModule_AddIntMacro(m, GIT_STATUS_WT_DELETED);
    PyModule_AddIntMacro(m, GIT_STATUS_WT_MODIFIED);


    lcgit_RepositoryType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&lcgit_RepositoryType) < 0)
    {
        return;
    }

    Py_INCREF(&lcgit_RepositoryType);
    PyModule_AddObject(m, "Repository", (PyObject *)&lcgit_RepositoryType);
}

static PyObject* m_git_threads_init(PyObject *self)
{
    git_threads_init();
    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject* m_git_threads_shutdown(PyObject *self)
{
    git_threads_shutdown();
    Py_INCREF(Py_True);
    return Py_True;
}
