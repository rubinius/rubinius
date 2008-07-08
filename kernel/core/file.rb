# depends on: io.rb class.rb module.rb enumerable.rb

module Platform::POSIX
  class TimeVal < FFI::Struct
    config 'rbx.platform.timeval', :tv_sec, :tv_usec
  end
end

class File < IO

  include Enumerable

  #--
  # Internal class for accessing timevals
  #++

  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end

  module Constants
    include IO::Constants

    # TODO: "OK" constants aren't in File::Constants in MRI
    F_OK = 0 # test for existence of file
    X_OK = 1 # test for execute or search permission
    W_OK = 2 # test for write permission
    R_OK = 4 # test for read permission

    FNM_NOESCAPE = 0x01
    FNM_PATHNAME = 0x02
    FNM_DOTMATCH = 0x04
    FNM_CASEFOLD = 0x08
  end
  include Constants

  SEPARATOR = Platform::File::SEPARATOR
  Separator = Platform::File::SEPARATOR
  ALT_SEPARATOR = Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = Platform::File::PATH_SEPARATOR
  POSIX = Platform::POSIX

  def initialize(path_or_fd, mode = "r", perm = 0666)
    if path_or_fd.kind_of?(Integer)
      super(path_or_fd, mode)
      @path = nil
      return self
    end

    path = StringValue(path_or_fd)

    fd = IO.sysopen(path, mode, perm)
    Errno.handle path if fd < 0

    @path = path
    setup fd
  end

  attr_reader :path

  def self.after_loaded
    private_class_method :dirsep?, :next_path, :range, :name_match

    # these will be necessary when we run on Windows
    const_set :DOSISH, RUBY_PLATFORM.match("mswin")
    const_set :CASEFOLD_FILESYSTEM, DOSISH
    const_set :FNM_SYSCASE, CASEFOLD_FILESYSTEM ? FNM_CASEFOLD : false
  end

  def self.atime(path)
    Stat.new(path).atime
  end

  def self.basename(path,ext = "")
    path = StringValue(path)
    ext = StringValue(ext)
    Platform::File.basename(path,ext)
  end

  def self.blockdev?(path)
    st = Stat.stat? path
    st ? st.blockdev? : false
  end

  def self.chardev?(path)
    st = Stat.stat? path
    st ? st.chardev? : false
  end

  def self.chmod(mode, *paths)
    mode = Type.coerce_to(mode, Integer, :to_int) unless mode.is_a? Integer
    paths.each do |path|
      path = Type.coerce_to(path, String, :to_str) unless path.is_a? String
      POSIX.chmod(path, mode)
    end
    paths.size
  end

  def self.lchmod(mode, *paths)
    mode = Type.coerce_to(mode, Integer, :to_int) unless mode.is_a? Integer
    paths.each do |path|
      path = Type.coerce_to(path, String, :to_str) unless path.is_a? String
      POSIX.lchmod(path, mode)
    end
    paths.size
  end

  def self.chown(owner_int, group_int, *paths)
    owner_int = -1 if owner_int == nil
    group_int = -1 if group_int == nil
    paths.each { |path| POSIX.chown(path, owner_int, group_int) }
    paths.size
  end

  def self.lchown(owner_int, group_int, *paths)
    owner_int = -1 if owner_int == nil
    group_int = -1 if group_int == nil
    paths.each { |path| POSIX.lchown(path, owner_int, group_int) }
    paths.size
  end

  def self.ctime(path)
    Stat.new(path).ctime
  end

  def self.directory?(path)
    st = Stat.stat? path
    st ? st.directory? : false
  end

  def self.dirname(path)
    path = StringValue(path)
    Platform::File.dirname(path)
  end

  def self.executable?(path)
    st = Stat.stat? path
    st ? st.executable? : false
  end

  def self.executable_real?(path)
    st = Stat.stat? path
    st ? st.executable_real? : false
  end

  def self.exist?(path)
    Stat.stat?(path) ? true : false
  rescue SystemCallError
    false
  end

  def self.expand_path(path, dir_string = nil)
    Platform::File.expand_path(path, dir_string)
  end

  def self.extname(path)
    filename = File.basename(StringValue(path))
    idx = filename.rindex '.'
    have_dot = idx != nil
    first_char = idx == 0
    last_char = idx == filename.length - 1
    only_dots = filename.match(/[^\.]/).nil?

    return '' unless have_dot
    return '' if first_char || last_char
    return '' if only_dots
    filename.slice idx..-1
  end

  def self.file?(path)
    st = Stat.stat? path
    st ? st.file? : false
  end

  #--
  # File.fnmatch and helpers. This is a port of JRuby's code
  #++

  def self.dirsep?(char)
    if DOSISH then
      char == ?\\ || char == ?/
    else
      char == ?/
    end
  end

  def self.next_path(str, start, strend)
    start += 1 while start < strend and !dirsep? str[start]
    start
  end

  def self.range(pattern, pstart, pend, test, flags)
    ok = false
    escape = (flags & FNM_NOESCAPE) == 0
    case_sensitive = (flags & FNM_CASEFOLD) == 0
    neg = pattern[pstart] == ?! || pattern[pstart] == ?^

    pstart += 1 if neg

    while pattern[pstart] != ?] do
      pstart += 1 if escape && pattern[pstart] == ?\\
      return -1 if pstart >= pend
      cstart = cend = pattern[pstart]
      pstart += 1

      if pattern[pstart] == ?- && pattern[pstart+1] != ?]
        pstart += 1
        pstart += 1 if escape && pattern[pstart] == ?\\
        return -1 if pstart >= pend
        cend = pattern[pstart]
        pstart += 1
      end

      if case_sensitive
        ok = true if cstart <= test && test <= cend
      else
        ok = true if cstart.tolower <= test.tolower &&
          test.tolower <= cend.tolower
      end
    end

    ok == neg ? -1 : pstart + 1
  end

  def self.name_match(pattern, str, flags, patstart, patend, strstart, strend)
    index = strstart
    pstart = patstart
    escape   = (flags & FNM_NOESCAPE) == 0
    pathname = (flags & FNM_PATHNAME) != 0
    period   = (flags & FNM_DOTMATCH) == 0
    nocase   = (flags & FNM_CASEFOLD) != 0

    while pstart < patend do
      char = pattern[pstart]
      pstart += 1
      case char
      when ??
        if index >= strend || (pathname && dirsep?(str[index])) ||
           (period && str[index] == ?. &&
            (index == 0 || (pathname && dirsep?(str[index-1]))))
          return false
        end

        index += 1

      when ?*
        while pstart < patend
          char = pattern[pstart]
          pstart += 1
          break unless char == ?*
        end

        if index < strend &&
           (period && str[index] == ?. &&
            (index == 0 || (pathname && dirsep?(str[index-1]))))
          return false
        end

        if pstart > patend || (pstart == patend && char == ?*)
          return !(pathname && next_path(str, index, strend) < strend)
        elsif pathname && dirsep?(char)
          index = next_path(str, index, strend)
          return false unless index < strend
          index += 1
        else
          test = if escape && char == ?\\ && pstart < patend then
                   pattern[pstart]
                 else
                   char
                 end.tolower

          pstart -= 1

          while index < strend do
            if (char == ?? || char == ?[ || str[index].tolower == test) &&
               name_match(pattern, str, flags | FNM_DOTMATCH, pstart, patend,
                          index, strend)
              return true
            elsif pathname && dirsep?(str[index])
              break
            end

            index += 1
          end

          return false
        end

      when ?[
        if index >= strend ||
           (pathname && dirsep?(str[index]) ||
            (period && str[index] == ?. &&
             (index == 0 ||
              (pathname && dirsep?(str[index-1])))))
          return false
        end

        pstart = range pattern, pstart, patend, str[index], flags

        return false if pstart == -1

        index += 1
      else
        if char == ?\\
          if escape &&
             (!DOSISH || (pstart < patend && "*?[]\\".index(pattern[pstart])))
            char = pstart >= patend ? ?\\ : pattern[pstart]
            pstart += 1
          end
        end

        return false if index >= strend

        if DOSISH && (pathname && isdirsep?(char) && dirsep?(str[index]))
          # TODO: invert this boolean expression
        else
          if nocase
            return false if char.tolower != str[index].tolower
          else
            return false if char != str[index]
          end
        end

        index += 1
      end
    end

    index >= strend ? true : false
  end

  def self.fnmatch(pattern, path, flags=0)
    pattern = StringValue(pattern).dup
    path    = StringValue(path).dup
    flags   = Type.coerce_to(flags, Fixnum, :to_int) unless Fixnum === flags

    name_match(pattern, path, flags, 0, pattern.size, 0, path.size)
  end

  def self.ftype(path)
    lstat(path).ftype
  end

  def self.grpowned?(path)
    begin
      lstat(path).grpowned?
    rescue
      false
    end
  end

  def self.identical?(orig, copy)
    st_o = stat(StringValue(orig))
    st_c = stat(StringValue(copy))

    return false unless st_o.ino == st_c.ino
    return false unless st_o.ftype == st_c.ftype
    return false unless POSIX.access(orig, Constants::R_OK)
    return false unless POSIX.access(copy, Constants::R_OK)

    true
  end

  def self.join(*args)
    args.map! { |o|
      o = o.to_str unless Array === o || String === o
      o
    } rescue raise TypeError

    # let join/split deal with all the recursive array complexities
    # one small hack is to replace URI header with \0 and swap back later
    result = args.join(SEPARATOR).gsub(/\:\//, "\0").split(/#{SEPARATOR}+/o)
    result << '' if args.empty? || args.last.empty? || args.last[-1] == SEPARATOR[0]
    result.join(SEPARATOR).gsub(/\0/, ':/')
  end

  def self.link(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.link(from, to)
    Errno.handle if n == -1
    n
  end

  def self.lstat(path)
    Stat.new path, false
  end

  def self.mtime(path)
    Stat.new(path).mtime
  end

  def self.pipe?(path)
    st = Stat.stat? path
    st ? st.pipe? : false
  end

  def self.readable?(path)
    st = Stat.stat? path
    st ? st.readable? : false
  end

  def self.readable_real?(path)
    st = Stat.stat? path
    st ? st.readable_real? : false
  end

  def self.rename(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.rename(from, to)
    Errno.handle if n == -1
    n
  end

  def self.readlink(path)
    StringValue(path)

    buf = " " * 1024

    n = POSIX.readlink(path, buf, buf.length)
    Errno.handle if n == -1

    buf[0, n]
  end

  def self.size(path)
    stat(path).size
  end

  def self.size?(path)
    st = Stat.stat? path
    st && st.size > 0 ? st.size : nil
  end

  def self.socket?(path)
    st = Stat.stat? path
    st ? st.socket? : false
  end

  def self.split(path)
    p = StringValue(path)
    [dirname(p), basename(p)]
  end

  def self.stat(path)
    Stat.new path
  end

  def self.symlink(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.symlink(from, to)
    Errno.handle if n == -1
    n
  end

  def self.symlink?(path)
    st = Stat.stat? path, false
    st ? st.symlink? : false
  end

  def self.to_sexp(name, newlines=true)
    out = to_sexp_full(name, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.import_position out.at(1), out.at(2), out.at(3)
      exc.file = name
      raise exc
    end

    out = [:newline, 0, name, [:nil]] unless out
    out
  end

  def self.truncate(path, length)
    unless self.exist?(path)
      raise Errno::ENOENT, path
    end

    unless length.respond_to?(:to_int)
      raise TypeError, "can't convert #{length.class} into Integer"
    end

    n = POSIX.truncate(path, length)
    Errno.handle if n == -1
    n
  end

  def self.umask(mask = nil)
    if mask
      POSIX.umask(mask)
    else
      old_mask = POSIX.umask(0)
      POSIX.umask(old_mask)
      old_mask
    end
  end

  def self.unlink(*paths)
    paths.each do |path|
      path = StringValue(path)

      n = POSIX.unlink(path)
      Errno.handle if n == -1
    end

    paths.size
  end

  def self.utime(a_in, m_in, *paths)
    ptr = MemoryPointer.new(POSIX::TimeVal, 2)
    atime = POSIX::TimeVal.new ptr
    mtime = POSIX::TimeVal.new ptr[1]
    atime[:tv_sec] = a_in.to_i
    atime[:tv_usec] = 0

    mtime[:tv_sec] = m_in.to_i
    mtime[:tv_usec] = 0

    paths.each do |path|
      if POSIX.utimes(path, ptr) != 0
        Errno.handle
      end
    end

    ptr.free
  end

  def self.writable?(path)
    st = Stat.stat? path
    st ? st.writable? : false
  end

  def self.writable_real?(path)
    st = Stat.stat? path
    st ? st.writable_real? : false
  end

  def self.zero?(path)
    st = Stat.stat? path
    st ? st.zero? : false
  end
  
  # 
  # Returns true if the named file exists and the effective
  # used id of the calling process is the owner of the file.
  #  File.owned?(file_name)   => true or false
  def self.owned?(file_name)
    Stat.new(file_name).owned?
  end
  
  # 
  # Returns true if the named file has the setgid bit set.
  def self.setgid?(file_name)
    Stat.new(file_name).setgid?
  end
  
  # 
  # Returns true if the named file has the setuid bit set.
  def self.setuid?(file_name)
    Stat.new(file_name).setuid?
  end
  
  # 
  # Returns true if the named file has the sticky bit set.
  def self.sticky?(file_name)
    Stat.new(file_name).sticky?
  end

  class << self
    alias_method :delete,   :unlink
    alias_method :exists?,  :exist?
    alias_method :fnmatch?, :fnmatch
  end

  def atime
    Stat.new(@path).atime
  end

  def chmod(mode)
    mode = Type.coerce_to(mode, Integer, :to_int) unless mode.is_a? Integer
    POSIX.fchmod(@descriptor, mode)
  end

  def chown(owner_int, group_int)
    POSIX.fchown(@descriptor, owner_int || -1, group_int || -1)
  end

  def ctime
    Stat.new(@path).ctime
  end

  def flock(locking_constant)
    result = POSIX.flock(@descriptor, locking_constant)
    return false if result == -1
    result
  end

  def lstat
    Stat.new @path, false
  end

  def mtime
    Stat.new(@path).mtime
  end

  def stat
    Stat.new @path
  end

  def truncate(length)
    length = Type.coerce_to(length, Integer, :to_int)

    raise Errno::EINVAL, "Can't truncate a file to a negative length" if length < 0
    raise IOError, "File is closed" if closed?

    n = POSIX.ftruncate(@descriptor, length)
    Errno.handle if n == -1
    n
  end

  def inspect
    return_string = "#<#{self.class}:0x#{object_id.to_s(16)} path=#{@path}"
    return_string << " (closed)" if closed?
    return_string << ">"
  end
end     # File

class File::Stat
  class Struct < FFI::Struct
    config "rbx.platform.stat", :st_dev, :st_ino, :st_mode, :st_nlink,
           :st_uid, :st_gid, :st_rdev, :st_size, :st_blksize, :st_blocks,
           :st_atime, :st_mtime, :st_ctime
  end

  include Comparable

  S_IRUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IRUSR']
  S_IWUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWUSR']
  S_IXUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXUSR']
  S_IRGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IRGRP']
  S_IWGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWGRP']
  S_IXGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXGRP']
  S_IROTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IROTH']
  S_IWOTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWOTH']
  S_IXOTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXOTH']

  S_IFMT   = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFMT']
  S_IFIFO  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFIFO']
  S_IFCHR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFCHR']
  S_IFDIR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFDIR']
  S_IFBLK  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFBLK']
  S_IFREG  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFREG']
  S_IFLNK  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFLNK']
  S_IFSOCK = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFSOCK']
  S_IFWHT  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IFWHT']
  S_ISUID  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_ISUID']
  S_ISGID  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_ISGID']

  POSIX    = Platform::POSIX

  ##
  # File::Stat#from_fd is used to support IO#stat which does not necessarily
  # have a path.

  def self.from_fd(descriptor)
    stat = allocate
    stat_struct = Struct.new

    stat.instance_variable_set :@stat, stat_struct
    stat.instance_variable_set :@path, nil

    result = POSIX.fstat descriptor, stat_struct.pointer

    Errno.handle "file descriptor #{descriptor}" unless result == 0

    stat
  end

  def initialize(path, follow_links=true)
    @stat = Struct.new
    @path = StringValue path

    result = if follow_links
               POSIX.stat @path, @stat.pointer
             else
               POSIX.lstat @path, @stat.pointer
             end

    Errno.handle @path unless result == 0
  end

  def self.stat?(path, follow_links=true)
    new path, follow_links
  rescue Errno::ENOENT, Errno::ENOTDIR
    nil
  end

  def atime
    Time.at @stat[:st_atime]
  end

  def blksize
    @stat[:st_blksize]
  end

  def blocks
    @stat[:st_blocks]
  end

  def blockdev?
    @stat[:st_mode] & S_IFMT == S_IFBLK
  end

  def chardev?
    @stat[:st_mode] & S_IFMT == S_IFCHR
  end

  def ctime
    Time.at @stat[:st_ctime]
  end

  def dev
    @stat[:st_dev]
  end

  def dev_major
    major = POSIX.major @stat[:st_dev]
    major < 0 ? nil : major
  end

  def dev_minor
    minor = POSIX.major @stat[:st_dev]
    minor < 0 ? nil : minor
  end

  def directory?
    @stat[:st_mode] & S_IFMT == S_IFDIR
  end

  def executable?
    return true if superuser?
    return @stat[:st_mode] & S_IXUSR != 0 if owned?
    return @stat[:st_mode] & S_IXGRP != 0 if grpowned?
    return @stat[:st_mode] & S_IXOTH != 0
  end

  def executable_real?
    return true if rsuperuser?
    return @stat[:st_mode] & S_IXUSR != 0 if rowned?
    return @stat[:st_mode] & S_IXGRP != 0 if rgrpowned?
    return @stat[:st_mode] & S_IXOTH != 0
  end

  def file?
    @stat[:st_mode] & S_IFMT == S_IFREG
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

  def gid
    @stat[:st_gid]
  end

  def grpowned?
    @stat[:st_gid] == POSIX.getegid
  end

  def ino
    @stat[:st_ino]
  end

  def inspect
    "#<File::Stat dev=0x#{self.dev.to_s(16)}, ino=#{self.ino}, " \
    "mode=#{sprintf("%07d", self.mode.to_s(8).to_i)}, nlink=#{self.nlink}, " \
    "uid=#{self.uid}, gid=#{self.gid}, rdev=0x#{self.rdev.to_s(16)}, " \
    "size=#{self.size}, blksize=#{self.blksize}, blocks=#{self.blocks}, " \
    "atime=#{self.atime}, mtime=#{self.mtime}, ctime=#{self.ctime}>"
  end

  def nlink
    @stat[:st_nlink]
  end

  def mtime
    Time.at @stat[:st_mtime]
  end

  def mode
    @stat[:st_mode]
  end

  def owned?
    @stat[:st_uid] == POSIX.geteuid
  end

  def path
    @path
  end

  def pipe?
    @stat[:st_mode] & S_IFMT == S_IFIFO
  end

  def rdev
    @stat[:st_rdev]
  end

  def rdev_major
    major = POSIX.major @stat[:st_rdev]
    major < 0 ? nil : major
  end

  def rdev_minor
    minor = POSIX.minor @stat[:st_rdev]
    minor < 0 ? nil : minor
  end

  def readable?
    return true if superuser?
    return @stat[:st_mode] & S_IRUSR != 0 if owned?
    return @stat[:st_mode] & S_IRGRP != 0 if grpowned?
    return @stat[:st_mode] & S_IROTH != 0
  end

  def readable_real?
    return true if rsuperuser?
    return @stat[:st_mode] & S_IRUSR != 0 if rowned?
    return @stat[:st_mode] & S_IRGRP != 0 if rgrpowned?
    return @stat[:st_mode] & S_IROTH != 0
  end
  
  def setgid?
    !!@stat[:st_gid]
  end
  
  def setuid?
    !!@stat[:st_uid]
  end

  def size
    @stat[:st_size]
  end

  def size?
    size == 0 ? nil : size
  end

  def socket?
    @stat[:st_mode] & S_IFMT == S_IFSOCK
  end

  def symlink?
    @stat[:st_mode] & S_IFMT == S_IFLNK
  end

  def uid
    @stat[:st_uid]
  end

  def writable?
    return true if superuser?
    return @stat[:st_mode] & S_IWUSR != 0 if owned?
    return @stat[:st_mode] & S_IWGRP != 0 if grpowned?
    return @stat[:st_mode] & S_IWOTH != 0
  end

  def writable_real?
    return true if rsuperuser?
    return @stat[:st_mode] & S_IWUSR != 0 if rowned?
    return @stat[:st_mode] & S_IWGRP != 0 if rgrpowned?
    return @stat[:st_mode] & S_IWOTH != 0
  end

  def zero?
    @stat[:st_size] == 0
  end

  def <=> (other)
    return nil unless other.is_a?(File::Stat)
    self.mtime <=> other.mtime
  end

  def rgrpowned?
    @stat[:st_gid] == POSIX.getgid
  end
  private :rgrpowned?

  def rowned?
    @stat[:st_uid] == POSIX.getuid
  end
  private :rowned?

  def rsuperuser?
    POSIX.getuid == 0
  end
  private :rsuperuser?

  def superuser?
    POSIX.geteuid == 0
  end
  private :superuser?
end     # File::Stat
