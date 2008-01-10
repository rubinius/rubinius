# depends on: ffi.rb

module Platform::POSIX

  # errors
  attach_function 'ffi_errno', :errno, [], :int
  attach_function 'ffi_set_errno', :errno=, [:int], :void
  attach_function 'strerror', [:int], :string
  attach_function 'perror', [:string], :void

  # memory
  attach_function 'malloc', [:int], :pointer
  attach_function 'realloc', [:pointer, :int], :pointer
  attach_function 'free', [:pointer], :void
  attach_function 'memset', [:pointer, :int, :int], :pointer
  attach_function 'memcpy', [:pointer, :pointer, :int], :pointer

  # rand
  attach_function 'srand', [:uint], :void
  attach_function 'rand',  [], :int

  # file system
  attach_function 'access', [:string, :int], :int
  attach_function 'chmod',  [:string, :int], :int
  attach_function 'fchmod', [:int, :int], :int
  attach_function 'unlink', [:string], :int
  attach_function 'getcwd', [:string, :int], :string
  attach_function 'umask', [:int], :int
  attach_function 'link', [:string, :string], :int
  attach_function 'symlink', [:string, :string], :int
  attach_function 'readlink', [:string, :string, :int], :int
  attach_function 'rename', [:string, :string], :int
  attach_function 'utimes', [:string, :pointer], :int

  # directories
  attach_function 'chdir', [:string], :int
  attach_function 'opendir',  [:string], :pointer
  attach_function 'readdir',  [:pointer], :pointer
  attach_function 'rewinddir',  [:pointer], :void
  attach_function 'closedir', [:pointer], :int
  attach_function 'mkdir', [:string, :short], :int
  attach_function 'rmdir', [:string], :int

  # File/IO
  #   opening/closing
  attach_function 'fdopen', [:int, :string], :pointer
  attach_function 'fopen',  [:string, :string], :pointer
  attach_function 'fclose', [:pointer], :int

  #   buffering
  attach_function 'fflush', [:pointer], :int

  #   inspecting
  attach_function 'fileno', [:pointer], :int
  attach_function 'feof',   [:pointer], :int
  attach_function 'ferror', [:pointer], :int
  attach_function 'clearerr', [:pointer], :void
  attach_function 'fseek',  [:pointer, :int, :int], :int
  attach_function 'ftell',  [:pointer], :int

  #   reading
  attach_function 'fread',   [:string, :int, :int, :pointer], :int
  attach_function 'fgets',   [:string, :int, :pointer], :void
  attach_function 'fgetc',   [:pointer], :int

  #   writing
  attach_function 'fwrite',  [:string, :int, :int, :pointer], :int
  attach_function 'ungetc',  [:int, :pointer], :int

  #   formatted strings
  attach_function 'ffi_sprintf_f', :sprintf_f, [:double, :int, :string], :strptr
  attach_function 'ffi_sprintf_d', :sprintf_d, [:int, :int, :string], :strptr

  # UID/GID
  gid_t = :uint
  uid_t = :uint
  id_t = :uint
  attach_function 'getuid', [], uid_t
  attach_function 'getgid', [], gid_t
  attach_function 'geteuid', [], uid_t
  attach_function 'getegid', [], gid_t

  attach_function 'setgid', [gid_t], :int
  attach_function 'setuid', [uid_t], :int
  attach_function 'setegid', [gid_t], :int
  attach_function 'seteuid', [uid_t], :int

  attach_function 'setregid', [gid_t, gid_t], :int
  attach_function 'setreuid', [uid_t, uid_t], :int
  # These are linux specific
  #attach_function 'setresgid', [gid_t, gid_t, gid_t], :int
  #attach_function 'setresuid', [uid_t, uid_t, uid_t], :int

  attach_function 'getpriority', [:int, id_t], :int
  attach_function 'setpriority', [:int, id_t, :int], :int

  attach_function 'getgroups', [:int, :pointer], :int
  attach_function 'setgroups', [:int, :pointer], :int
  attach_function 'initgroups', [:string, gid_t], :int

  attach_function 'setrlimit', [:int, :pointer], :int
  attach_function 'getrlimit', [:int, :pointer], :int

  # password and group file access
  attach_function 'getpwnam', [:string], :pointer
  attach_function 'getpwuid', [:uint], :pointer
  attach_function 'setpwent', [], :void
  attach_function 'getpwent', [], :pointer
  attach_function 'endpwent', [], :void

  attach_function 'getgrnam', [:string], :pointer
  attach_function 'getgrgid', [:uint], :pointer
  attach_function 'setgrent', [], :void
  attach_function 'getgrent', [], :pointer
  attach_function 'endgrent', [], :void

  # processes and signals
  pid_t = :int
  attach_function 'kill', [pid_t, :int], :int
  attach_function 'getpgid', [pid_t], pid_t
  attach_function 'setpgid', [pid_t, pid_t], :int
  attach_function 'getpid', [], pid_t
  attach_function 'getppid', [], pid_t
  attach_function 'getpgrp', [], pid_t
  attach_function 'setsid', [], pid_t
end
