# depends on: io.rb class.rb module.rb

class File < IO
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

  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end

  SEPARATOR = Platform::File::SEPARATOR
  ALT_SEPARATOR = Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = Platform::File::PATH_SEPARATOR

  attr_reader :path

  def initialize(path_or_fd, mode = "r", perm = 0666)
    if path_or_fd.kind_of?(Integer)
      super(path_or_fd)
      @path = nil
      return self
    end
    
    path = StringValue(path_or_fd)
    
    fd = IO.sysopen(path, mode, perm)
    
    if fd < 0
      Errno.handle path
    end

    super(fd)

    @path = path
  end

  def self.exist?(path)
    perform_stat(path) ? true : false
  end

  def self.file?(path)
    st = perform_stat path
    st ? st.file? : false
  end

  def self.directory?(path)
    st = perform_stat path
    st ? st.directory? : false
  end
  
  def self.symlink?(path)
    st = perform_stat path, false
    st ? st.symlink? : false
  end

  def self.blockdev?(path)
    st = perform_stat path
    st ? st.blockdev? : false
  end

  def self.chardev?(path)
    st = perform_stat path
    st ? st.chardev? : false
  end
  
  def self.pipe?(path)
    st = perform_stat path
    st ? st.pipe? : false
  end
  
  def self.socket?(path)
    st = perform_stat path
    st ? st.socket? : false
  end

  FILE_TYPES = {
    :dir => 'directory',
    :char => 'characterSpecial',
    :block => 'blockSpecial',
    :fifo => 'fifo',
    :link => 'link',
    :socket => 'socket',
    :file => 'file'
  }
  
  def self.ftype(path)
    kind = lstat(path).kind
    FILE_TYPES.include?(kind) ? FILE_TYPES[kind] : 'unknown'
  end
  
  def self.split(path)
    p = StringValue(path)
    [dirname(p), basename(p)]
  end

  def self.zero?(path)
    st = perform_stat path
    st ? st.zero? : false
  end

  def self.size(path)
    stat(path).size
  end

  def self.size?(path)
    st = perform_stat path
    st ? st.size : nil
  end

  def self.executable?(path)
    st = perform_stat path
    st ? st.executable? : false
  end

  def self.executable_real?(path)
    st = perform_stat path
    st ? st.executable_real? : false
  end

  def self.readable?(path)
    st = perform_stat path
    st ? st.readable? : false
  end

  def self.readable_real?(path)
    st = perform_stat path
    st ? st.readable_real? : false
  end
  
  def self.writable?(path)
    st = perform_stat path
    st ? st.writable? : false
  end

  def self.writable_real?(path)
    st = perform_stat path
    st ? st.writable_real? : false
  end

  def self.unlink(*paths)
    paths.each do |path|
      path = StringValue(path)

      n = Platform::POSIX.unlink(path)
      Errno.handle if n == -1
    end

    paths.size
  end
  
  def self.link(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = Platform::POSIX.link(from, to)
    Errno.handle if n == -1
    n
  end
  
  def self.symlink(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = Platform::POSIX.symlink(from, to)
    Errno.handle if n == -1
    n
  end

  def self.rename(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = Platform::POSIX.rename(from, to)
    Errno.handle if n == -1
    n
  end
  
  def self.readlink(path)
    StringValue(path)

    buf = " " * 1024

    n = Platform::POSIX.readlink(path, buf, buf.length)
    Errno.handle if n == -1

    buf[0, n]
  end
  
  def self.identical?(orig, copy)
    st_o = stat(StringValue(orig))
    st_c = stat(StringValue(copy))
    
    return false unless st_o.kind == st_c.kind
    return false unless st_o.ino == st_c.ino
    return false unless Platform::POSIX.access(orig, Constants::R_OK)
    return false unless Platform::POSIX.access(copy, Constants::R_OK)
    
    true
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

  def self.chmod(mode, *paths)
    paths.each { |path| Platform::POSIX.chmod(path, mode) }
    paths.size
  end
    
  def chmod(mode)
    Platform::POSIX.fchmod(@descriptor, mode)
  end
  
  class TimeVal < FFI::Struct
    config 'rbx.platform.timeval', :tv_sec, :tv_usec
  end
  
  def self.utime(a_in, m_in, *paths)
    ptr = MemoryPointer.new(TimeVal, 2)
    atime = TimeVal.new ptr
    mtime = TimeVal.new ptr[1]
    atime[:tv_sec] = a_in.to_i
    atime[:tv_usec] = 0
    
    mtime[:tv_sec] = m_in.to_i
    mtime[:tv_usec] = 0
    
    paths.each do |path|      
      if Platform::POSIX.utimes(path, ptr) != 0
        Errno.handle
      end
    end
    
    ptr.free
  end
  
  def self.atime(path)
    Time.at stat(path).atime
  end
  
  def atime
    Time.at self.class.stat(@path).atime
  end

  def self.mtime(path)
    Time.at stat(path).mtime
  end

  def self.ctime(path)
    Time.at stat(path).ctime
  end
  
  def ctime
    Time.at self.class.stat(@path).ctime
  end

  def self.dirname(path)
    path = StringValue(path)
    Platform::File.dirname(path)
  end

  def self.basename(path,ext = "")
    path = StringValue(path)
    ext = StringValue(ext)
    Platform::File.basename(path,ext)
  end

  def self.expand_path(path, dir_string = nil)
    Platform::File.expand_path(path, dir_string)
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
          part.gsub(/\\([^.])/, '\1')
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

  # FIXME: will fail a bunch if platfrom == :mswin
  def self.join(*args)
    args = args.flatten.map { |arg| StringValue(arg) }
    args.join(SEPARATOR).gsub(/#{SEPARATOR}+/,SEPARATOR)    
  end
  
  class << self
    alias_method :delete, :unlink
    alias_method :exists?, :exist?
    alias_method :fnmatch?, :fnmatch
  end
  
  class Stat
    self.instance_fields = 13
    ivar_as_index :inode => 0, :mode => 1, :kind => 2, :owner => 3, :group => 4,
                  :size => 5, :block => 6, :atime => 7, :mtime => 8,
                  :ctime => 9, :path => 10, :blksize => 11, :dev => 12

    def dev;     @dev; end
    def ino;     @inode; end
    def mode;    @mode; end
    def kind;    @kind; end
    def owner;   @owner; end
    def group;   @group; end
    def size;    @size; end
    def block;   @block; end
    def atime;   Time.at(@atime); end
    def mtime;   Time.at(@mtime); end
    def ctime;   Time.at(@ctime); end
    def path;    @path; end
    def blksize; @blksize; end
    
    alias_method :gid, :group
    alias_method :uid, :owner

    def ftype
      case @kind
      when :file
        "file"
      when :dir
        "directory"
      when :block
        "blockSpecial"
      when :char
        "characterSpecial"
      when :fifo
        "fifo"
      when :link
        "link"
      when :socket
        "socket"
      else
        "unknown"
      end
    end

    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} path=#{@path} kind=#{@kind}>"
    end
    
    def grpowned?
      @group == Platform::POSIX.getegid
    end
    
    def rgrpowned?
      @group == Platform::POSIX.getgid
    end
    private :rgrpowned?

    def size?
      if @size == 0
        nil
      else
        @size
      end
    end

    def zero?
      @size == 0
    end

    def blockdev?
      @kind == :block
    end

    def chardev?
      @kind == :char
    end

    def directory?
      @kind == :dir
    end

    def file?
      @kind == :file
    end
    
    def owned?
      @owner == Platform::POSIX.geteuid
    end

    def pipe?
      @kind == :fifo
    end
    
    def rowned?
      @owner == Platform::POSIX.getuid
    end
    private :rowned?
    
    def superuser?
      Platform::POSIX.geteuid == 0
    end
    private :superuser?
    
    def rsuperuser?
      Platform::POSIX.getuid == 0
    end
    private :rsuperuser?

    def socket?
      @kind == :socket
    end

    def symlink?
      @kind == :link
    end
    
    def executable?
      return true if superuser?
      return @mode & IO::S_IXUSR != 0 if owned?
      return @mode & IO::S_IXGRP != 0 if grpowned?
      return @mode & IO::S_IXOTH != 0
    end
    
    def executable_real?
      return true if rsuperuser?
      return @mode & IO::S_IXUSR != 0 if rowned?
      return @mode & IO::S_IXGRP != 0 if rgrpowned?
      return @mode & IO::S_IXOTH != 0
    end
    
    def readable?
      return true if superuser?
      return @mode & IO::S_IRUSR != 0 if owned?
      return @mode & IO::S_IRGRP != 0 if grpowned?
      return @mode & IO::S_IROTH != 0
    end
    
    def readable_real?
      return true if rsuperuser?
      return @mode & IO::S_IRUSR != 0 if rowned?
      return @mode & IO::S_IRGRP != 0 if rgrpowned?
      return @mode & IO::S_IROTH != 0
    end
    
    def writable?
      return true if superuser?
      return @mode & IO::S_IWUSR != 0 if owned?
      return @mode & IO::S_IWGRP != 0 if grpowned?
      return @mode & IO::S_IWOTH != 0
    end
    
    def writable_real?
      return true if rsuperuser?
      return @mode & IO::S_IWUSR != 0 if rowned?
      return @mode & IO::S_IWGRP != 0 if rgrpowned?
      return @mode & IO::S_IWOTH != 0
    end
  end
  
  def self.stat(path)
    perform_stat(path, true, true)
  end
  
  def stat
    self.class.stat(@path)
  end
  
  def self.lstat(path)
    perform_stat(path, false, true)
  end
  
  def lstat
    self.class.lstat(@path)
  end
  
  def self.perform_stat(path, follow_links=true, complain=false)
    out = Stat.stat(StringValue(path), follow_links)
    return out if out.is_a?(Stat) or not complain

    Errno.handle path
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
  
  def self.umask(mask = nil)
    if mask
      Platform::POSIX.umask(mask)
    else
      old_mask = Platform::POSIX.umask(0)
      Platform::POSIX.umask(old_mask)
      old_mask
    end
  end

  def self.truncate(path, length)
    unless self.exist?(path)
      raise Errno::ENOENT, path
    end
  end
end
