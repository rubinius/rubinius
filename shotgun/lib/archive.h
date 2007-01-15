OBJECT archive_list_files(STATE, char *path);
OBJECT archive_get_file(STATE, char *path, char *name);
OBJECT archive_get_object(STATE, char *path, char* name);
OBJECT archive_add_file(STATE, char *path, char *name, char *file);
OBJECT archive_add_object(STATE, char *path, char *name, OBJECT obj);
OBJECT archive_delete_file(STATE, char *path, int idx);

