##
# Namespace for various POSIX functions.

module FFI::Platform::POSIX
  extend FFI::Library

  # errors
  attach_function :errno,    'ffi_errno',     [],     :int
  attach_function :errno=,   'ffi_set_errno', [:int], :void
  attach_function :strerror, [:int], :string
  attach_function :perror,   [:string], :void

  # memory
  attach_function :malloc,   [:int], :pointer
  attach_function :realloc,  [:pointer, :int], :pointer
  attach_function :free,     [:pointer], :void
  attach_function :memset,   [:pointer, :int, :int], :pointer
  attach_function :memcpy,   [:pointer, :pointer, :int], :pointer

  # rand
  attach_function :srand,    [:uint], :void
  attach_function :rand,     [], :uint

  # file system
  attach_function :access,   [:string, :int], :int
  attach_function :chmod,    [:string, :mode_t], :int
  attach_function :fchmod,   [:int, :mode_t], :int
  #  Removed, fails on OS X Tiger, OpenBSD and we are not using it.
#  attach_function 'lchmod',  [:string, :mode_t], :int
  attach_function :chown,    [:string, :uid_t, :gid_t], :int
  attach_function :fchown,   [:int, :uid_t, :gid_t], :int
  attach_function :lchown,   [:string, :uid_t, :gid_t], :int
  attach_function :unlink,   [:string], :int
  attach_function :getcwd,   [:string, :size_t], :string
  attach_function :umask,    [:mode_t], :int
  attach_function :link,     [:string, :string], :int
  attach_function :symlink,  [:string, :string], :int
  attach_function :readlink, [:string, :string, :int], :int
  attach_function :rename,   [:string, :string], :int
  attach_function :utimes,   [:string, :pointer], :int

  # directories
  attach_function :chdir,     [:string], :int
  attach_function :opendir,   [:string], :pointer
  attach_function :readdir,   [:pointer], :pointer
  attach_function :rewinddir, [:pointer], :void
  attach_function :closedir,  [:pointer], :int
  attach_function :mkdir,     [:string, :short], :int
  attach_function :rmdir,     [:string], :int
  attach_function :chroot,    [:string], :int

  attach_function :fnmatch,   [:string, :string, :int], :int

  # File/IO
  attach_function :fcntl,    [:int, :int, :int], :int

  #   opening/closing
  attach_function :open,     [:string, :int, :int], :int
  attach_function :fdopen,   [:int, :string], :pointer
  attach_function :fopen,    [:string, :string], :pointer
  attach_function :fclose,   [:pointer], :int
  attach_function :fsync,    [:int], :int

  attach_function :dup,      [:int], :int

  #   buffering
  attach_function :fflush,   [:pointer], :int

  #   inspecting
  attach_function :fileno,   [:pointer], :int
  attach_function :feof,     [:pointer], :int
  attach_function :ferror,   [:pointer], :int
  attach_function :clearerr, [:pointer], :void
  attach_function :fseek,    [:pointer, :int, :int], :int
  attach_function :ftell,    [:pointer], :int
  attach_function :lseek,    [:int, :int, :int], :int
  attach_function :isatty,   [:int], :int

  #   reading
  attach_function :fread,    [:string, :size_t, :size_t, :pointer], :size_t
  attach_function :fgets,    [:string, :int, :pointer], :void
  attach_function :fgetc,    [:pointer], :int

  #   writing
  attach_function :fwrite,   [:string, :size_t, :size_t, :pointer], :size_t
  attach_function :ungetc,   [:int, :pointer], :int

  #   truncating
  attach_function :truncate,  [:string, :off_t], :int
  attach_function :ftruncate, [:int, :off_t], :int

  #   locking
  attach_function :flock, [:int, :int], :int

  # Time
  attach_function :time,         [:pointer], :time_t
  attach_function :timezone,     'ffi_timezone', [], :time_t
  attach_function :tzname,       'ffi_tzname', [:int], :string
  attach_function :gettimeofday, [:pointer, :pointer], :int

  # UID/GID
  attach_function :getuid,  [], :uid_t
  attach_function :getgid,  [], :gid_t
  attach_function :geteuid, [], :uid_t
  attach_function :getegid, [], :gid_t

  attach_function :setgid,  [:gid_t], :int
  attach_function :setuid,  [:uid_t], :int
  attach_function :setegid, [:gid_t], :int
  attach_function :seteuid, [:uid_t], :int

  attach_function :setregid, [:gid_t, :gid_t], :int
  attach_function :setreuid, [:uid_t, :uid_t], :int

  # These are linux specific
  #attach_function 'setresgid', [gid_t, gid_t, gid_t], :int
  #attach_function 'setresuid', [uid_t, uid_t, uid_t], :int

  attach_function :getpriority, [:int, :id_t], :int
  attach_function :setpriority, [:int, :id_t, :int], :int

  attach_function :getgroups,  [:int, :pointer], :int
  attach_function :setgroups,  [:int, :pointer], :int
  attach_function :initgroups, [:string, :gid_t], :int

  attach_function :setrlimit, [:int, :pointer], :int
  attach_function :getrlimit, [:int, :pointer], :int

  # password and group file access
  attach_function :getpwnam, [:string], :pointer
  attach_function :getpwuid, [:uint], :pointer
  attach_function :setpwent, [], :void
  attach_function :getpwent, [], :pointer
  attach_function :endpwent, [], :void

  attach_function :getgrnam, [:string], :pointer
  attach_function :getgrgid, [:gid_t], :pointer
  attach_function :setgrent, [], :void
  attach_function :getgrent, [], :pointer
  attach_function :endgrent, [], :void

  # processes and signals
  attach_function :kill,    [:pid_t, :int], :int
  attach_function :getpgid, [:pid_t], :pid_t
  attach_function :setpgid, [:pid_t, :pid_t], :int
  attach_function :getpid,  [], :pid_t
  attach_function :getppid, [], :pid_t
  attach_function :getpgrp, [], :pid_t
  attach_function :setsid,  [], :pid_t

  # related to stat()
  attach_function :major, 'ffi_major', [:dev_t], :dev_t
  attach_function :minor, 'ffi_minor', [:dev_t], :dev_t

  # stat
  # FIXME: these are TEMPORARY until we determine how to
  # have FFI resolve symbols that may be macros. This is
  # used rather than a primitive so that it is easier to
  # replace (unlike primitives).
  attach_function :stat,  'ffi_stat',  [:string, :pointer], :int
  attach_function :fstat, 'ffi_fstat', [:int,    :pointer], :int
  attach_function :lstat, 'ffi_lstat', [:string, :pointer], :int
end
