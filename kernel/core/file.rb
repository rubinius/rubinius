# depends on: io.rb class.rb module.rb

module Platform::POSIX
  class TimeVal < FFI::Struct
    config 'rbx.platform.timeval', :tv_sec, :tv_usec
  end
end 

class File < IO
  # Internal class for accessing timevals
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

    FNM_NOESCAPE = 0x01;
    FNM_PATHNAME = 0x02;
    FNM_DOTMATCH = 0x04;
    FNM_CASEFOLD = 0x08;
  end
  include Constants

  SEPARATOR = Platform::File::SEPARATOR
  ALT_SEPARATOR = Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = Platform::File::PATH_SEPARATOR
  POSIX = Platform::POSIX


  # Creation

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


  # Class methods

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
    paths.each { |path| POSIX.chmod(path, mode) }
    paths.size
  end
  
  def self.chown(owner_int, group_int, *paths)
    owner_int = -1 if owner_int == nil
    group_int = -1 if group_int == nil
    paths.each { |path| POSIX.chown(path, owner_int, group_int) }
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

  def self.fnmatch(pattern, path, flags=0)
    pattern = StringValue(pattern).dup
    path = StringValue(path).dup
    escape = (flags & FNM_NOESCAPE) == 0
    pathname = (flags & FNM_PATHNAME) != 0
    nocase = (flags & FNM_CASEFOLD) != 0
    period = (flags & FNM_DOTMATCH) == 0
    subs = { /\{/ => '\{',
             /\}/ => '\}',
             /(^|([^\\]))\*{1,2}/ => '\1(.*)',
             /(^|([^\\]))\?/ => '\1(.)',
             /\\\?/ => '\\\\\?',
             /\\\*/ => '\\\\\*',
             # HACK /\+/   => '\\\+',
             /(\+)/ => '\\\\\\\\\+', # HACK String#sub is broken
           }

    return false if path[0] == ?. and pattern[0] != ?. and period
    pattern.gsub!('.', '\.')
    pattern = pattern.split(/(?<b>\[(?:\\[\[\]]|[^\[\]]|\g<b>)*\])/).collect do |part|
      if part[0] == ?[
        part.gsub!(/\\([*?])/, '\1')
        part.gsub(/\[!/, '[^')
      else
        subs.each { |p,s| part.gsub!(p, s) }
        # Because this pattern anchors itself, we have to rerun by hand
        subd = part.gsub!(/(^|([^\\]))\?/, '\1(.)')
        subd = part.gsub!(/(^|([^\\]))\?/, '\1(.)') while subd
        if escape
          part.gsub(/\\([^.\[])/, '\1')
        else
          part.gsub(/(\\)([^*?\[\]])/, '\1\1\2')
        end
      end
    end.join

    re = Regexp.new("^#{pattern}$", nocase ? Regexp::IGNORECASE : 0)

    m = re.match path
    if m
      return false unless m[0].size == path.size
      if pathname
        return false if m.captures.any? { |c| c.include?('/') }

        a = StringValue(pattern).dup.split '/'
        b = path.split '/'
        return false unless a.size == b.size
        return false unless a.zip(b).all? { |ary| ary[0][0] == ary[1][0] }
      end
      return true
    else
      return false
    end
  end

  def self.ftype(path)
    lstat(path).ftype
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

  # FIXME: will fail a bunch if platfrom == :mswin
  def self.join(*parts)
    path = ''

    parts.flatten.each_with_index do |part, i|
      part = StringValue part

      if i > 0 then
        if part[0] == ?/ then
          path.sub! %r|(.*?)/+\z|m, '\1'
        elsif path !~ %r|/+\z| then
          path << SEPARATOR
        end
      end

      path << part
    end

    path
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
    st ? st.size : nil
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

  class << self
    alias_method :delete,   :unlink
    alias_method :exists?,  :exist?
    alias_method :fnmatch?, :fnmatch
  end

  # Instance methods

  def atime
    Stat.new(@path).atime
  end

  def chmod(mode)
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
end     # File

class File::Stat
  class Struct < FFI::Struct
    config "rbx.platform.stat", :st_dev, :st_ino, :st_mode, :st_nlink,
           :st_uid, :st_gid, :st_rdev, :st_size, :st_blksize, :st_blocks,
           :st_atime, :st_mtime, :st_ctime
  end

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
  
  def initialize(path, follow_links=true)
    @path = StringValue path
    @stat = Struct.new
    if follow_links
      result = POSIX.stat @path, @stat.pointer
    else
      result = POSIX.lstat @path, @stat.pointer
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
    "#<#{self.class}:0x#{object_id.to_s(16)} path=#{@path} kind=#{ftype}>"
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

  # Internal methods

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
