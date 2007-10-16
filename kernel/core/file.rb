# depends on: io.rb

class File < IO
  module Constants
    # TODO: "OK" constants aren't in File::Constants in MRI
    F_OK = 0 # test for existence of file
    X_OK = 1 # test for execute or search permission
    W_OK = 2 # test for write permission
    R_OK = 4 # test for read permission

    # TODO: these flags should probably be imported from Platform
    FNM_NOESCAPE = 0x01;
    FNM_PATHNAME = 0x02;
    FNM_DOTMATCH = 0x04;
    FNM_CASEFOLD = 0x08;

    LOCK_SH  = 0x01
    LOCK_EX  = 0x02
    LOCK_NB  = 0x04
    LOCK_UN  = 0x08

    SEEK_SET = 0x00
    SEEK_CUR = 0x01
    SEEK_END = 0x02
    
    RDONLY   = 0x00
    WRONLY   = 0x01
    RDRW     = 0x02
    
    CREAT    = 0x40
    EXCL     = 0x80
    NOCTTY   = 0x100
    TRUNC    = 0x200
    APPEND   = 0x400
    NONBLOCK = 0x800
    SYNC     = 0x1000
  end
  include Constants

  # TODO: remove these when 'include Constants' works
  FNM_NOESCAPE = 0x01;
  FNM_PATHNAME = 0x02;
  FNM_DOTMATCH = 0x04;
  FNM_CASEFOLD = 0x08;
    
  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end

  if Rubinius::OS == :win32
    SEPARATOR = "\\"
  else
    SEPARATOR = "/"
  end

  ALT_SEPARATOR = nil
  
  def self.new(path, mode)
    return open_with_mode(path, mode)
  end
    
  def self.open(path, mode="r")
    raise Errno::ENOENT if mode == "r" and not exists?(path)
    
    f = open_with_mode(path, mode)
    return f unless block_given?

    begin
      yield f
    ensure
      f.close unless f.closed?
    end
  end
  
  def self.exist?(path)
    out = Stat.stat(StringValue(path), true)
    if out.kind_of? Stat
      return true
    else
      return false
    end
  end

  def self.file?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :file
  end

  def self.directory?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :dir
  end
  
  def self.symlink?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :link
  end

  def self.blockdev?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :block
  end

  def self.chardev?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :char
  end
  
  def self.fifo?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :fifo
  end
  
  def self.socket?(path)
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.kind == :socket
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
    st = Stat.stat(StringValue(path), true)
    return false unless st.kind_of? Stat
    st.size == 0
  end

  def self.size(path)
    stat(path).size
  end

  def self.size?(path)
    st = Stat.stat(StringValue(path), true)
    return nil unless st.kind_of? Stat
    st.size
  end
  
  def self.writable_real?(path)
    Platform::POSIX.access(StringValue(path), Constants::W_OK) == 0
  end

  def self.executable_real?(path)
    Platform::POSIX.access(StringValue(path), Constants::X_OK) == 0
  end

  def self.readable_real?(path)
    Platform::POSIX.access(StringValue(path), Constants::R_OK) == 0
  end
  
  def self.unlink(*paths)
    paths.each do |path|
      path = StringValue(path)
      raise Errno::ENOENT, "No such file or directory - #{path}" unless exists?(path)
      Platform::POSIX.unlink(path) 
    end
    paths.size
  end
  
  def self.link(from, to)
    to = StringValue(to)
    from = StringValue(from)
    raise Errno::EEXIST if exists?(to)
    Platform::POSIX.link(from, to)
  end

  def self.readlink(path)
    StringValue(path)

    buf = " " * 1024
    n = Platform::POSIX.readlink(path, buf, buf.length)
    buf[0, n]
  end
  
  def self.identical?(orig, copy)
    st_o = stat(StringValue(orig))
    st_c = stat(StringValue(copy))
    
    return false unless st_o.kind == st_c.kind
    return false unless st_o.inode == st_c.inode
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
    only_dots = filename.match(/[^\.]/) == nil

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
  
  def self.atime(path)
    Time.at stat(path).atime
  end

  def self.mtime(path)
    Time.at stat(path).mtime
  end

  def self.ctime(path)
    Time.at stat(path).ctime
  end

  # TODO - needs work for Win32
  def self.dirname(path)
    path = StringValue(path)

    slash = -1
    nonslash = 0
    before_slash = 0
    0.upto(path.length-1) do |i|
      if path[i].chr == "/" 
        slash = i
        before_slash = nonslash + 1
      else
        nonslash = i
      end
    end
    return "/" if slash > 0  and nonslash == 0
    return "." if slash == -1
    return path.slice(0, before_slash)
  end
  
  # TODO - needs work for Win32
  def self.basename(path, ext="")
    path = StringValue(path)
    ext = StringValue(ext)

    slash = -1
    0.upto(path.length-1) do |i|
      if path[i].chr == "/"
        slash = i
      end
    end
    if slash == -1
      slash = 0 
    else 
      slash += 1 unless path.length == 1
    end
    bn = path.slice(slash, path.length-slash)
    if (ext.size > 0)
      x = bn.length
      y = ext.length
      # TODO the to_sym works around a mysterious bug
      if (x > y && bn.slice(x-y, y).to_sym == ext.to_sym) 
        bn = bn.slice(0, x-y) 
      end
    end
    return bn 
  end
  
  def self.expand_path(path)
    path = path.to_s.strip
    dirs = path.split('/')
    if path == '' || (dirs.empty? && path[0].chr != '/')
      Dir.pwd
    else
      first = case dirs.first
      when '..' : Dir.pwd.split('/')[0...-1].join('/')
      when '~' : ENV['HOME']
      when '.' : Dir.pwd
      when '' : '/'
      else
        if dirs.first.nil?
          match = /(\/+)/.match(path) 
          prefix = match[0] if match
          ''
        else
          Dir.pwd + '/' + dirs.first
        end
      end
      dirs.shift
      paths = first.split('/')
      dirs.each do |dir|
        next if dir == '.' || dir == ''
        dir == '..' ? paths.pop : paths.push(dir)
      end
      string = paths.empty? ? '' : paths.join("/")
      !string.empty? && string[0].chr == '/' ? string : prefix || '/' +string
    end
  end
  
  def self.fnmatch(pattern, path, flags=0)
    pattern = StringValue(pattern).dup
    path = StringValue(path).dup
    escape = (flags & FNM_NOESCAPE) == 0
    pathname = (flags & FNM_PATHNAME) != 0
    nocase = (flags & FNM_CASEFOLD) != 0
    period = (flags & FNM_DOTMATCH) == 0
    subs = { /\*{1,2}/ => '(.*)', /\?/ => '(.)', /\{/ => '\{', /\}/ => '\}' }
    
    return false if path[0] == ?. and pattern[0] != ?. and period
    pattern.gsub!('.', '\.')
    pattern = pattern.split(/(?<b>\[(?:\\[\[\]]|[^\[\]]|\g<b>)*\])/).collect do |part|
      if part[0] == ?[
        part.gsub!(/\\([*?])/, '\1')
        part.gsub(/\[!/, '[^')
      else
        subs.each { |p,s| part.gsub!(p, s) }
        if escape
          part.gsub(/\\(.)/, '\1')
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

  def self.join(*args)
    args.map { |arg| arg.to_str }.join(SEPARATOR)
  end
  
  class << self
    alias_method :delete, :unlink
    alias_method :exists?, :exist?
    alias_method :fnmatch?, :fnmatch
  end
  
  class Stat
    self.instance_fields = 11
    ivar_as_index :inode => 0, :mode => 1, :kind => 2, :owner => 3, :group => 4,
      :size => 5, :block => 6, :atime => 7, :mtime => 8, :ctime => 9, :path => 10
      
    def inode; @inode; end
    def mode; @mode; end
    def kind; @kind; end
    def owner; @owner; end
    def group; @group; end
    def size; @size; end
    def block; @block; end
    def atime; @atime; end
    def mtime; @mtime; end
    def ctime; @ctime; end
    def path; @path; end
   
    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} path=#{@path} kind=#{@kind}>"
    end

    def symlink?
      @kind == :link
    end
  end
  
  def self.stat(path)
    perform_stat(path, true)
  end
  
  def self.lstat(path)
    perform_stat(path, false)
  end
  
  def self.perform_stat(path, follow_links)
    out = Stat.stat(StringValue(path), follow_links)
    if !out
      raise UnableToStat, "Unable to perform stat on '#{path}'"
    elsif out == 1
      raise Errno::ENOENT, "No such file or directory - #{path}"
    elsif out == 2
      raise Errno::EACCES, "Unable to access '#{path}'"
    else
      return out
    end
  end
  
  def self.to_sexp(name, newlines=true)
    out = to_sexp_full(name, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.import_position out.at(1), out.at(2)
      raise exc
    end
    return out
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
end
