##
# Namespace for various POSIX functions.

module Rubinius
module FFI::Platform::POSIX
  extend FFI::Library

  # errors
  attach_function :errno,    'ffi_errno',     [],     :int
  attach_function :errno=,   'ffi_set_errno', [:int], :void

  # memory
  attach_function :malloc,   [:size_t], :pointer
  attach_function :realloc,  [:pointer, :size_t], :pointer
  attach_function :free,     [:pointer], :void
  attach_function :memset,   [:pointer, :int, :size_t], :pointer
  attach_function :memcpy,   [:pointer, :pointer, :size_t], :pointer

  # file system
  attach_function :access,   [:string, :int], :int
  attach_function :chmod,    [:string, :mode_t], :int
  attach_function :fchmod,   [:int, :mode_t], :int
  attach_function :lchmod,   [:string, :mode_t], :int
  attach_function :chown,    [:string, :uid_t, :gid_t], :int
  attach_function :fchown,   [:int, :uid_t, :gid_t], :int
  attach_function :lchown,   [:string, :uid_t, :gid_t], :int
  attach_function :unlink,   [:string], :int
  attach_function :getcwd,   [:string, :size_t], :string
  attach_function :umask,    [:mode_t], :int
  attach_function :link,     [:string, :string], :int
  attach_function :symlink,  [:string, :string], :int
  attach_function :readlink, [:string, :pointer, :size_t], :ssize_t
  attach_function :rename,   [:string, :string], :int
  attach_function :utimes,   [:string, :pointer], :int

  # directories
  attach_function :chdir,     [:string], :int
  attach_function :mkdir,     [:string, :mode_t], :int
  attach_function :rmdir,     [:string], :int
  attach_function :chroot,    [:string], :int

  # File/IO
  attach_function :fcntl,    [:int, :int, :long], :int
  attach_function :ioctl,    [:int, :ulong, :long], :int
  attach_function :fsync,    [:int], :int
  attach_function :dup,      [:int], :int

  #   inspecting
  attach_function :isatty,   [:int], :int

  #   locking
  attach_function :flock, [:int, :int], :int

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

  attach_function :setresgid, [:gid_t, :gid_t, :gid_t], :int
  attach_function :setresuid, [:uid_t, :uid_t, :uid_t], :int

  attach_function :getpriority, [:int, :id_t], :int
  attach_function :setpriority, [:int, :id_t, :int], :int

  attach_function :getgroups,  [:int, :pointer], :int
  attach_function :setgroups,  [:int, :pointer], :int
  attach_function :initgroups, [:string, :gid_t], :int

  attach_function :setrlimit, [:int, :pointer], :int
  attach_function :getrlimit, [:int, :pointer], :int

  # password and group file access
  attach_function :getpwnam, [:string], :pointer
  attach_function :getpwuid, [:uid_t], :pointer
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
end
end
