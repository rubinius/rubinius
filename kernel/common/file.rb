# -*- encoding: us-ascii -*-

module FFI::Platform::POSIX
  #--
  # Internal class for accessing timevals
  #++
  class TimeVal < FFI::Struct
    config 'rbx.platform.timeval', :tv_sec, :tv_usec
  end
end

class File < IO
  include Enumerable

  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end

  module Constants
    F_GETFL  = Rubinius::Config['rbx.platform.fcntl.F_GETFL']
    F_SETFL  = Rubinius::Config['rbx.platform.fcntl.F_SETFL']

    # O_ACCMODE is /undocumented/ for fcntl() on some platforms
    ACCMODE  = Rubinius::Config['rbx.platform.fcntl.O_ACCMODE']

    F_GETFD  = Rubinius::Config['rbx.platform.fcntl.F_GETFD']
    F_SETFD  = Rubinius::Config['rbx.platform.fcntl.F_SETFD']
    FD_CLOEXEC = Rubinius::Config['rbx.platform.fcntl.FD_CLOEXEC']

    RDONLY   = Rubinius::Config['rbx.platform.file.O_RDONLY']
    WRONLY   = Rubinius::Config['rbx.platform.file.O_WRONLY']
    RDWR     = Rubinius::Config['rbx.platform.file.O_RDWR']

    CREAT    = Rubinius::Config['rbx.platform.file.O_CREAT']
    EXCL     = Rubinius::Config['rbx.platform.file.O_EXCL']
    NOCTTY   = Rubinius::Config['rbx.platform.file.O_NOCTTY']
    TRUNC    = Rubinius::Config['rbx.platform.file.O_TRUNC']
    APPEND   = Rubinius::Config['rbx.platform.file.O_APPEND']
    NONBLOCK = Rubinius::Config['rbx.platform.file.O_NONBLOCK']
    SYNC     = Rubinius::Config['rbx.platform.file.O_SYNC']

    # TODO: these flags should probably be imported from Platform
    LOCK_SH  = 0x01
    LOCK_EX  = 0x02
    LOCK_NB  = 0x04
    LOCK_UN  = 0x08
    BINARY   = 0x04

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

  SEPARATOR = FFI::Platform::File::SEPARATOR
  Separator = FFI::Platform::File::SEPARATOR
  ALT_SEPARATOR = FFI::Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = FFI::Platform::File::PATH_SEPARATOR
  POSIX = FFI::Platform::POSIX

  attr_reader :path

  # The mode_t type is 2 bytes (ushort). Instead of getting whatever
  # value happens to be in the least significant 16 bits, just set
  # the value to 0 if it is greater than 0xffff. Also, negative values
  # don't make any sense here.
  def clamp_short(value)
    mode = Rubinius::Type.coerce_to value, Integer, :to_int
    mode < 0 || mode > 0xffff ? 0 : mode
  end
  module_function :clamp_short

  ##
  # Returns the last access time for the named file as a Time object).
  #
  #  File.atime("testfile")   #=> Wed Apr 09 08:51:48 CDT 2003
  def self.atime(path)
    Stat.new(path).atime
  end

  ##
  # Returns the last component of the filename given
  # in file_name, which must be formed using forward
  # slashes (``/’’) regardless of the separator used
  # on the local file system. If suffix is given and
  # present at the end of file_name, it is removed.
  #
  #  File.basename("/home/gumby/work/ruby.rb")          #=> "ruby.rb"
  #  File.basename("/home/gumby/work/ruby.rb", ".rb")   #=> "ruby"
  def self.basename(path, ext=undefined)
    path = Rubinius::Type.coerce_to_path(path)

    slash = "/"

    ext_not_present = ext.equal?(undefined)

    if pos = path.find_string_reverse(slash, path.size)
      # special case. If the string ends with a /, ignore it.
      if pos == path.size - 1

        # Find the first non-/ from the right
        data = path.data
        found = false
        pos.downto(0) do |i|
          if data[i] != 47  # ?/
            path = path.byteslice(0, i+1)
            found = true
            break
          end
        end

        # edge case, it's all /'s, return "/"
        return slash unless found

        # Now that we've trimmed the /'s at the end, search again
        pos = path.find_string_reverse(slash, path.size)
        if ext_not_present and !pos
          # No /'s found and ext not present, return path.
          return path
        end
      end

      path = path.byteslice(pos + 1, path.size - pos) if pos
    end

    return path if ext_not_present

    # special case. if ext is ".*", remove any extension

    ext = StringValue(ext)

    if ext == ".*"
      if pos = path.rindex(?.)
        return path.byteslice(0, pos)
      end
    elsif pos = path.rindex(ext)
      # Check that ext is the last thing in the string
      if pos == path.size - ext.size
        return path.byteslice(0, pos)
      end
    end

    return path
  end

  ##
  # Returns true if the named file is a block device.
  def self.blockdev?(path)
    st = Stat.stat path
    st ? st.blockdev? : false
  end

  ##
  # Returns true if the named file is a character device.
  def self.chardev?(path)
    st = Stat.stat path
    st ? st.chardev? : false
  end

  ##
  # Changes permission bits on the named file(s) to
  # the bit pattern represented by mode_int. Actual
  # effects are operating system dependent (see the
  # beginning of this section). On Unix systems, see
  # chmod(2) for details. Returns the number of files processed.
  #
  #  File.chmod(0644, "testfile", "out")   #=> 2
  def self.chmod(mode, *paths)
    mode = clamp_short mode

    paths.each do |path|
      n = POSIX.chmod Rubinius::Type.coerce_to_path(path), mode
      Errno.handle if n == -1
    end
    paths.size
  end

  ##
  # Equivalent to File::chmod, but does not follow symbolic
  # links (so it will change the permissions associated with
  # the link, not the file referenced by the link).
  # Often not available.
  def self.lchmod(mode, *paths)
    mode = Rubinius::Type.coerce_to(mode, Integer, :to_int)

    paths.each do |path|
      n = POSIX.lchmod Rubinius::Type.coerce_to_path(path), mode
      Errno.handle if n == -1
    end

    paths.size
  end

  ##
  # Changes the owner and group of the
  # named file(s) to the given numeric owner
  # and group id‘s. Only a process with superuser
  # privileges may change the owner of a file. The
  # current owner of a file may change the file‘s
  # group to any group to which the owner belongs.
  # A nil or -1 owner or group id is ignored.
  # Returns the number of files processed.
  #
  #  File.chown(nil, 100, "testfile")
  def self.chown(owner, group, *paths)
    if owner
      owner = Rubinius::Type.coerce_to(owner, Integer, :to_int)
    else
      owner = -1
    end

    if group
      group = Rubinius::Type.coerce_to(group, Integer, :to_int)
    else
      group = -1
    end

    paths.each do |path|
      n = POSIX.chown Rubinius::Type.coerce_to_path(path), owner, group
      Errno.handle if n == -1
    end

    paths.size
  end

  def chmod(mode)
    mode = Rubinius::Type.coerce_to(mode, Integer, :to_int)
    n = POSIX.fchmod @descriptor, clamp_short(mode)
    Errno.handle if n == -1
    n
  end

  def chown(owner, group)
    if owner
      owner = Rubinius::Type.coerce_to(owner, Integer, :to_int)
    else
      owner = -1
    end

    if group
      group = Rubinius::Type.coerce_to(group, Integer, :to_int)
    else
      group = -1
    end

    n = POSIX.fchown @descriptor, owner, group
    Errno.handle if n == -1
    n
  end

  ##
  # Equivalent to File::chown, but does not follow
  # symbolic links (so it will change the owner
  # associated with the link, not the file referenced
  # by the link). Often not available. Returns number
  # of files in the argument list.
  def self.lchown(owner, group, *paths)
    if owner
      owner = Rubinius::Type.coerce_to(owner, Integer, :to_int)
    else
      owner = -1
    end

    if group
      group = Rubinius::Type.coerce_to(group, Integer, :to_int)
    else
      group = -1
    end

    paths.each do |path|
      n = POSIX.lchown Rubinius::Type.coerce_to_path(path), owner, group
      Errno.handle if n == -1
    end

    paths.size
  end

  ##
  # Returns the change time for the named file (the
  # time at which directory information about the
  # file was changed, not the file itself).
  #
  #  File.ctime("testfile")   #=> Wed Apr 09 08:53:13 CDT 2003
  def self.ctime(path)
    Stat.new(path).ctime
  end

  ##
  # Returns true if the named file is a directory, false otherwise.
  #
  # File.directory?(".")
  def self.directory?(io_or_path)
    io = Rubinius::Type.try_convert io_or_path, IO, :to_io

    if io.is_a? IO
      Stat.from_fd(io.fileno).directory?
    else
      st = Stat.stat io_or_path
      st ? st.directory? : false
    end
  end

  def self.last_nonslash(path, start=nil)
    # Find the first non-/ from the right
    data = path.data
    idx = nil
    start ||= (path.size - 1)

    start.downto(0) do |i|
      if data[i] != 47  # ?/
        return i
      end
    end

    return nil
  end

  ##
  # Returns all components of the filename given in
  # file_name except the last one. The filename must be
  # formed using forward slashes (``/’’) regardless of
  # the separator used on the local file system.
  #
  #  File.dirname("/home/gumby/work/ruby.rb")   #=> "/home/gumby/work"
  def self.dirname(path)
    path = Rubinius::Type.coerce_to_path(path)

    # edge case
    return "." if path.empty?

    slash = "/"

    # pull off any /'s at the end to ignore
    chunk_size = last_nonslash(path)
    return "/" unless chunk_size

    if pos = path.find_string_reverse(slash, chunk_size)
      return "/" if pos == 0

      path = path.byteslice(0, pos)

      return "/" if path == "/"

      return path unless path.suffix? slash

      # prune any trailing /'s
      idx = last_nonslash(path, pos)

      # edge case, only /'s, return /
      return "/" unless idx

      return path.byteslice(0, idx - 1)
    end

    return "."
  end

  ##
  # Returns true if the named file is executable by the
  # effective user id of this process.
  def self.executable?(path)
    st = Stat.stat path
    st ? st.executable? : false
  end

  ##
  # Returns true if the named file is executable by
  # the real user id of this process.
  def self.executable_real?(path)
    st = Stat.stat path
    st ? st.executable_real? : false
  end

  ##
  # Return true if the named file exists.
  def self.exist?(path)
    POSIX.stat(Rubinius::Type.coerce_to_path(path), Stat::EXISTS_STRUCT.pointer) == 0
  end

  # Pull a constant for Dir local to File so that we don't have to depend
  # on the global Dir constant working. This sounds silly, I know, but it's a
  # little bit of defensive coding so Rubinius can run things like fakefs better.
  PrivateDir = ::Dir

  ##
  # Converts a pathname to an absolute pathname. Relative
  # paths are referenced from the current working directory
  # of the process unless dir_string is given, in which case
  # it will be used as the starting point. The given pathname
  # may start with a ``~’’, which expands to the process owner‘s
  # home directory (the environment variable HOME must be set
  # correctly). "~user" expands to the named user‘s home directory.
  #
  #  File.expand_path("~oracle/bin")           #=> "/home/oracle/bin"
  #  File.expand_path("../../bin", "/tmp/x")   #=> "/bin"
  def self.expand_path(path, dir=nil)
    path = Rubinius::Type.coerce_to_path(path)

    first = path[0]
    if first == ?~
      case path[1]
      when ?/
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        path = ENV["HOME"] + path.byteslice(1, path.size - 1)
      when nil
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        if home.empty?
          raise ArgumentError, "HOME environment variable is empty expanding '~'"
        end

        return home
      else
        unless length = path.index("/", 1)
          length = path.size
        end

        name = path.byteslice 1, length - 1
        unless dir = Rubinius.get_user_home(name)
          raise ArgumentError, "user #{name} does not exist"
        end

        path = dir + path.byteslice(length, path.size - length)
      end
    elsif first != ?/
      if dir
        dir = expand_path dir
      else
        dir = PrivateDir.pwd
      end

      path = "#{dir}/#{path}"
    end

    items = []
    start = 0
    size = path.size

    while index = path.index("/", start) or (start < size and index = size)
      length = index - start

      if length > 0
        item = path.byteslice start, length

        if item == ".."
          items.pop
        elsif item != "."
          items << item
        end
      end

      start = index + 1
    end

    return Rubinius::Type.external_encoding_string("/") if items.empty?

    str = ""
    items.each { |x| str.append "/#{x}" }

    return Rubinius::Type.external_encoding_string(str)
  end

  ##
  # Returns the extension (the portion of file name in
  # path after the period).
  #
  #  File.extname("test.rb")         #=> ".rb"
  #  File.extname("a/b/d/test.rb")   #=> ".rb"
  #  File.extname("test")            #=> ""
  #  File.extname(".profile")        #=> ""
  def self.extname(path)
    path = Rubinius::Type.coerce_to_path(path)
    path_size = path.size

    dot_idx = path.find_string_reverse(".", path_size)

    # No dots at all
    return "" unless dot_idx

    slash_idx = path.find_string_reverse("/", path_size)

    # pretend there is / just to the left of the start of the string
    slash_idx ||= -1

    # no . in the last component of the path
    return "" if dot_idx < slash_idx

    # last component starts with a .
    return "" if dot_idx == slash_idx + 1

    # last component ends with a .
    return "" if dot_idx == path_size - 1

    return path.byteslice(dot_idx, path_size - dot_idx)
  end

  ##
  # Returns true if the named file exists and is a regular file.
  def self.file?(path)
    st = Stat.stat path
    st ? st.file? : false
  end

  ##
  # Returns true if path matches against pattern The pattern
  # is not a regular expression; instead it follows rules
  # similar to shell filename globbing. It may contain the
  # following metacharacters:
  #
  # *:  Matches any file. Can be restricted by other values in the glob. * will match all files; c* will match all files beginning with c; *c will match all files ending with c; and c will match all files that have c in them (including at the beginning or end). Equivalent to / .* /x in regexp.
  # **:  Matches directories recursively or files expansively.
  # ?:  Matches any one character. Equivalent to /.{1}/ in regexp.
  # [set]:  Matches any one character in set. Behaves exactly like character sets in Regexp, including set negation ([^a-z]).
  # <code></code>:  Escapes the next metacharacter.
  # flags is a bitwise OR of the FNM_xxx parameters. The same glob pattern and flags are used by Dir::glob.
  #
  #  File.fnmatch('cat',       'cat')        #=> true  : match entire string
  #  File.fnmatch('cat',       'category')   #=> false : only match partial string
  #  File.fnmatch('c{at,ub}s', 'cats')       #=> false : { } isn't supported
  #
  #  File.fnmatch('c?t',     'cat')          #=> true  : '?' match only 1 character
  #  File.fnmatch('c??t',    'cat')          #=> false : ditto
  #  File.fnmatch('c*',      'cats')         #=> true  : '*' match 0 or more characters
  #  File.fnmatch('c*t',     'c/a/b/t')      #=> true  : ditto
  #  File.fnmatch('ca[a-z]', 'cat')          #=> true  : inclusive bracket expression
  #  File.fnmatch('ca[^t]',  'cat')          #=> false : exclusive bracket expression ('^' or '!')
  #
  #  File.fnmatch('cat', 'CAT')                     #=> false : case sensitive
  #  File.fnmatch('cat', 'CAT', File::FNM_CASEFOLD) #=> true  : case insensitive
  #
  #  File.fnmatch('?',   '/', File::FNM_PATHNAME)  #=> false : wildcard doesn't match '/' on FNM_PATHNAME
  #  File.fnmatch('*',   '/', File::FNM_PATHNAME)  #=> false : ditto
  #  File.fnmatch('[/]', '/', File::FNM_PATHNAME)  #=> false : ditto
  #
  #  File.fnmatch('\?',   '?')                       #=> true  : escaped wildcard becomes ordinary
  #  File.fnmatch('\a',   'a')                       #=> true  : escaped ordinary remains ordinary
  #  File.fnmatch('\a',   '\a', File::FNM_NOESCAPE)  #=> true  : FNM_NOESACPE makes '\' ordinary
  #  File.fnmatch('[\?]', '?')                       #=> true  : can escape inside bracket expression
  #
  #  File.fnmatch('*',   '.profile')                      #=> false : wildcard doesn't match leading
  #  File.fnmatch('*',   '.profile', File::FNM_DOTMATCH)  #=> true    period by default.
  #  File.fnmatch('.*',  '.profile')                      #=> true
  #
  #  rbfiles = '**' '/' '*.rb' # you don't have to do like this. just write in single string.
  #  File.fnmatch(rbfiles, 'main.rb')                    #=> false
  #  File.fnmatch(rbfiles, './main.rb')                  #=> false
  #  File.fnmatch(rbfiles, 'lib/song.rb')                #=> true
  #  File.fnmatch('**.rb', 'main.rb')                    #=> true
  #  File.fnmatch('**.rb', './main.rb')                  #=> false
  #  File.fnmatch('**.rb', 'lib/song.rb')                #=> true
  #  File.fnmatch('*',           'dave/.profile')                      #=> true
  #
  #  pattern = '*' '/' '*'
  #  File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME)  #=> false
  #  File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  #
  #  pattern = '**' '/' 'foo'
  #  File.fnmatch(pattern, 'a/b/c/foo', File::FNM_PATHNAME)     #=> true
  #  File.fnmatch(pattern, '/a/b/c/foo', File::FNM_PATHNAME)    #=> true
  #  File.fnmatch(pattern, 'c:/a/b/c/foo', File::FNM_PATHNAME)  #=> true
  #  File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME)    #=> false
  #  File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  def self.fnmatch(pattern, path, flags=0)
    pattern = StringValue(pattern)
    path    = Rubinius::Type.coerce_to_path(path)
    flags   = Rubinius::Type.coerce_to(flags, Fixnum, :to_int)

    super pattern, path, flags
  end

  ##
  # Identifies the type of the named file; the return string is
  # one of "file", "directory", "characterSpecial",
  # "blockSpecial", "fifo", "link", "socket", or "unknown".
  #
  #  File.ftype("testfile")            #=> "file"
  #  File.ftype("/dev/tty")            #=> "characterSpecial"
  #  File.ftype("/tmp/.X11-unix/X0")   #=> "socket"
  def self.ftype(path)
    lstat(path).ftype
  end

  ##
  # Returns true if the named file exists and the effective
  # group id of the calling process is the owner of the file.
  # Returns false on Windows.
  def self.grpowned?(path)
    begin
      lstat(path).grpowned?
    rescue
      false
    end
  end

  ##
  # Returns true if the named files are identical.
  #
  #   open("a", "w") {}
  #   p File.identical?("a", "a")      #=> true
  #   p File.identical?("a", "./a")    #=> true
  #   File.link("a", "b")
  #   p File.identical?("a", "b")      #=> true
  #   File.symlink("a", "c")
  #   p File.identical?("a", "c")      #=> true
  #   open("d", "w") {}
  #   p File.identical?("a", "d")      #=> false
  def self.identical?(orig, copy)
    orig = Rubinius::Type.coerce_to_path(orig)
    st_o = File::Stat.stat(orig)
    copy = Rubinius::Type.coerce_to_path(copy)
    st_c = File::Stat.stat(copy)

    return false if st_o.nil? || st_c.nil?

    return false unless st_o.dev == st_c.dev
    return false unless st_o.ino == st_c.ino
    return false unless st_o.ftype == st_c.ftype
    return false unless POSIX.access(orig, Constants::R_OK)
    return false unless POSIX.access(copy, Constants::R_OK)

    true
  end

  ##
  # Returns a new string formed by joining the strings using File::SEPARATOR.
  #
  #  File.join("usr", "mail", "gumby")   #=> "usr/mail/gumby"
  def self.join(*args)
    return '' if args.empty?

    sep = SEPARATOR

    # The first one is unrolled out of the loop to remove a condition
    # from the loop. It seems needless, but you'd be surprised how much hinges
    # on the performance of File.join
    #
    first = args.shift
    case first
    when String
      first = first.dup
    when Array
      recursion = Thread.detect_recursion(first) do
        first = join(*first)
      end

      raise ArgumentError, "recursive array" if recursion
    else
      # We need to use dup here, since it's possible that
      # StringValue gives us a direct object we shouldn't mutate
      first = Rubinius::Type.coerce_to_path(first).dup
    end

    ret = first

    args.each do |el|
      value = nil

      case el
      when String
        value = el
      when Array
        recursion = Thread.detect_recursion(el) do
          value = join(*el)
        end

        raise ArgumentError, "recursive array" if recursion
      else
        value = Rubinius::Type.coerce_to_path(el)
      end

      if value.prefix? sep
        ret.gsub!(/#{SEPARATOR}+$/, '')
      elsif not ret.suffix? sep
        ret << sep
      end

      ret << value
    end
    ret
  end

  ##
  # Creates a new name for an existing file using a hard link.
  # Will not overwrite new_name if it already exists (raising
  # a subclass of SystemCallError). Not available on all platforms.
  #
  #  File.link("testfile", ".testfile")   #=> 0
  #  IO.readlines(".testfile")[0]         #=> "This is line one\n"
  def self.link(from, to)
    n = POSIX.link Rubinius::Type.coerce_to_path(from), Rubinius::Type.coerce_to_path(to)
    Errno.handle if n == -1
    n
  end

  ##
  # Same as File::stat, but does not follow the last symbolic link.
  # Instead, reports on the link itself.
  #
  #  File.symlink("testfile", "link2test")   #=> 0
  #  File.stat("testfile").size              #=> 66
  #  File.lstat("link2test").size            #=> 8
  #  File.stat("link2test").size             #=> 66
  def self.lstat(path)
    Stat.lstat path
  end

  ##
  # Returns the modification time for the named file as a Time object.
  #
  #  File.mtime("testfile")   #=> Tue Apr 08 12:58:04 CDT 2003
  def self.mtime(path)
    Stat.new(path).mtime
  end

  ##
  # Returns true if the named file is a pipe.
  def self.pipe?(path)
    st = Stat.stat path
    st ? st.pipe? : false
  end

  ##
  # Returns true if the named file is readable by the effective
  # user id of this process.
  def self.readable?(path)
    st = Stat.stat path
    st ? st.readable? : false
  end

  ##
  # Returns true if the named file is readable by the real user
  # id of this process.
  def self.readable_real?(path)
    st = Stat.stat path
    st ? st.readable_real? : false
  end

  ##
  # Returns the name of the file referenced by the given link.
  # Not available on all platforms.
  #
  #  File.symlink("testfile", "link2test")   #=> 0
  #  File.readlink("link2test")              #=> "testfile"
  def self.readlink(path)
    FFI::MemoryPointer.new(1024) do |ptr|
      n = POSIX.readlink Rubinius::Type.coerce_to_path(path), ptr, 1024
      Errno.handle if n == -1

      return ptr.read_string(n)
    end
  end

  ##
  # Renames the given file to the new name. Raises a SystemCallError
  # if the file cannot be renamed.
  #
  #  File.rename("afile", "afile.bak")   #=> 0
  def self.rename(from, to)
    n = POSIX.rename Rubinius::Type.coerce_to_path(from), Rubinius::Type.coerce_to_path(to)
    Errno.handle if n == -1
    n
  end

  ##
  # Returns the size of file_name.
  def self.size(io_or_path)
    io = Rubinius::Type.try_convert io_or_path, IO, :to_io

    if io.is_a? IO
      Stat.from_fd(io.fileno).size
    else
      stat(io_or_path).size
    end
  end

  ##
  # Returns nil if file_name doesn‘t exist or has zero size,
  # the size of the file otherwise.
  def self.size?(io_or_path)
    s = 0

    io = Rubinius::Type.try_convert io_or_path, IO, :to_io

    if io.is_a? IO
      s = Stat.from_fd(io.fileno).size
    else
      st = Stat.stat io_or_path
      s = st.size if st
    end

    s > 0 ? s : nil
  end

  ##
  # Returns true if the named file is a socket.
  def self.socket?(path)
    st = Stat.stat path
    st ? st.socket? : false
  end

  ##
  # Splits the given string into a directory and a file component and returns them in a two-element array. See also File::dirname and File::basename.
  #
  #  File.split("/home/gumby/.profile")   #=> ["/home/gumby", ".profile"]
  def self.split(path)
    p = Rubinius::Type.coerce_to_path(path)
    [dirname(p), basename(p)]
  end

  ##
  # Returns a File::Stat object for the named file (see File::Stat).
  #
  #  File.stat("testfile").mtime   #=> Tue Apr 08 12:58:04 CDT 2003
  def self.stat(path)
    Stat.new path
  end

  ##
  # Creates a symbolic link called new_name for the
  # existing file old_name. Raises a NotImplemented
  # exception on platforms that do not support symbolic links.
  #
  #  File.symlink("testfile", "link2test")   #=> 0
  def self.symlink(from, to)
    n = POSIX.symlink Rubinius::Type.coerce_to_path(from), Rubinius::Type.coerce_to_path(to)
    Errno.handle if n == -1
    n
  end

  ##
  # Returns true if the named file is a symbolic link.
  def self.symlink?(path)
    Stat.lstat(path).symlink?
  rescue Errno::ENOENT, Errno::ENODIR
    false
  end

  ##
  # Copies a file from to to. If to is a directory, copies from to to/from.
  def self.syscopy(from, to)
    out = directory?(to) ? to + basename(from) : to

    open(out, 'w') do |f|
      f.write read(from).read
    end
  end

  ##
  # Truncates the file file_name to be at most integer
  # bytes long. Not available on all platforms.
  #
  #  f = File.new("out", "w")
  #  f.write("1234567890")     #=> 10
  #  f.close                   #=> nil
  #  File.truncate("out", 5)   #=> 0
  #  File.size("out")          #=> 5
  def self.truncate(path, length)
    path = Rubinius::Type.coerce_to_path(path)

    unless exist?(path)
      raise Errno::ENOENT, path
    end

    length = Rubinius::Type.coerce_to length, Integer, :to_int

    n = POSIX.truncate(path, length)
    Errno.handle if n == -1
    n
  end

  ##
  # Returns the current umask value for this process.
  # If the optional argument is given, set the umask
  # to that value and return the previous value. Umask
  # values are subtracted from the default permissions,
  # so a umask of 0222 would make a file read-only for
  # everyone.
  #
  #  File.umask(0006)   #=> 18
  #  File.umask         #=> 6
  def self.umask(mask = nil)
    if mask
      POSIX.umask clamp_short(mask)
    else
      old_mask = POSIX.umask(0)
      POSIX.umask old_mask
      old_mask
    end
  end

  ##
  # Deletes the named files, returning the number of names
  # passed as arguments. Raises an exception on any error.
  #
  # See also Dir::rmdir.
  def self.unlink(*paths)
    paths.each do |path|
      n = POSIX.unlink Rubinius::Type.coerce_to_path(path)
      Errno.handle if n == -1
    end

    paths.size
  end

  ##
  # Sets the access and modification times of each named
  # file to the first two arguments. Returns the number
  # of file names in the argument list.
  #  #=> Integer
  def self.utime(a_in, m_in, *paths)
    a_in ||= Time.now
    m_in ||= Time.now
    FFI::MemoryPointer.new(POSIX::TimeVal, 2) do |ptr|
      atime = POSIX::TimeVal.new ptr
      mtime = POSIX::TimeVal.new ptr[1]
      atime[:tv_sec] = a_in.to_i
      atime[:tv_usec] = 0

      mtime[:tv_sec] = m_in.to_i
      mtime[:tv_usec] = 0

      paths.each do |path|

        n = POSIX.utimes(Rubinius::Type.coerce_to_path(path), ptr)
        Errno.handle unless n == 0
      end
    end
  end

  ##
  # Returns true if the named file is writable by the effective
  # user id of this process.
  def self.writable?(path)
    st = Stat.stat path
    st ? st.writable? : false
  end

  ##
  # Returns true if the named file is writable by the real user
  # id of this process.
  def self.writable_real?(path)
    st = Stat.stat path
    st ? st.writable_real? : false
  end

  ##
  # Returns true if the named file exists and has a zero size.
  def self.zero?(path)
    st = Stat.stat path
    st ? st.zero? : false
  end

  ##
  # Returns true if the named file exists and the effective
  # used id of the calling process is the owner of the file.
  #  File.owned?(file_name)   => true or false
  def self.owned?(file_name)
    Stat.new(file_name).owned?
  rescue Errno::ENOENT
    return false
  end

  ##
  # Returns true if the named file has the setgid bit set.
  def self.setgid?(file_name)
    Stat.new(file_name).setgid?
  rescue Errno::ENOENT
    return false
  end

  ##
  # Returns true if the named file has the setuid bit set.
  def self.setuid?(file_name)
    Stat.new(file_name).setuid?
  rescue Errno::ENOENT
    return false
  end

  ##
  # Returns true if the named file has the sticky bit set.
  def self.sticky?(file_name)
    Stat.new(file_name).sticky?
  rescue Errno::ENOENT
    return false
  end

  class << self
    alias_method :delete,   :unlink
    alias_method :exists?,  :exist?
    alias_method :fnmatch?, :fnmatch
  end

  def atime
    Stat.new(@path).atime
  end

  def reopen(other, mode = 'r+')
    rewind unless closed?
    unless other.kind_of? IO
      other = Rubinius::Type.coerce_to_path(other)
    end
    super(other, mode)
  end

  def ctime
    Stat.new(@path).ctime
  end

  def flock(const)
    const = Rubinius::Type.coerce_to const, Integer, :to_int

    result = POSIX.flock @descriptor, const

    return false if result == -1
    result
  end

  def lstat
    Stat.lstat @path
  end

  def mtime
    Stat.new(@path).mtime
  end

  def stat
    Stat.from_fd @descriptor
  end

  def truncate(length)
    length = Rubinius::Type.coerce_to length, Integer, :to_int

    ensure_open_and_writable
    raise Errno::EINVAL, "Can't truncate a file to a negative length" if length < 0

    flush
    reset_buffering
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

# Inject the constants into IO
class IO
  include File::Constants
end

class File::Stat
  class Struct < FFI::Struct
    config "rbx.platform.stat", :st_dev, :st_ino, :st_mode, :st_nlink,
           :st_uid, :st_gid, :st_rdev, :st_size, :st_blksize, :st_blocks,
           :st_atime, :st_mtime, :st_ctime
  end

  EXISTS_STRUCT = Struct.new

  include Comparable

  S_IRUSR  = Rubinius::Config['rbx.platform.file.S_IRUSR']
  S_IWUSR  = Rubinius::Config['rbx.platform.file.S_IWUSR']
  S_IXUSR  = Rubinius::Config['rbx.platform.file.S_IXUSR']
  S_IRGRP  = Rubinius::Config['rbx.platform.file.S_IRGRP']
  S_IWGRP  = Rubinius::Config['rbx.platform.file.S_IWGRP']
  S_IXGRP  = Rubinius::Config['rbx.platform.file.S_IXGRP']
  S_IROTH  = Rubinius::Config['rbx.platform.file.S_IROTH']
  S_IWOTH  = Rubinius::Config['rbx.platform.file.S_IWOTH']
  S_IXOTH  = Rubinius::Config['rbx.platform.file.S_IXOTH']

  S_IRUGO  = S_IRUSR | S_IRGRP | S_IROTH
  S_IWUGO  = S_IWUSR | S_IWGRP | S_IWOTH
  S_IXUGO  = S_IXUSR | S_IXGRP | S_IXOTH

  S_IFMT   = Rubinius::Config['rbx.platform.file.S_IFMT']
  S_IFIFO  = Rubinius::Config['rbx.platform.file.S_IFIFO']
  S_IFCHR  = Rubinius::Config['rbx.platform.file.S_IFCHR']
  S_IFDIR  = Rubinius::Config['rbx.platform.file.S_IFDIR']
  S_IFBLK  = Rubinius::Config['rbx.platform.file.S_IFBLK']
  S_IFREG  = Rubinius::Config['rbx.platform.file.S_IFREG']
  S_IFLNK  = Rubinius::Config['rbx.platform.file.S_IFLNK']
  S_IFSOCK = Rubinius::Config['rbx.platform.file.S_IFSOCK']
  S_IFWHT  = Rubinius::Config['rbx.platform.file.S_IFWHT']
  S_ISUID  = Rubinius::Config['rbx.platform.file.S_ISUID']
  S_ISGID  = Rubinius::Config['rbx.platform.file.S_ISGID']
  S_ISVTX  = Rubinius::Config['rbx.platform.file.S_ISVTX']

  POSIX    = FFI::Platform::POSIX

  attr_reader :path

  def self.create(path)
    path = Rubinius::Type.coerce_to_path path
    stat = allocate
    Rubinius.privately { stat.setup path, Struct.new }
  end

  def self.stat(path)
    stat = create path

    result = POSIX.stat stat.path, stat.pointer
    return nil unless result == 0

    stat
  end

  # --
  # Stat.lstat raises whereas Stat.stat does not because most things
  # that use Stat.stat do not expect exceptions but most things that
  # uses Stat.lstat do.
  # ++
  def self.lstat(path)
    stat = create path

    result = POSIX.lstat stat.path, stat.pointer
    Errno.handle path unless result == 0

    stat
  end

  ##
  # File::Stat#from_fd is used to support IO#stat which does not necessarily
  # have a path.

  def self.from_fd(descriptor)
    stat = allocate
    struct = Struct.new

    result = POSIX.fstat descriptor, struct.pointer
    Errno.handle "file descriptor #{descriptor}" unless result == 0

    Rubinius.privately { stat.setup nil, struct }
  end

  def initialize(path)
    @path = Rubinius::Type.coerce_to_path path
    @stat = Struct.new
    result = POSIX.stat @path, @stat.pointer
    Errno.handle path unless result == 0
  end

  private :initialize

  def setup(path, struct)
    @path = path
    @stat = struct
    self
  end

  private :setup

  def pointer
    @stat.pointer
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
    @stat[:st_mode] & S_ISGID != 0
  end

  def setuid?
    @stat[:st_mode] & S_ISUID != 0
  end

  def sticky?
    @stat[:st_mode] & S_ISVTX != 0
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

  def <=>(other)
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
