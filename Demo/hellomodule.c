#include <Python.h>

static PyObject*
say_hello(PyObject* self, PyObject* args)
{
	    const char* name;

	        if (!PyArg_ParseTuple(args, "s", &name))
			        return NULL;

		    printf("Hello %s!\n", name);

		        Py_RETURN_NONE;
}

static PyMethodDef HelloMethods[] =
{
	     {"List_Disk_Contents", List_Disk_Contents, METH_VARARGS, "Greet somebody."},
	     {"Report_Disk_Status", Report_Disk_Status, METH_VARARGS, "ort_Disk_Status somebody."},
	     {"Report_Cache_Status", Report_Cache_Status, METH_VARARGS, "Greet somebody."},
	     {"Delete_File", Delete_File, METH_VARARGS, "Greet somebody."},
	     {"Extract_File", Extract_File, METH_VARARGS, "Greet somebody."},
	     {"Enter_File", Enter_File, METH_VARARGS, "Greet somebody."},
	     {"Get_Deduplication_Status", Get_Deduplication_Status, METH_VARARGS, "Greet somebody."},
	     {"Exit_FS", Exit_FS, METH_VARARGS, "Greet somebody."},
	     {"Initialize_FSCS", Initialize_FSCS, METH_VARARGS, "Greet somebody."},
	     {"Initialize_FS_In_Drive", Initialize_FS_In_Drive, METH_VARARGS, "Greet somebody."},
	     {"Read_File_Sequentially", Read_File_Sequentially, METH_VARARGS, "Greet somebody."},
	     {"Read_File_Block", Read_File_Block, METH_VARARGS, "Greet somebody."},
	     {"Set_Cache_Flag", say_hello, METH_VARARGS, "Greet somebody."},
	     {"Set_Deduplication_Flag", Set_Deduplication_Flag, METH_VARARGS, "Greet somebody."},
	     {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
inithello(void)
{
	     (void) Py_InitModule("hello", HelloMethods);
}
