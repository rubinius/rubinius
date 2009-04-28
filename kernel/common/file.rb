module Platform::POSIX
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
    F_GETFL  = Rubinius::RUBY_CONFIG['rbx.platform.fcntl.F_GETFL']
    F_SETFL  = Rubinius::RUBY_CONFIG['rbx.platform.fcntl.F_SETFL']

    # O_ACCMODE is /undocumented/ for fcntl() on some platforms
    ACCMODE  = Rubinius::RUBY_CONFIG['rbx.platform.fcntl.O_ACCMODE']

    RDONLY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_RDONLY']
    WRONLY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_WRONLY']
    RDWR     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_RDWR']

    CREAT    = Rubinius::RUBY_CONFIG['rbx.platform.file.O_CREAT']
    EXCL     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_EXCL']
    NOCTTY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_NOCTTY']
    TRUNC    = Rubinius::RUBY_CONFIG['rbx.platform.file.O_TRUNC']
    APPEND   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_APPEND']
    NONBLOCK = Rubinius::RUBY_CONFIG['rbx.platform.file.O_NONBLOCK']
    SYNC     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_SYNC']

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

  SEPARATOR = Platform::File::SEPARATOR
  Separator = Platform::File::SEPARATOR
  ALT_SEPARATOR = Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = Platform::File::PATH_SEPARATOR
  POSIX = Platform::POSIX

  def self.open(path_or_fd, mode = "r", perm = 0666)
    file = new path_or_fd, mode, perm

    return file unless block_given?

    begin
      yield file
    ensure
      file.close rescue nil unless file.closed?
    end
  end

  def initialize(path_or_fd, mode = "r", perm = 0666)
    if path_or_fd.kind_of? Integer
      super(path_or_fd, mode)
      @path = nil
    else
      path = StringValue(path_or_fd)

      fd = IO.sysopen(path, mode, perm)
      Errno.handle path if fd < 0

      @path = path
      super(fd)
    end
  end

  private :initialize

  attr_reader :path

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
  def self.basename(path,ext = "")
    path = StringValue(path)
    ext = StringValue(ext)
    Platform::File.basename(path,ext)
  end

  ##
  # Returns true if the named file is a block device.
  def self.blockdev?(path)
    st = Stat.stat? path
    st ? st.blockdev? : false
  end

  ##
  # Returns true if the named file is a character device.
  def self.chardev?(path)
    st = Stat.stat? path
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
    mode = Type.coerce_to(mode, Integer, :to_int) unless mode.is_a? Integer
    paths.each do |path|
      path = Type.coerce_to(path, String, :to_str) unless path.is_a? String
      POSIX.chmod(path, mode)
    end
    paths.size
  end

  ##
  # Equivalent to File::chmod, but does not follow symbolic
  # links (so it will change the permissions associated with
  # the link, not the file referenced by the link).
  # Often not available.
  def self.lchmod(mode, *paths)
    mode = Type.coerce_to(mode, Integer, :to_int) unless mode.is_a? Integer
    paths.each do |path|
      path = Type.coerce_to(path, String, :to_str) unless path.is_a? String
      POSIX.lchmod(path, mode)
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
  def self.chown(owner_int, group_int, *paths)
    owner_int = -1 if owner_int == nil
    group_int = -1 if group_int == nil
    paths.each { |path| POSIX.chown(path, owner_int, group_int) }
    paths.size
  end

  ##
  # Equivalent to File::chown, but does not follow
  # symbolic links (so it will change the owner
  # associated with the link, not the file referenced
  # by the link). Often not available. Returns number
  # of files in the argument list.
  def self.lchown(owner_int, group_int, *paths)
    owner_int = -1 if owner_int == nil
    group_int = -1 if group_int == nil
    paths.each { |path| POSIX.lchown(path, owner_int, group_int) }
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
  def self.directory?(path)
    st = Stat.stat? path
    st ? st.directory? : false
  end

  ##
  # Returns all components of the filename given in
  # file_name except the last one. The filename must be
  # formed using forward slashes (``/’’) regardless of
  # the separator used on the local file system.
  # 
  #  File.dirname("/home/gumby/work/ruby.rb")   #=> "/home/gumby/work"
  def self.dirname(path)
    path = StringValue(path)
    Platform::File.dirname(path)
  end

  ##
  # Returns true if the named file is executable by the
  # effective user id of this process.
  def self.executable?(path)
    st = Stat.stat? path
    st ? st.executable? : false
  end

  ##
  # Returns true if the named file is executable by
  # the real user id of this process.
  def self.executable_real?(path)
    st = Stat.stat? path
    st ? st.executable_real? : false
  end

  ##
  # Return true if the named file exists.
  def self.exist?(path)
    Stat.stat?(path) ? true : false
  rescue SystemCallError
    false
  end

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
  def self.expand_path(path, dir_string = nil)
    Platform::File.expand_path(path, dir_string)
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

  ##
  # Returns true if the named file exists and is a regular file.
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

  ##
  # Returns true if path matches against pattern The pattern
  # is not a regular expression; instead it follows rules
  # similar to shell filename globbing. It may contain the
  # following metacharacters:
  # 
  # *:	Matches any file. Can be restricted by other values in the glob. * will match all files; c* will match all files beginning with c; *c will match all files ending with c; and c will match all files that have c in them (including at the beginning or end). Equivalent to / .* /x in regexp.
  # **:	Matches directories recursively or files expansively.
  # ?:	Matches any one character. Equivalent to /.{1}/ in regexp.
  # [set]:	Matches any one character in set. Behaves exactly like character sets in Regexp, including set negation ([^a-z]).
  # <code></code>:	Escapes the next metacharacter.
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
    pattern = StringValue(pattern).dup
    path    = StringValue(path).dup
    flags   = Type.coerce_to(flags, Fixnum, :to_int) unless Fixnum === flags

    name_match(pattern, path, flags, 0, pattern.size, 0, path.size)
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
    st_o = stat(StringValue(orig))
    st_c = stat(StringValue(copy))

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

  ##
  # Creates a new name for an existing file using a hard link.
  # Will not overwrite new_name if it already exists (raising
  # a subclass of SystemCallError). Not available on all platforms.
  #
  #  File.link("testfile", ".testfile")   #=> 0
  #  IO.readlines(".testfile")[0]         #=> "This is line one\n"
  def self.link(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.link(from, to)
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
    Stat.new path, false
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
    st = Stat.stat? path
    st ? st.pipe? : false
  end

  ## 
  # Returns true if the named file is readable by the effective
  # user id of this process.
  def self.readable?(path)
    st = Stat.stat? path
    st ? st.readable? : false
  end

  ##
  # Returns true if the named file is readable by the real user
  # id of this process.
  def self.readable_real?(path)
    st = Stat.stat? path
    st ? st.readable_real? : false
  end

  ##
  # Returns the name of the file referenced by the given link.
  # Not available on all platforms.
  #
  #  File.symlink("testfile", "link2test")   #=> 0
  #  File.readlink("link2test")              #=> "testfile"
  def self.readlink(path)
    StringValue(path)

    buf = " " * 1024

    n = POSIX.readlink(path, buf, buf.length)
    Errno.handle if n == -1

    buf[0, n]
  end

  ##
  # Renames the given file to the new name. Raises a SystemCallError
  # if the file cannot be renamed.
  #
  #  File.rename("afile", "afile.bak")   #=> 0
  def self.rename(from, to)
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.rename(from, to)
    Errno.handle if n == -1
    n
  end
  
  ##
  # Returns the size of file_name.
  def self.size(path)
    stat(path).size
  end

  ##
  # Returns nil if file_name doesn‘t exist or has zero size,
  # the size of the file otherwise.
  def self.size?(path_or_file)
    s = 0

    if path_or_file.is_a? File
      s = Stat.from_fd(path_or_file.fileno).size
    else
      st = Stat.stat? path_or_file
      s = st.size if st
    end

    s > 0 ? s : nil
  end

  ##
  # Returns true if the named file is a socket.
  def self.socket?(path)
    st = Stat.stat? path
    st ? st.socket? : false
  end

  ##
  # Splits the given string into a directory and a file component and returns them in a two-element array. See also File::dirname and File::basename.
  # 
  #  File.split("/home/gumby/.profile")   #=> ["/home/gumby", ".profile"]
  def self.split(path)
    p = StringValue(path)
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
    to = StringValue(to)
    from = StringValue(from)

    n = POSIX.symlink(from, to)
    Errno.handle if n == -1
    n
  end

  ##
  # Returns true if the named file is a symbolic link.
  def self.symlink?(path)
    st = Stat.stat? path, false
    st ? st.symlink? : false
  end
  
  ##
  # Copies a file from to to. If to is a directory, copies from to to/from.
  def self.syscopy(from, to)
    out = File.directory?(to) ? to + File.basename(from) : to
    
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
    mask = mask.to_int if mask
    if mask
      POSIX.umask(mask)
    else
      old_mask = POSIX.umask(0)
      POSIX.umask(old_mask)
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
      path = StringValue(path)

      n = POSIX.unlink(path)
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
    ptr = FFI::MemoryPointer.new(POSIX::TimeVal, 2)
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

  ##
  # Returns true if the named file is writable by the effective 
  # user id of this process.
  def self.writable?(path)
    st = Stat.stat? path
    st ? st.writable? : false
  end

  ##
  # Returns true if the named file is writable by the real user
  # id of this process.
  def self.writable_real?(path)
    st = Stat.stat? path
    st ? st.writable_real? : false
  end

  ##
  # Returns true if the named file exists and has a zero size.
  def self.zero?(path)
    st = Stat.stat? path
    st ? st.zero? : false
  end
  
  ##
  # Returns true if the named file exists and the effective
  # used id of the calling process is the owner of the file.
  #  File.owned?(file_name)   => true or false
  def self.owned?(file_name)
    Stat.new(file_name).owned?
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
  end
  
  ##
  # Returns true if the named file exists and the effective
  # used id of the calling process is the owner of the file.
  def self.owned?(file_name)
    Stat.new(file_name).owned?
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
    rewind
    super(other, mode)
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
    @stat[:st_mode] & S_ISGID != 0
  end
  
  def setuid?
    @stat[:st_mode] & S_ISUID != 0
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
