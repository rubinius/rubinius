set(soversion 1.0.0)

find_package(ZLIB)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

CHECK_FUNCTION_EXISTS(fseeko HAVE_FSEEKO)
CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)

set(HAVE_LIBZ ${ZLIB_FOUND})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(sources
  lib/zip_add.c lib/zip_close.c lib/zip_delete.c
  lib/zip_dirent.c lib/zip_entry_free.c lib/zip_entry_new.c
  lib/zip_err_str.c lib/zip_error.c lib/zip_error_get.c
  lib/zip_error_get_sys_type.c lib/zip_error_strerror.c
  lib/zip_error_to_str.c lib/zip_fclose.c
  lib/zip_file_error_get.c lib/zip_file_get_offset.c
  lib/zip_file_strerror.c lib/zip_fopen.c
  lib/zip_fopen_index.c lib/zip_fread.c lib/zip_free.c
  lib/zip_get_archive_comment.c lib/zip_get_file_comment.c
  lib/zip_get_num_files.c lib/zip_get_name.c
  lib/zip_memdup.c lib/zip_name_locate.c lib/zip_new.c
  lib/zip_open.c lib/zip_rename.c lib/zip_replace.c
  lib/zip_set_archive_comment.c lib/zip_set_file_comment.c
  lib/zip_source_buffer.c lib/zip_source_file.c
  lib/zip_source_filep.c lib/zip_source_free.c
  lib/zip_source_function.c lib/zip_source_zip.c
  lib/zip_set_name.c lib/zip_stat.c lib/zip_stat_index.c
  lib/zip_strerror.c lib/zip_unchange.c)

if(NOT HAVE_MKSTEMP)
  set(sources ${sources} lib/mkstemp.c)
endif(NOT HAVE_MKSTEMP)

add_library(zip SHARED ${sources})
set_target_properties(zip PROPERTIES SOVERSION ${soversion})
