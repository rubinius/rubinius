require File.dirname(__FILE__) + '/../spec_helper' 

# class methods
# atime, basename, blockdev?, chardev?, chmod, chown, ctime, delete,
# directory?, dirname, executable?, executable_real?, exist?,
# exists?, expand_path, extname, file?, fnmatch, fnmatch?, ftype,
# grpowned?, identical?, join, lchmod, lchown, link, lstat, mtime,
# new, owned?, pipe?, readable?, readable_real?, readlink, rename,
# setgid?, setuid?, size, size?, socket?, split, stat, sticky?,
# symlink, symlink?, truncate, umask, unlink, utime, writable?,
# writable_real?, zero?

# atime, chmod, chown, ctime, flock, lstat, mtime, path, truncate

WINDOWS =  RUBY_PLATFORM.match('mswin')

context "File class method" do
  def non_existent_file
    file = 'i_dont_exist'
    File.delete(file) rescue nil
    file
  end
  
  specify "dirname should return all the components of filename except the last one" do
    File.dirname('/home/jason').should == '/home'
    File.dirname('/home/jason/poot.txt').should == '/home/jason'
    File.dirname('poot.txt').should == '.'
    File.dirname('/holy///schnikies//w00t.bin').should == '/holy///schnikies'
    File.dirname('').should == '.'
    File.dirname('/').should == '/'
    File.dirname('/////').should == '/'
  end

  specify "basename should return the last component of the filename" do
    File.basename('a').should == 'a'
    File.basename('/a').should == 'a'
    File.basename('/a/b').should == 'b'
    File.basename('/ab/ba/bag').should == 'bag'
    File.basename('/ab/ba/bag.txt').should == 'bag.txt'
    File.basename('/').should == '/'
    File.basename('/foo/bar/baz.rb', '.rb').should == 'baz'
    File.basename('baz.rb', 'z.rb').should == 'ba'
  end
  
  specify "blockdev? should return true/false depending if the named file is a block device" do
    File.blockdev?("/tmp").should == false
  end
  
  specify "chardev? should return true/false depending if the named file is a char device" do
    File.chardev?("/tmp")
  end
  
  specify "zero? should return true if the named file exists and has a zero size." do
    begin
      File.zero?('fake_file').should == false
      file = '/tmp/i_exist'
      File.open(file,'w'){
        File.zero?(file).should == true
      }
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "writable_real? should return true if named file is writable by the real user id of the process, otherwise false" do
    begin
      File.writable_real?('fake_file').should == false
      file = '/tmp/i_exist'
      File.open(file,'w'){
        File.writable_real?(file).should == true
      }
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "writable? should return true if named file is writable by the effective user id of the process, otherwise false" do
    begin
      File.writable?('fake_file').should == false
      File.writable?('/etc/passwd').should == false
      file = '/tmp/i_exist'
      File.open(file,'w'){
        File.writable?(file).should == true
      }
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "delete should delete a file and return the number of names passed as arguments" do
    begin
      file = non_existent_file
      File.exists?(file).should == false
      File.new(file,'w').close
      File.exists?(file).should == true
      File.delete(file).should == 1
      File.exists?(file).should == false
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "unlink should delete a file and return the number of names passed as arguments" do
    begin
      file = non_existent_file
      File.exists?(file).should == false
      File.new(file,'w').close
      File.exists?(file).should == true
      File.unlink(file).should == 1
      File.exists?(file).should == false 
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "umask should return the current umask value for the process" do
    File.umask(0006).should == 18
    File.umask.should == 6
  end
  
  specify "split should split the given string into a directory and a file component and returns them in a 2 element array" do
    File.split("/rubinius/better/than/ruby").should == ["/rubinius/better/than", "ruby"]
  end
  
  specify "size? should return nil if file_name doesn't exist or has zero size, the size of the file otherwise" do
    begin
      File.size?(non_existent_file).should == nil
      file = '/tmp/i_exist'
      File.open(file,'w'){|f| f.write 'rubinius'}
      File.size?(file).should == 8
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "size should the size of the file" do
    begin
      file = '/tmp/i_exist'
      File.open(file,'w'){|f| f.write 'rubinius'}
      File.size?(file).should == 8
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "ftype should return the type of the named file" do
    begin
      file = 'testfile'
      File.new(file,'w').close
      File.ftype(file).should == "file"
      File.ftype("/dev/tty").should == "characterSpecial"
      File.ftype("/").should == "directory"
    ensure
      File.delete(file) rescue nil
    end
  end
  
  specify "identical? should return true if both named files are identical" do
    begin
      file = '/tmp/i_exist'
      file2 = '/tmp/i_exist_too'
      File.open(file,'w'){|f| f.write 'rubinius'}
      File.open(file2,'w'){|f| f.write 'ruby'}
      File.identical?(file,file).should == true
      File.identical?(file,file2).should == false
    ensure
      File.delete(file) rescue nil
      File.delete(file2) rescue nil
    end
  end
  
  specify "join should return new string formed by joining the strings using File::SEPARATOR" do
    File.join("smalltalk","ruby","rubinius").should == "smalltalk/ruby/rubinius"
    File.join.should == ""

    # arguments must respond to to_str
    should_raise(TypeError) do
      File.join(Object.new)
    end
  end
