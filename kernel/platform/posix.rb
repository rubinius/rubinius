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

  attach_function 'getuid', [], :uint
  attach_function 'getgid', [], :uint

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
  attach_function 'kill', [:int, :int], :int
  attach_function 'getpgid', [:int], :int
  attach_function 'setpgid', [:int, :int], :int
  attach_function 'getpid', [], :int
  attach_function 'getppid', [], :int
end
