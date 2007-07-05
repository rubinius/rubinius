#ifndef RBS_ARCHIVE_H
#define RBS_ARCHIVE_H

typedef void archive_handle_t;

archive_handle_t *archive_open(STATE, const char *path);
OBJECT archive_close(STATE, archive_handle_t *archive);
OBJECT archive_get_file2(STATE, archive_handle_t *archive, const char *name);
OBJECT archive_get_object2(STATE, archive_handle_t *archive, const char *name, int version);

OBJECT archive_list_files(STATE, char *path);
OBJECT archive_get_file(STATE, const char *path, const char *name);
OBJECT archive_get_object(STATE, const char *path, char* name, int version);
OBJECT archive_add_file(STATE, char *path, char *name, char *file);
OBJECT archive_add_object(STATE, char *path, char *name, OBJECT obj, int version);
OBJECT archive_delete_file(STATE, char *path, int idx);

#endif