end

context "File instance method" do  
  specify "path should return the pathname used to create file as a string" do
    begin
      file1 = "testfile"
      file2 = "/tmp/../tmp/xxx"
      File.open(file1,'w'){|file| file.path.should == "testfile"}
      File.open(file2, 'w'){|file| file.path.should == "/tmp/../tmp/xxx"}
    ensure
      File.delete(file1) rescue nil
      File.delete(file2) rescue nil
    end
  end
end

describe "File#atime" do
  before(:each) do
    @name = File.expand_path(__FILE__)
    @file = File.open(@name)
  end

  it "returns the last access time to self" do
    @file.atime  
    @file.atime.class.should === Time
  end

  it "raise an Exception if it has the worng number of argments" do
    should_raise(ArgumentError){ @file.atime(@name) }
  end

  after(:each) do 
    @name = nil
    @file = nil
  end
end
 
describe "File#ctime" do
  before(:each) do
    @file = File.open(__FILE__)
  end

  it "Returns the change time for the named file (the time at which directory information about the file was changed, not the file itself)." do 
    @file.ctime
    @file.ctime.class.should == Time
  end
 
  #  it "" do
  #    sys("touch  #@file")
  #    ctime = RubiconStat::ctime(@file)
  #    @cTime = Time.at(ctime)
  #    @cTime.should == File.ctime(@file)
  #  end
  
  it  "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ @file.ctime(@file, @file) } 
  end

  after(:each) do
    @file = nil
  end
end 

describe "File.ctime" do
  before(:each) do
    @file = __FILE__
  end

  it "Returns the change time for the named file (the time at which directory information about the file was changed, not the file itself)." do 
    File.ctime(@file)  
    File.ctime(@file).class.should == Time
  end

  it "raise an Errno::ENOENT exception if the file is not found" do    
    should_raise(Errno::ENOENT){ File.ctime('bogus') }
  end
  
  it  "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.ctime }
    should_raise(ArgumentError){ File.ctime(@file, @file) }
    should_raise(TypeError){ File.ctime(1) }
  end

  after(:each) do
    @file = nil
  end
end 

describe "File.delete" do
  before(:each) do
    @file1 = 'temp1.txt'
    @file2 = 'temp2.txt'

    File.open(@file1, "w"){} # touch # Touch
    File.open(@file2, "w"){} # Touch
  end

  it "deletes the named files," do
    File.delete(@file1)
    should_raise(Errno::ENOENT){File.open(@file1,"r")}
  end

  it "return the number of names passed as arguments (0 and 1 argument)" do
    File.delete.should == 0
    File.delete(@file1).should == 1
  end

  it "return the number of names passed as arguments(multiples arguments)" do
    File.delete(@file1, @file2).should == 2
  end

  it "raise an exception its the arguments are the worng type or number" do
    should_raise(TypeError){ File.delete(1) }
    should_raise(Errno::ENOENT){ File.delete('a_fake_file') }
  end
 
  after(:each) do
    # FileUtils.remove_file(@file1)
    # FileUtils.remove_file(@file2)

    @file1 = nil
    @file2 = nil
  end
