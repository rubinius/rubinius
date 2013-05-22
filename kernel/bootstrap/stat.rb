module Rubinius
  class Stat

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

    def inspect
      "#<#{self.class.name} dev=0x#{self.dev.to_s(16)}, ino=#{self.ino}, " \
      "mode=#{sprintf("%07d", self.mode.to_s(8).to_i)}, nlink=#{self.nlink}, " \
      "uid=#{self.uid}, gid=#{self.gid}, rdev=0x#{self.rdev.to_s(16)}, " \
      "size=#{self.size}, blksize=#{self.blksize}, blocks=#{self.blocks}, " \
      "atime=#{self.atime}, mtime=#{self.mtime}, ctime=#{self.ctime}>"
    end
  end
end
