# depends on: array.rb class.rb fixnum.rb

class Dir
  include Enumerable

  GLOB_VERBOSE = 1 << (4 * 8 - 1) # HACK
  GLOB_RECURSIVE = -2

  SeekKind = 0
  RewindKind = 1
  TellKind = 2

  def self.[](pattern)
    glob(pattern, 0)
  end

  def self.chdir(path = ENV['HOME'])
    if block_given?
      original_path = self.getwd
      error = Platform::POSIX.chdir path
      Errno.handle(path) if error != 0

      begin
        value = yield path
      ensure
        error = Platform::POSIX.chdir original_path
        Errno.handle(original_path) if error != 0
      end

      return value
    else
      error = Platform::POSIX.chdir path
      if error != 0
        Errno.handle path
      end
      error
    end
  end

  def self.entries(path)
    ret = []

    self.open(path) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end

  def self.find_dirsep(pattern, flags)
    escape = (flags & File::FNM_NOESCAPE) == 0
    open = false

    chars = pattern.split ''

    chars.each_with_index do |char, i|
      case char
      when '[' then
        open = true
      when ']' then
        open = false
      when '/' then
        return i unless open
      when '\\' then
        return i if escape and (i + 1 == chars.length)
      end
    end

    chars.length
  end

  def self.foreach(path)
    self.open(path) do |dir|
      while s = dir.read
        yield s
      end
    end

    nil
  end

  def self.getwd
    buf = " " * 1024
    Platform::POSIX.getcwd(buf, buf.length)
  end

  ##
  # call-seq:
  #    Dir.glob( pattern, [flags] ) => array
  #    Dir.glob( pattern, [flags] ) {| filename | block }  => nil
  #
  # Returns the filenames found by expanding <i>pattern</i> which is an
  # +Array+ of the patterns or the pattern +String+, either as an +array+ or
  # as parameters to the block. Note that this pattern is not a regexp (it's
  # closer to a shell glob). See File::fnmatch for the meaning of the +flags+
  # parameter. Note that case sensitivity depends on your system (so
  # <code>File::FNM_CASEFOLD</code> is ignored)
  #
  # <code>*</code>::        Matches any file. Can be restricted by
  #                         other values in the glob. <code>*</code>
  #                         will match all files; <code>c*</code> will
  #                         match all files beginning with
  #                         <code>c</code>; <code>*c</code> will match
  #                         all files ending with <code>c</code>; and
  #                         <code>*c*</code> will match all files that
  #                         have <code>c</code> in them (including at
  #                         the beginning or end). Equivalent to
  #                         <code>/ .* /x</code> in regexp.
  # <code>**</code>::       Matches directories recursively.
  # <code>?</code>::        Matches any one character. Equivalent to
  #                         <code>/.{1}/</code> in regexp.
  # <code>[set]</code>::    Matches any one character in +set+.
  #                         Behaves exactly like character sets in
  #                         Regexp, including set negation
  #                         (<code>[^a-z]</code>).
  # <code>{p,q}</code>::    Matches either literal <code>p</code> or
  #                         literal <code>q</code>. Matching literals
  #                         may be more than one character in length.
  #                         More than two literals may be specified.
  #                         Equivalent to pattern alternation in
  #                         regexp.
  # <code>\</code>::        Escapes the next metacharacter.
  #
  #   Dir["config.?"]                     #=> ["config.h"]
  #   Dir.glob("config.?")                #=> ["config.h"]
  #   Dir.glob("*.[a-z][a-z]")            #=> ["main.rb"]
  #   Dir.glob("*.[^r]*")                 #=> ["config.h"]
  #   Dir.glob("*.{rb,h}")                #=> ["main.rb", "config.h"]
  #   Dir.glob("*")                       #=> ["config.h", "main.rb"]
  #   Dir.glob("*", File::FNM_DOTMATCH)   #=> [".", "..", "config.h", "main.rb"]
  #   
  #   rbfiles = File.join("**", "*.rb")
  #   Dir.glob(rbfiles)                   #=> ["main.rb",
  #                                            "lib/song.rb",
  #                                            "lib/song/karaoke.rb"]
  #   
  #   libdirs = File.join("**", "lib")
  #   Dir.glob(libdirs)                   #=> ["lib"]
  #
  #   
  #   librbfiles = File.join("**", "lib", "**", "*.rb")
  #   Dir.glob(librbfiles)                #=> ["lib/song.rb",
  #                                            "lib/song/karaoke.rb"]
  #   
  #   librbfiles = File.join("**", "lib", "*.rb")
  #   Dir.glob(librbfiles)                #=> ["lib/song.rb"]

  def self.glob(pattern, flags = 0)
    matches = []
    pattern = StringValue(pattern)

    glob_brace_expand pattern, flags & ~GLOB_VERBOSE, matches

    matches
  end

  ##
  # Adds matches for +pattern+ to +matches+.  Called from glob_brace_expand.

  def self.glob0(pattern, flags, matches)
    root = pattern.dup
    path = ''

    if root[0] == ?/ then # HACK windows
      root = root[1..-1]
      path = '/'
    end

    pattern = glob_pattern root, flags

    glob_helper path, false, :unknown, :unknown, [pattern], flags, matches
  end

  ##
  # Expands {} alternation in +pattern+ and searches for matching files with
  # each expanded pattern, collecting them into +matches+.

  def self.glob_brace_expand(pattern, flags, matches)
    escape = (flags & File::FNM_NOESCAPE) == 0

    rbrace = nil
    lbrace = nil
    nest = 0

    chars = pattern.split ''
    skip = false

    chars.each_with_index do |char, i|
      if skip then
        skip = false
        next
      end

      if char == '{' and nest == 0 then
        lbrace = i
        nest += 1
      end

      if char == '}' and nest - 1 <= 0 then
        rbrace = i
        nest -= 1
      end

      skip = true if char == '\\' and escape
    end

    if lbrace and rbrace then
      pos = lbrace
      front = pattern[0...lbrace]
      back = pattern[(rbrace + 1)..-1]

      while pos < rbrace do
        nest = 0
        pos += 1
        last = pos

        while pos < rbrace and not (chars[pos] == ',' and nest == 0) do
          nest += 1 if chars[pos] == '{'
          nest -= 1 if chars[pos] == '}'

          if chars[pos] == '\\' and escape then
            pos += 1
            break if pos == rbrace
          end

          pos += 1
        end

        brace_pattern = "#{front}#{pattern[last...pos]}#{back}"

        glob_brace_expand brace_pattern, flags, matches
      end
    else
      glob0 pattern, flags, matches
    end
  end

  ##
  # glob_helper performs recursive matching on +path+ with +patterns+ and
  # collects matches found into +matches+.
  #--
  # +dirsep+:: Should '/' be placed before appending child's entry name to
  #            +path+?
  # +exist+:: Does 'path' indicate an existing entry?
  # +isdir+:: Does 'path' indicate a directory or a symlink to a directory?
  # +patterns+:: An Array of Arrays of pattern tuples
  # +flags+:: File.fnmatch flags

  def self.glob_helper(path, dirsep, exist, isdir, patterns, flags, matches)
    status = nil
    plain = magic = recursive = match_all = match_dir = false
    escape = (flags & File::FNM_NOESCAPE) == 0

    last_type = nil

    # sanity check
    patterns.each do |pattern|
      type = pattern.first.last

      if type == :recursive then
        recursive = true
        type = pattern[1].last
      end

      case type
      when :magic then
        magic = true
      when :match_all then
        match_all = true
      when :match_dir then
        match_dir = true
      when :plain then
        plain = true
      when :recursive then
        raise "continuous RECURSIVEs"
      end
    end

    # add entry to matches
    unless path.empty? then
      if match_all and exist == :unknown then
        if stat = File.stat(path) rescue nil then
          exist = :yes
          isdir = if stat.directory? then
                    :yes
                  elsif stat.symlink? then
                    :unknown
                  else
                    :no
                  end
        else
          exist = isdir = :no
        end
      end

      if match_dir and isdir == :unknown then
        if stat = File.stat(path) rescue nil then
          exist = :yes
          isdir = stat.directory? ? :yes : :no
        else
          exist = isdir = :no
        end
      end

      matches << path if match_all and exist == :yes

      if match_dir and isdir == :yes then
        return if path.empty? and not dirsep
        tmp = join_path path, '', dirsep
        matches << tmp
      end
    end

    return if exist == :no or isdir == :no

    if magic or recursive then
      dir = path.empty? ? '.' : path

      return unless File.directory? dir

      begin
        Dir.foreach dir do |entry|
          buf = join_path path, entry, dirsep
          new_isdir = :unknown
          new_patterns = []

          if recursive and not %w[. ..].include?(entry) and
            File.fnmatch('*', entry, flags) then
            stat = File.stat buf
            new_isdir = if stat.directory? then
                          :yes
                        elsif stat.symlink? then
                          :unknown
                        else
                          :no
                        end
          end

          # build up a new patterns Array
          patterns.each do |pattern|
            pattern.each_with_index do |(match_str, type), i|
              if type == :recursive then
                # add recursive pattern if real directory
                new_patterns << pattern[i..-1] if new_isdir == :yes
                i += 1
                part = pattern[i]
              end

              if (type == :plain or type == :magic) and
                 File.fnmatch match_str, entry, flags then
                new_patterns << pattern[(i + 1)..-1]
              end
            end
          end

          glob_helper buf, true, :yes, new_isdir, new_patterns, flags, matches
        end
      rescue Errno::ENOTDIR
        # File.directory? may return true on entries in an fdesc file system
        return
      end
    elsif plain then
      plain_patterns = patterns.select do |pattern|
        pattern.first[1] == :plain
      end

      # build a list of plain patterns that all start with the same string
      plain_patterns.each_with_index do |pattern, i|
        next if pattern.nil?

        pattern.each_with_index do |part, j|
          new_patterns = []
          next_offset = j + 1

          name = part[0]
          name = name.gsub '\\', '' if escape

          new_patterns << pattern[next_offset..-1]

          same_patterns = plain_patterns[(i + 1)..-1].select do |pattern2|
            File.fnmatch pattern2.first.first, name, flags
          end

          same_patterns = same_patterns.map do |pattern2|
            pattern2[1..-1]
          end

          new_patterns.push same_patterns

          new_patterns.delete_if { |pattern| pattern.nil? or pattern.empty? }

          new_path = join_path path, name, dirsep

          glob_helper(new_path, true, :unknown, :unknown, new_patterns, flags,
                      matches)
        end
      end
    end
  end

  ##
  # A glob pattern is magic if it contains the magic characters *, ? or [], or
  # if the File::FNM_CASEFOLD flag is set.

  def self.glob_magic?(pattern, flags)
    escape = (flags & File::FNM_NOESCAPE) == 0
    nocase = (flags & File::FNM_CASEFOLD) == 0

    chars = pattern.split ''

    chars.each_with_index do |char, i|
      case char
      when '*', '?', '[' then
        return true
      when '\\' then
        return false if escape && i == chars.length
      else
       return true if char == /[a-z]/i && nocase # HACK FNM_SYSCASE
      end
    end

    false
  end

  ##
  # Builds a glob pattern from pattern string +pattern+.
  #
  # A glob pattern is an Array of pairs containing a match string and a type.
  # The type can be one of:
  # +:recursive+:: '**'
  # +:magic+:: contains '*', etc., see #glob_magic?
  # +:plain+:: plain string
  # +:match_dir+:: +pattern+ ends in '/'
  # +:match_all+:: +pattern+ does not end in '/'

  def self.glob_pattern(pattern, flags)
    dirsep = false
    glob_pattern = []

    until pattern.empty? do
      part = []

      if pattern =~ /\A\*\*\// then
        # fold continuous :recursive for glob_helper
        pattern.sub!(/\A(\*\*\/)+/, '')
        part[0] = ''
        part[1] = :recursive
        dirsep = true
      else
        m = find_dirsep pattern, flags

        part[0] = pattern[0...m]
        part[1] = glob_magic?(part[0], flags) ? :magic : :plain

        unless pattern.length == m then
          dirsep = true
          pattern = pattern[(m+1), pattern.size-m]
        else
          dirsep = false
          pattern = ''
        end
      end

      glob_pattern << part
    end

    part = []
    part[0] = ''
    part[1] = dirsep ? :match_dir : :match_all

    glob_pattern << part

    glob_pattern
  end

  ##
  # Joins p1 with p2 and adds a directory separator if +dirsep+ is true.
  # Used by Dir::glob.

  def self.join_path(p1, p2, dirsep)
    "#{p1}#{dirsep ? '/' : ''}#{p2}"
  end

  def self.mkdir(path, mode = 0777)
    error = Platform::POSIX.mkdir(path, mode)
    if error != 0
      Errno.handle path
    end
    error
  end

  def self.open(path)
    dir = self.new(path)
    if block_given?
      begin
        value = yield dir
      ensure
        dir.close
      end

      return value
    else
      return dir
    end
  end

  def self.prim_close(dir)
    Ruby.primitive :dir_close
    raise PrimitiveFailure, "primitive failed"
  end

  def self.prim_control(dir, kind, pos)
    Ruby.primitive :dir_control
    raise PrimitiveFailure, "primitive failed"
  end

  def self.prim_open(path)
    Ruby.primitive :dir_open
  end

  def self.prim_read(dir)
    Ruby.primitive :dir_read
    raise PrimitiveFailure, "primitive failed"
  end

  def self.rmdir(path)
    error = Platform::POSIX.rmdir(path)
    if error != 0
      Errno.handle path
    end
    error
  end

  def initialize(path)
    @dirptr = Dir.prim_open(path)

    if @dirptr.nil?
      Errno.handle path
    end

    @path = path
  end

  def close
    raise IOError, "closed directory" if @dirptr.nil?

    Dir.prim_close(@dirptr)

    @dirptr = nil
  end

  def each
    raise IOError, "closed directory" if @dirptr.nil?

    while s = read
      yield s
    end

    self
  end

  def path
    raise IOError, "closed directory" if @dirptr.nil?

    @path
  end

  def pos
    raise IOError, "closed directory" if @dirptr.nil?

    Dir.prim_control(@dirptr, TellKind, 0)
  end

  alias_method :tell, :pos

  def pos=(position)
    raise IOError, "closed directory" if @dirptr.nil?

    seek(position)

    position
  end

  def read
    raise IOError, "closed directory" if @dirptr.nil?

    return Dir.prim_read(@dirptr)
  end

  def rewind
    raise IOError, "closed directory" if @dirptr.nil?

    Dir.prim_control(@dirptr, RewindKind, 0);

    self
  end

  def seek(position)
    raise IOError, "closed directory" if @dirptr.nil?

    Dir.prim_control(@dirptr, SeekKind, position)

    self
  end

  class << self
    alias_method :pwd, :getwd
    alias_method :delete, :rmdir
    alias_method :unlink, :rmdir
  end

end