end

describe "File.directory?" do 
  before(:each) do
    if WINDOWS
      @dir  = "C:\\"
      @file = "C:\\winnt\\notepad.exe"
    else
      @dir  = "/"
      @file = "/bin/ls"
    end
  end
 
  it "return true if dir is a directory, otherwise return false" do
    File.directory?(@dir).should == true
    File.directory?(@file).should == false
  end
   
  it "raise an exception its the arguments are the worng type or number" do
    should_raise(ArgumentError){ File.directory? }
    should_raise(ArgumentError){ File.directory?(@dir, @file) }
    should_raise(TypeError){ File.directory?(nil) }
  end

  after(:each) do
    @dir = nil
  end
end

describe "File.executable?" do 
  before(:each) do
    @file1 = File.join(Dir.pwd, 'temp1.txt')
    @file2 = File.join(Dir.pwd, 'temp2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
    
    File.chmod(0755, @file1)
  end

  unless WINDOWS
    it "return true if the argument its an executable file" do
      File.executable?(@file1).should == true
      File.executable?(@file2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raise an exception its the arguments are the worng type or number" do
    should_raise(ArgumentError){ File.executable? }
    should_raise(TypeError){ File.executable?(1) }
    should_raise(TypeError){ File.executable?(nil) }
    should_raise(TypeError){ File.executable?(false) }
  end

  after(:each) do
    # FileUtils.remove_file(@file1)
    # FileUtils.remove_file(@file2)

    @file1 =  nil
    @file2 = nil
  end
end

describe "File.executable_real?" do
  def setup
    @file1 = File.join(Dir.pwd, 'temp1.txt')
    @file2 = File.join(Dir.pwd, 'temp2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
      
    File.chmod(0755, @file1)
  end
 
  unless WINDOWS
    it "returns true if the file its an executable" do 
      File.executable_real?(@file1).should == true
      File.executable_real?(@file2).should == false
      File.executable_real?('bogus').should == false
    end
  end
  
  it "raise an exception if the argumnent is not from the correct type or are missing" do
    should_raise(ArgumentError){ File.executable_real? }
    should_raise(TypeError){ File.executable_real?(1) }
    should_raise(TypeError){ File.executable_real?(nil) }
    should_raise(TypeError){ File.executable_real?(false) }
  end

  after(:each) do
    #FileUtils.remove_file(@file1)
    #FileUtils.remove_file(@file2)

    @file1 = nil
    @file2 = nil
  end
end
   

#describe "File::Constants" do 
#  before(:each) do
#    
#  end
#   
#  after(:each) do
#    
#  end  
#end

describe "File.exist?" do 
  before(:each) do
    @file = 'temp.txt'
    File.open(@file, "w"){}
  end 
  
  it "return true if the file exist" do
    File.exist?(@file).should == true
    File.exist?('a_fake_file').should == false
  end

  it "return true if the file exist using the alias exists?" do 
    File.exists?(@file).should == true
    File.exists?('a_fake_file').should == false
  end

  it "raise an exception if the argumnent is not from the correct type or are missing" do
    should_raise(ArgumentError){ File.exist? }
    should_raise(ArgumentError){ File.exist?(@file, @file) }
    should_raise(TypeError){ File.exist?(nil) }
  end 
   
  after(:each) do
    #FileUtils.remove_file(@file)
    @file = nil
  end  
end

describe "File::Constants" do 
  specify "match mode constants" do 
    File::FNM_NOESCAPE.should_not == nil
    File::FNM_PATHNAME.should_not == nil
    File::FNM_DOTMATCH.should_not == nil
    File::FNM_CASEFOLD.should_not == nil
    File::FNM_SYSCASE.should_not == nil

    if WINDOWS #|| VMS
      File::FNM_SYSCASE.should == 8
    end
  end

  # Only these constants are not inherited from the IO class
  specify "the sperator constant" do
    File::SEPARATOR.should_not == nil 
    File::Separator.should_not == nil
    File::PATH_SEPARATOR.should_not == nil
    File::SEPARATOR.should == "/"

    if WINDOWS #|| VMS
      File::ALT_SEPARATOR.should_not == nil
      File::PATH_SEPARATOR.should == ";"
    else
      File::ALT_SEPARATOR.should == nil
      File::PATH_SEPARATOR.should == ":"
    end
  end

  specify "the open mode constants" do   
    File::APPEND.should_not == nil
    File::CREAT.should_not == nil
    File::EXCL.should_not == nil
    File::NONBLOCK.should_not == nil
    File::RDONLY.should_not == nil
    File::RDWR.should_not == nil
    File::TRUNC.should_not == nil
    File::WRONLY.should_not == nil
      
    unless WINDOWS # Not sure about VMS here
      File::NOCTTY.should_not == nil
    end
  end

  specify "lock mode constants" do 
    File::LOCK_EX.should_not == nil
    File::LOCK_NB.should_not == nil
    File::LOCK_SH.should_not == nil
    File::LOCK_UN.should_not == nil
  end
end

describe "File.expand_path" do
  before(:each) do
    if WINDOWS
      @base = `cd`.chomp.tr '\\', '/'
      @tmpdir = "c:/tmp"
      @rootdir = "c:/"
    else
      @base = `pwd`.chomp
      @tmpdir = "/tmp"
      @rootdir = "/"
      @pwd  = Dir.pwd
    end
  end
  
  it "Converts a pathname to an absolute pathname" do 
    File.expand_path('').should == @base
    File.expand_path('a').should == File.join(@base, 'a')
    File.expand_path('a', nil).should == File.join(@base, 'a')
  end
  
  it "Converts a pathname to an absolute pathname, Ruby-Talk:18512 " do 
    # Because of Ruby-Talk:18512
    File.expand_path('a.').should == File.join(@base, 'a.')
    File.expand_path('.a').should == File.join(@base, '.a')
    File.expand_path('a..').should == File.join(@base, 'a..')
    File.expand_path('..a').should == File.join(@base, '..a')
    File.expand_path('a../b').should == File.join(@base, 'a../b')
  end
  
  it "Converts a pathname to an absolute pathname, using a complete path" do     
    File.expand_path("", "#{@tmpdir}").should == "#{@tmpdir}"
    File.expand_path("a", "#{@tmpdir}").should =="#{@tmpdir}/a"
    File.expand_path("../a", "#{@tmpdir}/xxx").should == "#{@tmpdir}/a"
    File.expand_path(".", "#{@rootdir}").should == "#{@rootdir}"
  end
  
  unless not home = ENV['HOME']
    it "Converts a pathname to an absolute pathname, using ~ (home) as base" do
      File.expand_path('~').should == home
      File.expand_path('~', '/tmp/gumby/ddd').should == home
      File.expand_path('~/a', '/tmp/gumby/ddd').should == File.join(home, 'a')
    end
  end
  
  unless WINDOWS
    it "expand path with " do      
      File.expand_path("../../bin", "/tmp/x").should == "/bin" 
      File.expand_path("../../bin", "/tmp").should == "/bin"
      File.expand_path("../../bin", "/").should == "/bin"
      File.expand_path("../../bin", "tmp/x").should == File.join(@pwd, 'bin')
    end
  end  
  
  unless WINDOWS
    specify "expand_path for commoms unix path  give a full path" do      
      File.expand_path('/tmp/').should =='/tmp'
      File.expand_path('/tmp/../../../tmp').should == '/tmp'
      File.expand_path('').should == Dir.pwd
      File.expand_path('./////').should == Dir.pwd
      File.expand_path('.').should == Dir.pwd
      File.expand_path(Dir.pwd).should == Dir.pwd
      File.expand_path('~/').should == ENV['HOME']
      File.expand_path('~/..badfilename').should == ENV['HOME'] + '/..badfilename'
      File.expand_path('..').should == Dir.pwd.split('/')[0...-1].join("/")
      File.expand_path('//').should == '//'
    end
  end 
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.expand_path }
    should_raise(TypeError){ File.expand_path(1) }
    should_raise(TypeError){ File.expand_path(nil) }
    should_raise(TypeError){ File.expand_path(true) }
      
    unless WINDOWS
      should_raise(ArgumentError){ File.expand_path("~a_fake_file") }
    end
  end
  
  after(:each) do
    @base = nil
    @tmpdir = nil
    @rootdir = nil    
    @pwd  = nil
  end
end

describe "File.extname" do
  it "returns the extension (the portion of file name in path after the period)." do 
    File.extname("foo.rb").should == ".rb"
    File.extname("/foo/bar.rb").should == ".rb"
    File.extname("/foo.rb/bar.c").should == ".c"
    File.extname("bar").should == ""
    File.extname(".bashrc").should == ""
    File.extname("/foo.bar/baz").should == ""
    File.extname(".app.conf").should == ".conf"
  end

  it "returns the extension (the portion of file name in path after the period).(edge cases)" do 
    File.extname("").should ==  ""
    File.extname(".").should ==  ""
    File.extname("/").should ==  ""
    File.extname("/.").should ==  ""
    File.extname("..").should ==  ""
    File.extname(".foo.").should ==  ""
    File.extname("foo.").should ==  ""
  end
  
  it "returns only the last extension of a file with several dots" do
    File.extname("a.b.c.d.e").should == ".e"
  end

  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(TypeError){ File.extname(nil) }
    should_raise(TypeError){ File.extname(0) }
    should_raise(TypeError){ File.extname(true) }
    should_raise(TypeError){ File.extname(false) }
    should_raise(ArgumentError){ File.extname("foo.bar", "foo.baz") }
  end  
end

describe "File.file?" do 
  before(:each) do 
    if WINDOWS
      @null = "NUL"
      @dir  = "C:\\"
    else
      @null = "/dev/null"
      @dir  = "/bin"
    end

    @file = "test.txt"
    File.open(@file1, "w"){} # touch
  end
  
  it "returns true if the named file exists and is a regular file." do 
    File.file?(@file).should == true
    File.file?(@dir).should == false
    File.file?(@null).should == false # May fail on MS Windows
  end

  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.file? }
    should_raise(ArgumentError){ File.file?(@null, @file) }
    should_raise(TypeError){ File.file?(nil) }
  end

  after(:each) do
    #FileUtils.remove_file(@file)
    @null = nil
    @file = nil
  end
end

describe "File.fnmatch" do  
  it "match entire strings" do 
    File.fnmatch('cat',       'cat').should == true        #=> true  : match entire string
  end
  
  it "match when use a partial strings" do 
    File.fnmatch('cat',       'category').should == false   #=> false : only match partial string
  end
  
  it "not match  when use { } because is not supported" do
    File.fnmatch('c{at,ub}s', 'cats').should == false       #=> false : { } isn't supported
  end
  
  it "match when use the wildcard for one-character expresion (?)" do
    File.fnmatch('c?t',     'cat').should == true          #=> true  : '?' match only 1 character
    File.fnmatch('c??t',    'cat').should == false          #=> false : ditto
  end
  
  it "match when use the wildcard for 0 or more characters expresion (*)" do
    File.fnmatch('c*',      'cats').should == true         #=> true  : '*' match 0 or more characters
    File.fnmatch('c*t',     'c/a/b/t').should == true      #=> true  : ditto
  end
  
  it "match when use inclusive bracket expresion [a-z] " do
    File.fnmatch('ca[a-z]', 'cat').should == true          #=> true  : inclusive bracket expression
  end  
  
  it "not match when the character is out of the range using an inclusive bracket expresion [x-z] " do
    File.fnmatch('ca[x-z]', 'cat').should == false          #=> true  : inclusive bracket expression
  end
  
  it "match exclusive bracket expresion [^t] or [!t]" do
    File.fnmatch('ca[^t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
    File.fnmatch('ca[!t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
  end
  
  it "match case sensitive" do
    File.fnmatch('cat', 'CAT').should == false              #=> false : case sensitive
  end
  
  it "match case insensitive" do
    File.fnmatch('cat', 'CAT', File::FNM_CASEFOLD).should == true #=> true  : case insensitive
  end
  
  it "not match a character using the wildcard '?'" do
    File.fnmatch('?',   '/', File::FNM_PATHNAME).should == false  #=> false : wildcard doesn't match '/' on FNM_PATHNAME
  end
  
  it "not match a character using the wildcard '*'" do
    File.fnmatch('*',   '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "not match a character using a charater insede a bracket expression " do
    File.fnmatch('[/]', '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "escaped wildcard becomes ordinary" do
    File.fnmatch('\?',   '?').should == true                       #=> true  : escaped wildcard becomes ordinary
  end
  
  it "escaped ordinary remains ordinary" do
    File.fnmatch('\a',   'a').should == true                       #=> true  : escaped ordinary remains ordinary
  end
  
  it " FNM_NOESACPE makes '\' ordinary" do
    File.fnmatch('\a',   '\a', File::FNM_NOESCAPE).should == true  #=> true  : FNM_NOESACPE makes '\' ordinary
  end
  
  it "can escape inside bracket expression" do
    File.fnmatch('[\?]', '?').should == true                       #=> true  : can escape inside bracket expression
  end
  
  it "wildcards doesnt match leading by default" do
    File.fnmatch('*',   '.profile').should == false                      #=> false : wildcard doesn't match leading
    File.fnmatch('*',   '.profile', File::FNM_DOTMATCH).should == true  #=> true    period by default.
    File.fnmatch('.*',  '.profile').should == true                      #=> true
  end

  it "match some paths using a composite string" do
    rbfiles = '**' '/' '*.rb'                           # you don't have to do like this. just write in single string.
    File.fnmatch(rbfiles, 'main.rb').should == false    #=> false
    File.fnmatch(rbfiles, './main.rb').should == false  #=> false
    File.fnmatch(rbfiles, 'lib/song.rb').should == true #=> true
    File.fnmatch('**.rb', 'main.rb').should == true     #=> true
    File.fnmatch('**.rb', './main.rb').should == false  #=> false
    File.fnmatch('**.rb', 'lib/song.rb').should == true #=> true
    File.fnmatch('*',     'dave/.profile').should == true  #=> true
  end
  
  it "match usign File::CONSTANT" do
    pattern = '*' '/' '*'
    File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME)  #=> false
    File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true

    pattern = '**' '/' 'foo'
    File.fnmatch(pattern, 'a/b/c/foo', File::FNM_PATHNAME)     #=> true
    File.fnmatch(pattern, '/a/b/c/foo', File::FNM_PATHNAME)    #=> true
    File.fnmatch(pattern, 'c:/a/b/c/foo', File::FNM_PATHNAME)  #=> true
    File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME)    #=> false
    File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  end
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.fnmatch(@path1, @path1, 0, 0) }
    should_raise(TypeError){ File.fnmatch(1, @path1) }
    should_raise(TypeError){ File.fnmatch(@path1, 1) }
    should_raise(TypeError){ File.fnmatch(@path1, @path2, @path3) }
  end 
end


describe "File.fnmatch?" do  
  it "match entire strings" do 
    File.fnmatch?('cat',       'cat').should == true        #=> true  : match entire string
  end
  
  it "match when use a partial strings" do 
    File.fnmatch?('cat',       'category').should == false   #=> false : only match partial string
  end
  
  it "not match  when use { } because is not supported" do
    File.fnmatch?('c{at,ub}s', 'cats').should == false       #=> false : { } isn't supported
  end
  
  it "match when use the wildcard for one-character expresion (?)" do
    File.fnmatch?('c?t',     'cat').should == true          #=> true  : '?' match only 1 character
    File.fnmatch?('c??t',    'cat').should == false          #=> false : ditto
  end
  
  it "match when use the wildcard for 0 or more characters expresion (*)" do
    File.fnmatch?('c*',      'cats').should == true         #=> true  : '*' match 0 or more characters
    File.fnmatch?('c*t',     'c/a/b/t').should == true      #=> true  : ditto
  end
  
  it "match when use inclusive bracket expresion [a-z] " do
    File.fnmatch?('ca[a-z]', 'cat').should == true          #=> true  : inclusive bracket expression
  end  
  
  it "not match when the character is out of the range using an inclusive bracket expresion [x-z] " do
    File.fnmatch?('ca[x-z]', 'cat').should == false          #=> true  : inclusive bracket expression
  end
  
  it "match exclusive bracket expresion [^t] or [!t]" do
    File.fnmatch?('ca[^t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
    File.fnmatch?('ca[!t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
  end
  
  it "match case sensitive" do
    File.fnmatch?('cat', 'CAT').should == false              #=> false : case sensitive
  end
  
  it "match case insensitive" do
    File.fnmatch?('cat', 'CAT', File::FNM_CASEFOLD).should == true #=> true  : case insensitive
  end
  
  it "not match a character using the wildcard '?'" do
    File.fnmatch?('?',   '/', File::FNM_PATHNAME).should == false  #=> false : wildcard doesn't match '/' on FNM_PATHNAME
  end
  
  it "not match a character using the wildcard '*'" do
    File.fnmatch?('*',   '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "not match a character using a charater insede a bracket expression " do
    File.fnmatch?('[/]', '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "escaped wildcard becomes ordinary" do
    File.fnmatch?('\?',   '?').should == true                       #=> true  : escaped wildcard becomes ordinary
  end
  
  it "escaped ordinary remains ordinary" do
    File.fnmatch?('\a',   'a').should == true                       #=> true  : escaped ordinary remains ordinary
  end
  
  it " FNM_NOESACPE makes '\' ordinary" do
    File.fnmatch?('\a',   '\a', File::FNM_NOESCAPE).should == true  #=> true  : FNM_NOESACPE makes '\' ordinary
  end
  
  it "can escape inside bracket expression" do
    File.fnmatch?('[\?]', '?').should == true                       #=> true  : can escape inside bracket expression
  end
  
  it "wildcards doesnt match leading by default" do
    File.fnmatch?('*',   '.profile').should == false                      #=> false : wildcard doesn't match leading
    File.fnmatch?('*',   '.profile', File::FNM_DOTMATCH).should == true  #=> true    period by default.
    File.fnmatch?('.*',  '.profile').should == true                      #=> true
  end

  it "match some paths using a composite string" do
    rbfiles = '**' '/' '*.rb'                           # you don't have to do like this. just write in single string.
    File.fnmatch?(rbfiles, 'main.rb').should == false    #=> false
    File.fnmatch?(rbfiles, './main.rb').should == false  #=> false
    File.fnmatch?(rbfiles, 'lib/song.rb').should == true #=> true
    File.fnmatch?('**.rb', 'main.rb').should == true     #=> true
    File.fnmatch?('**.rb', './main.rb').should == false  #=> false
    File.fnmatch?('**.rb', 'lib/song.rb').should == true #=> true
    File.fnmatch?('*',     'dave/.profile').should == true  #=> true
  end
  
  it "match usign File::CONSTANT" do
    pattern = '*' '/' '*'
    File.fnmatch?(pattern, 'dave/.profile', File::FNM_PATHNAME)  #=> false
    File.fnmatch?(pattern, 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true

    pattern = '**' '/' 'foo'
    File.fnmatch?(pattern, 'a/b/c/foo', File::FNM_PATHNAME)     #=> true
    File.fnmatch?(pattern, '/a/b/c/foo', File::FNM_PATHNAME)    #=> true
    File.fnmatch?(pattern, 'c:/a/b/c/foo', File::FNM_PATHNAME)  #=> true
    File.fnmatch?(pattern, 'a/.b/c/foo', File::FNM_PATHNAME)    #=> false
    File.fnmatch?(pattern, 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  end
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.fnmatch?(@path1, @path1, 0, 0) }
    should_raise(TypeError){ File.fnmatch?(1, @path1) }
    should_raise(TypeError){ File.fnmatch?(@path1, 1) }
    should_raise(TypeError){ File.fnmatch?(@path1, @path2, @path3) }
  end 
end

describe "File.atime" do
  before(:each) do
    @file = File.join(Dir.pwd, 'test.txt')
    File.open(@file1, "w"){} # touch
  end

  it "returns the last access time for the named file as a Time object" do      
    File.atime(@file)
    File.atime(@file).class.should == Time
  end

  it "raise an Errno::ENOENT exception if the file is not found" do 
    should_raise(Errno::ENOENT){ File.atime('a_fake_file') }
  end
   
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.atime }
    should_raise(ArgumentError){ File.atime(@file, @file) }
    should_raise(TypeError){ File.atime(1) }
  end

  after(:each) do
    #FileUtils.remove_file(@file)
    @file = nil
  end
end