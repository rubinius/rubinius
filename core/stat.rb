module Rubinius
  class Stat
    include Comparable

    attr_reader :path

    def self.stat(path)
      stat = allocate
      if Rubinius.privately { stat.setup path } == 0
        stat
      else
        nil
      end
    end

    def self.fstat(fd)
      stat = allocate
      result = Rubinius.privately { stat.fsetup fd }
      Errno.handle "file descriptor #{fd}" unless result == 0
      stat
    end

    def self.lstat(path)
      stat = allocate
      result = Rubinius.privately { stat.lsetup path }
      Errno.handle path unless result == 0
      stat
    end

    def initialize(path)
      Errno.handle path unless setup(path) == 0
    end

    private :initialize

    def setup(path)
      Rubinius.primitive :stat_stat
      path = Rubinius::Type.coerce_to_path(path)
      setup(path)
    end
    private :setup

    def fsetup(fd)
      Rubinius.primitive :stat_fstat
      fd = Rubinius::Type.coerce_to fd, Integer, :to_int
      fsetup(fd)
    end
    private :fsetup

    def lsetup(path)
      Rubinius.primitive :stat_lstat
      path = Rubinius::Type.coerce_to_path(path)
      lsetup(path)
    end
    private :lsetup

    def dev
      Rubinius.primitive :stat_dev
      raise PrimitiveFailure, "Rubinius::Stat#dev primitive failed"
    end

    def ino
      Rubinius.primitive :stat_ino
      raise PrimitiveFailure, "Rubinius::Stat#ino primitive failed"
    end

    def mode
      Rubinius.primitive :stat_mode
      raise PrimitiveFailure, "Rubinius::Stat#mode primitive failed"
    end

    def nlink
      Rubinius.primitive :stat_nlink
      raise PrimitiveFailure, "Rubinius::Stat#nlink primitive failed"
    end

    def uid
      Rubinius.primitive :stat_uid
      raise PrimitiveFailure, "Rubinius::Stat#uid primitive failed"
    end

    def gid
      Rubinius.primitive :stat_gid
      raise PrimitiveFailure, "Rubinius::Stat#gid primitive failed"
    end

    def rdev
      Rubinius.primitive :stat_rdev
      raise PrimitiveFailure, "Rubinius::Stat#rdev primitive failed"
    end

    def size
      Rubinius.primitive :stat_size
      raise PrimitiveFailure, "Rubinius::Stat#size primitive failed"
    end

    def blksize
      Rubinius.primitive :stat_blksize
      raise PrimitiveFailure, "Rubinius::Stat#blksize primitive failed"
    end

    def blocks
      Rubinius.primitive :stat_blocks
      raise PrimitiveFailure, "Rubinius::Stat#blocks primitive failed"
    end

    def atime
      Rubinius.primitive :stat_atime
      raise PrimitiveFailure, "Rubinius::Stat#atime primitive failed"
    end

    def mtime
      Rubinius.primitive :stat_mtime
      raise PrimitiveFailure, "Rubinius::Stat#mtime primitive failed"
    end

    def ctime
      Rubinius.primitive :stat_ctime
      raise PrimitiveFailure, "Rubinius::Stat#ctime primitive failed"
    end

    def birthtime
      Rubinius.primitive :stat_birthtime
      raise NotImplementedError, "birthtime() function is unimplemented on this machine"
    end

    def inspect
      "#<#{self.class.name} dev=0x#{self.dev.to_s(16)}, ino=#{self.ino}, " \
      "mode=#{sprintf("%07d", self.mode.to_s(8).to_i)}, nlink=#{self.nlink}, " \
      "uid=#{self.uid}, gid=#{self.gid}, rdev=0x#{self.rdev.to_s(16)}, " \
      "size=#{self.size}, blksize=#{self.blksize}, blocks=#{self.blocks}, " \
      "atime=#{self.atime}, mtime=#{self.mtime}, ctime=#{self.ctime}>"
    end

    def blockdev?
      mode & S_IFMT == S_IFBLK
    end

    def chardev?
      mode & S_IFMT == S_IFCHR
    end

    def dev_major
      major = Rubinius::FFI::Platform::POSIX.major dev
      major < 0 ? nil : major
    end

    def dev_minor
      minor = Rubinius::FFI::Platform::POSIX.major dev
      minor < 0 ? nil : minor
    end

    def directory?
      mode & S_IFMT == S_IFDIR
    end

    def executable?
      return true if superuser?
      return mode & S_IXUSR != 0 if owned?
      return mode & S_IXGRP != 0 if grpowned?
      return mode & S_IXOTH != 0
    end

    def executable_real?
      return true if rsuperuser?
      return mode & S_IXUSR != 0 if rowned?
      return mode & S_IXGRP != 0 if rgrpowned?
      return mode & S_IXOTH != 0
    end

    def file?
      mode & S_IFMT == S_IFREG
    end

    def ftype
      if file?
        "file"
      elsif directory?
        "directory"
      elsif chardev?
        "characterSpecial"
      elsif blockdev?
        "blockSpecial"
      elsif pipe?
        "fifo"
      elsif socket?
        "socket"
      elsif symlink?
        "link"
      else
        "unknown"
      end
    end

    def grpowned?
      Process.groups.include?(gid)
    end

    def owned?
      uid == Rubinius::FFI::Platform::POSIX.geteuid
    end

    def pipe?
      mode & S_IFMT == S_IFIFO
    end

    def rdev_major
      major = Rubinius::FFI::Platform::POSIX.major rdev
      major < 0 ? nil : major
    end

    def rdev_minor
      minor = Rubinius::FFI::Platform::POSIX.minor rdev
      minor < 0 ? nil : minor
    end

    def readable?
      return true if superuser?
      return mode & S_IRUSR != 0 if owned?
      return mode & S_IRGRP != 0 if grpowned?
      return mode & S_IROTH != 0
    end

    def readable_real?
      return true if rsuperuser?
      return mode & S_IRUSR != 0 if rowned?
      return mode & S_IRGRP != 0 if rgrpowned?
      return mode & S_IROTH != 0
    end

    def setgid?
      mode & S_ISGID != 0
    end

    def setuid?
      mode & S_ISUID != 0
    end

    def sticky?
      mode & S_ISVTX != 0
    end

    def size?
      size == 0 ? nil : size
    end

    def socket?
      mode & S_IFMT == S_IFSOCK
    end

    def symlink?
      mode & S_IFMT == S_IFLNK
    end

    def writable?
      return true if superuser?
      return mode & S_IWUSR != 0 if owned?
      return mode & S_IWGRP != 0 if grpowned?
      return mode & S_IWOTH != 0
    end

    def writable_real?
      return true if rsuperuser?
      return mode & S_IWUSR != 0 if rowned?
      return mode & S_IWGRP != 0 if rgrpowned?
      return mode & S_IWOTH != 0
    end

    def zero?
      size == 0
    end

    def <=>(other)
      return nil unless other.is_a?(File::Stat)
      self.mtime <=> other.mtime
    end

    def rgrpowned?
      gid == Rubinius::FFI::Platform::POSIX.getgid
    end
    private :rgrpowned?

    def rowned?
      uid == Rubinius::FFI::Platform::POSIX.getuid
    end
    private :rowned?

    def rsuperuser?
      Rubinius::FFI::Platform::POSIX.getuid == 0
    end
    private :rsuperuser?

    def superuser?
      Rubinius::FFI::Platform::POSIX.geteuid == 0
    end
    private :superuser?
  end
end
