require File.dirname(__FILE__) + '/../spec_helper' 
failure :rbx do
  require 'pathname'
end

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

WINDOWS = RUBY_PLATFORM.match('mswin')

describe "File.dirname" do   
  it "dirname should return all the components of filename except the last one" do
    File.dirname('/home/jason').should == '/home'
    File.dirname('/home/jason/poot.txt').should == '/home/jason'
    File.dirname('poot.txt').should == '.'
    File.dirname('/holy///schnikies//w00t.bin').should == '/holy///schnikies'
    File.dirname('').should == '.'
    File.dirname('/').should == '/'
    File.dirname('/////').should == '/'
  end 
  
  it "return a String" do
    File.dirname("foo").class.should == String
  end

  it "return the return all the components of filename except the last one (unix format)" do
    File.dirname("foo").should =="."
    File.dirname("/foo").should =="/"
    File.dirname("/foo/bar").should =="/foo"
    File.dirname("/foo/bar.txt").should =="/foo"
    File.dirname("/foo/bar/baz").should =="/foo/bar"
  end

  it "return all the components of filename except the last one (edge cases)" do
    File.dirname("").should == "."
    File.dirname(".").should == "."
    File.dirname("..").should == "."
    File.dirname("/").should == "/"
    File.dirname("/foo/").should == "/"    
    File.dirname("//foo//").should == "/"
    File.dirname("//foo").should == "/" unless WINDOWS  # Fails on MS Windows
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do
    should_raise(TypeError){ File.dirname(nil) }
    should_raise(TypeError){ File.dirname(0) }
    should_raise(TypeError){ File.dirname(true) }
    should_raise(TypeError){ File.dirname(false) }
  end

  # Windows specific tests
  if WINDOWS
    it "return the return all the components of filename except the last one (Windows format)" do 
      File.dirname("C:\\foo\\bar\\baz.txt").should =="C:\\foo\\bar"
      File.dirname("C:\\foo\\bar").should =="C:\\foo"
      File.dirname("C:\\foo\\bar\\").should == "C:\\foo"
      File.dirname("C:\\foo").should == "C:\\"
      File.dirname("C:\\").should =="C:\\"
    end

    it "return the return all the components of filename except the last one (windows unc)" do
      File.dirname("\\\\foo\\bar\\baz.txt").should == "\\\\foo\\bar"
      File.dirname("\\\\foo\\bar\\baz").should == "\\\\foo\\bar"
      File.dirname("\\\\foo").should =="\\\\foo"
      File.dirname("\\\\foo\\bar").should =="\\\\foo\\bar"
    end
         
    it "return the return all the components of filename except the last one (forward_slash)" do 
      File.dirname("C:/").should == "C:/"
      File.dirname("C:/foo").should == "C:/"
      File.dirname("C:/foo/bar").should == "C:/foo"
      File.dirname("C:/foo/bar/").should == "C:/foo"
      File.dirname("C:/foo/bar//").should == "C:/foo"
    end
  end 
end

describe "File.basename" do  
  before :each do     
    @file = "test.txt"  
    File.delete(@file) if File.exist? @file
    File.open(@file,"w+") 
  end
  
  after :each do
    #File.delete(@file)
  end
  
  it "return the basename of a path (basic cases)" do     
    File.basename(@file).should == "test.txt"
    File.basename(File.join("/tmp")).should == "tmp"
    File.basename(File.join(*%w( g f d s a b))).should == "b"
    File.basename("/tmp", ".*").should == "tmp"
    File.basename("/tmp", ".c").should == "tmp"
    File.basename("/tmp.c", ".c").should == "tmp"
    File.basename("/tmp.c", ".*").should == "tmp"
    File.basename("/tmp.o", ".c").should == "tmp.o"
    #Version.greater_or_equal("1.8.0") do
    File.basename(File.join("/tmp/")).should == "tmp"
    File.basename("/").should == "/"
    File.basename("//").should == "/"
    File.basename("dir///base", ".*").should == "base"
    File.basename("dir///base", ".c").should == "base"
    File.basename("dir///base.c", ".c").should == "base"
    File.basename("dir///base.c", ".*").should == "base"
    File.basename("dir///base.o", ".c").should == "base.o"
    File.basename("dir///base///").should == "base"
    File.basename("dir//base/", ".*").should == "base"
    File.basename("dir//base/", ".c").should == "base"
    File.basename("dir//base.c/", ".c").should == "base"
    File.basename("dir//base.c/", ".*").should == "base"
    #end
  end
  
  it "return the last component of the filename" do
    File.basename('a').should == 'a'
    File.basename('/a').should == 'a'
    File.basename('/a/b').should == 'b'
    File.basename('/ab/ba/bag').should == 'bag'
    File.basename('/ab/ba/bag.txt').should == 'bag.txt'
    File.basename('/').should == '/'
    File.basename('/foo/bar/baz.rb', '.rb').should == 'baz'
    File.basename('baz.rb', 'z.rb').should == 'ba'
  end 
  
  it "return an string" do 
    File.basename("foo").class.should == String
  end

  it "return the basename for unix format" do 
    File.basename("/foo/bar").should == "bar"
    File.basename("/foo/bar.txt").should == "bar.txt"
    File.basename("bar.c").should == "bar.c"
    File.basename("/bar").should == "bar"
    File.basename("/bar/").should == "bar"
      
    # Considered UNC paths on Windows
    unless WINDOWS 
      File.basename("baz//foo").should =="foo"
      File.basename("//foo/bar/baz").should == "baz"
    end
  end

  it "return the basename for edge cases" do  
    File.basename("").should == ""
    File.basename(".").should == "."
    File.basename("..").should == ".."
    File.basename("//foo/").should == "foo"
    File.basename("//foo//").should == "foo"
  end
      
  it "return the basename for unix suffix" do
    File.basename("bar.c", ".c").should == "bar"
    File.basename("bar.txt", ".txt").should == "bar"
    File.basename("/bar.txt", ".txt").should == "bar"
    File.basename("/foo/bar.txt", ".txt").should == "bar"
    File.basename("bar.txt", ".exe").should == "bar.txt"
    File.basename("bar.txt.exe", ".exe").should == "bar.txt"
    File.basename("bar.txt.exe", ".txt").should == "bar.txt.exe"
    File.basename("bar.txt", ".*").should == "bar"
    File.basename("bar.txt.exe", ".*").should == "bar.txt"
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do
    should_raise(TypeError){ File.basename(nil) }
    should_raise(TypeError){ File.basename(1) }
    should_raise(TypeError){ File.basename("bar.txt", 1) }
    should_raise(TypeError){ File.basename(true) }
    should_raise(ArgumentError){ File.basename('bar.txt', '.txt', '.txt') }
  end

  # specific to MS Windows
  unless not WINDOWS 
    it "return the basename for windows" do  
      File.basename("C:\\foo\\bar\\baz.txt").should == "baz.txt"
      File.basename("C:\\foo\\bar").should == "baz"
      File.basename("C:\\foo\\bar\\").should == "baz"
      File.basename("C:\\foo").should == "foo"
      File.basename("C:\\").should == "C:\\"
    end

    it "return basename windows unc" do 
      File.basename("\\\\foo\\bar\\baz.txt").shoould == "baz.txt"
      File.basename("\\\\foo\\bar\\baz").shoould =="baz"
      File.basename("\\\\foo").should == "\\\\foo"
      File.basename("\\\\foo\\bar").shoould == "\\\\foo\\bar"
    end
         
    it "return basename windows forward slash" do  
      File.basename("C:/").should == "C:/"
      File.basename("C:/foo").should == "foo"
      File.basename("C:/foo/bar").should == "bar"
      File.basename("C:/foo/bar/").should "bar"
      File.basename("C:/foo/bar//").shouldl == "bar"
    end

    it "return basename with windows suffix" do
      File.basename("c:\\bar.txt", ".txt").should == "bar"
      File.basename("c:\\foo\\bar.txt", ".txt").should == "bar"
      File.basename("c:\\bar.txt", ".exe").should == "bar.txt"
      File.basename("c:\\bar.txt.exe", ".exe").should == "bar.txt"
      File.basename("c:\\bar.txt.exe", ".txt").should == "bar.txt.exe"
      File.basename("c:\\bar.txt", ".*").should == "bar"
      File.basename("c:\\bar.txt.exe", ".*").should == "bar.txt"
    end
  end
end 

describe "File.blockdev?" do
  it "returns true/false depending if the named file is a block device" do
    File.blockdev?("/tmp").should == false
  end
end

describe "File.chardev?" do
  it "returns true/false depending if the named file is a char device" do
    File.chardev?("/tmp").should == false
  end
end 

describe "File.executable_real?" do
  before(:each) do
    @file1 = 'test.txt'
    @file2 = 'test2.txt'

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
      
    File.chmod(0755, @file1)
  end
  
  it "returns true if named file is readable by the real user id of the process, otherwise false" do
    File.executable_real?('fake_file').should == false
    File.executable_real?(@file1).should == true
  end
  
  after(:each) do
    File.delete(@file1)    
    File.delete(@file2) 
    @file1 = nil
    @file2 = nil
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
end   
 
describe "File.executable?" do 
  before(:each) do
    @file1 = File.join('test.txt')
    @file2 = File.join('test2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
    
    File.chmod(0755, @file1)
  end

  after(:each) do
    File.delete(@file1)    
    File.delete(@file2) 
    @file1 =  nil
    @file2 = nil
  end
  
  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.executable?('fake_file').should == false
    File.executable?('/etc/passwd').should == false
    File.executable?(@file).should == true
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
end

describe "File.readable_real?" do
  before(:each) do
    @file = '/tmp/i_exist'
  end
  
  after(:each) do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "returns true if named file is readable by the real user id of the process, otherwise false" do
    File.readable_real?('fake_file').should == false
    File.open(@file,'w'){
      File.readable_real?(@file).should == true
    }
  end
end

describe "File.readable?" do
  before(:each) do
    @file = '/tmp/i_exist'
  end
  
  after(:each) do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.readable?('fake_file').should == false
    File.readable?('/etc/passwd').should == true
    File.open(@file,'w'){
      File.readable?(@file).should == true
    }
  end
end
 
describe "File.writable?" do
  before(:each) do
    @file = '/tmp/i_exist'
  end
  
  after(:each) do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "should return true if named file is writable by the effective user id of the process, otherwise false" do
    File.writable?('fake_file').should == false
    File.writable?('/etc/passwd').should == false
    File.open(@file,'w'){
      File.writable?(@file).should == true
    }
  end
end

describe "File.link" do
  before(:each) do   
    @file = "test.txt"
    @link = "test.lnk"     
    File.delete(@link) if File.exists?(@link)
    File.delete(@file) if File.exists?(@file)
    File.open(@file,"w+") 
  end 

  unless WINDOWS
    it "link a file with another " do
      File.link(@file, @link).should == 0
      File.exists?(@link).should == true
      File.identical?(@file, @link).should == true
    end

    it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
      File.link(@file, @link)  
      should_raise(Errno::EEXIST){ File.link(@file, @link) }
      should_raise(ArgumentError){ File.link }
      should_raise(ArgumentError){ File.link(@file) }
    end
  end

  after(:each)do
    File.unlink(@link)
    File.delete(@file)
    @link = nil
  end
end

describe "File.unlink" do
  before(:each) do
    @file = "i_dont_exist"
    File.delete(@file) if File.exists?(@file)
  end
  
  after(:each) do
    File.delete(@file) rescue nil
  end
  
  it "should delete a file and return the number of names passed as arguments" do
    File.exists?(@file).should == false
    File.new(@file,'w').close
    File.exists?(@file).should == true
    File.unlink(@file).should == 1
    File.exists?(@file).should == false 
  end
  
  it "should raise Errno::ENOENT if the file does not exist" do
    should_raise(Errno::ENOENT){ File.unlink('bogus') }
  end
end 


describe "File.zero?" do
  before :each do
    @zero_file    = 'test.txt'
    @nonzero_file = 'test2.txt'
      
    File.open(@zero_file    , "w"){} # Touch
    File.open(@nonzero_file , "w"){|f| f.puts "hello"} # Touch
  end
  
  after :each do
    File.delete(@zero_file    ) if File.exists?(@zero_file)
    File.delete(@nonzero_file ) if File.exists?(@nonzero_file)  
    @zero_file    = nil
    @nonzero_file = nil
  end
     
  it "return true if the length of a file its zero, otherwise true" do
    File.zero?(@zero_file).should == true
    File.zero?(@nonzero_file).should == false
  end
   
  it " edgy cases" do
    if WINDOWS
      File.zero?('NUL').should == true
    else
      File.zero?('/dev/null').should == true
    end
  end
  
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do  
    should_raise(ArgumentError){ File.zero? }
    should_raise(TypeError){ File.zero?(nil) }
    should_raise(TypeError){ File.zero?(true) }
    should_raise(TypeError){ File.zero?(false) }
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
end 

describe "File.size?" do
  before(:each) do
    @file = "i_dont_exist"
    File.delete(@file) if File.exists?(@file)
    File.size?(@file).should == nil
    @file = '/tmp/i_exist'
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end
  
  after(:each) do
    File.delete(@file) rescue nil
  end
  
  it "should return nil if file_name doesn't exist or has zero size, the size of the file otherwise" do
    File.size?(@file).should == 8
  end
end

describe "File.size" do
  before(:each) do
    @file = '/tmp/i_exist'
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end
  
  after(:each) do
    File.delete(@file) rescue nil
  end
  
  it "should return the size of the file" do
    File.size?(@file).should == 8
  end
end 
 
describe "File.identical?" do  
  before(:each) do    
    @file1 = 'test.txt'
    @file2 = 'test2.txt'
    @file3 = 'test.lnk'
    File.delete(@file3) if File.exists?(@file3)
     
    File.open(@file1,"w+"){|f| f.puts "file1"}
    File.open(@file2,"w+"){|f| f.puts "file2"} 
    File.link(@file1, @file3)
  end

  it  "return a Boolean class" do 
    File.identical?(@file1, @file2).class.should == FalseClass
    File.identical?(@file1, @file1).class.should == TrueClass
  end

  it "return true if they are identicals" do
    File.identical?(@file1, @file1).should == true
    File.identical?(@file1, @file2).should == false
    File.identical?(@file1, @file3).should == true
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.identical?(@file1, @file2, @file3) }
    should_raise(TypeError){ File.identical?(1,1) }
  end

  it "identical? should return true if both named files are identical" do
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
  
  after(:each) do          
    File.unlink(@file3)  
    File.delete(@file1) if File.exists?(@file1)    
    File.delete(@file2) if File.exists?(@file2)     
    @file1 = nil
    @file1 = nil
    @file1 = nil
  end
end

describe "File.join" do
  it "should return a new string formed by joining the strings using File::SEPARATOR" do
    File.join("smalltalk","ruby","rubinius").should == "smalltalk/ruby/rubinius"
    File.join.should == ""

    # arguments must respond to to_str
    should_raise(TypeError) do
      File.join(Object.new)
    end
  end
end

describe "File#path" do
  before(:each) do
    @file1 = "testfile"
    @file2 = "/tmp/../tmp/xxx"
  end
  
  after(:each) do
    File.delete(@file1) rescue nil
    File.delete(@file2) rescue nil
  end
  
  it "should return the pathname used to create file as a string" do
    File.open(@file1,'w'){|file| file.path.should == "testfile"}
    File.open(@file2, 'w'){|file| file.path.should == "/tmp/../tmp/xxx"}
  end
end

describe "File#atime" do
  before(:each) do
    @name = File.expand_path(__FILE__)
    @file = File.open(@name)
  end

  after(:each) do 
    @name = nil
    @file = nil
  end

  it "returns the last access time to self" do
    @file.atime  
    @file.atime.class.should == Time
  end

  it "raise an Exception if it has the worng number of argments" do
    should_raise(ArgumentError){ @file.atime(@name) }
  end
end
 
describe "File#ctime" do
  before(:each) do
    @file = File.open(__FILE__)
  end

  after(:each) do
    @file = nil
  end

  it "Returns the change time for the named file (the time at which directory information about the file was changed, not the file itself)." do 
    @file.ctime
    @file.ctime.class.should == Time
  end 
  
  it  "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ @file.ctime(@file, @file) } 
  end
end 

describe "File.ctime" do
  before(:each) do
    @file = __FILE__
  end

  after(:each) do
    @file = nil
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

  after(:each) do
    @dir = nil
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
end

describe "File.executable?" do 
  before(:each) do
    @file1 = 'temp1.txt'
    @file2 = 'temp2.txt'

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){} # touch
    
    File.chmod(0755, @file1)
  end

  after(:each) do
    File.delete("temp1.txt")    
    File.delete("temp2.txt")

    @file1 =  nil
    @file2 = nil
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
end

describe "File.executable_real?" do
  before(:each) do
    @file1 = File.join(Dir.pwd, 'temp1.txt')
    @file2 = File.join(Dir.pwd, 'temp2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
      
    File.chmod(0755, @file1)
  end
 
  after(:each) do
    File.delete("temp1.txt")    
    File.delete("temp2.txt")

    @file1 = nil
    @file2 = nil
  end 
 
  unless WINDOWS
    it "returns true if the file its an executable" do 
      File.executable_real?(@file1).should == true
      File.executable_real?(@file2).should == false
      File.executable_real?('bogus').should == false
    end
  end
  
  it "raise an exception if the argument is not from the correct type or are missing" do
    should_raise(ArgumentError){ File.executable_real? }
    should_raise(TypeError){ File.executable_real?(1) }
    should_raise(TypeError){ File.executable_real?(nil) }
    should_raise(TypeError){ File.executable_real?(false) }
  end
end

describe "File.chmod" do
  before(:each) do
    @file = '/tmp/i_exist'
    File.open(@file, 'w') {}
    @count = File.chmod(0755, @file)
  end
  
  it "should return the number of files modified" do
    @count.should == 1
  end
  
  it "should modify the permission bits of the files specified" do
    File.stat(@file).mode.should == 33261
  end
  
  after(:each) do
    File.delete(@file) if File.exist?(@file)
  end  
end

describe "File#chmod" do
  before(:each) do
    @filename = '/tmp/i_exist'
    @file = File.open(@filename, 'w')
  end
  
  it "should modify the permission bits of the files specified" do
    @file.chmod(0755).should == 0
    File.stat(@filename).mode.should == 33261
  end
  
  after(:each) do
    @file.close
    File.delete(@filename) if File.exist?(@filename)
  end
end

describe "File.exist?" do 
  before(:each) do
    @file = 'temp.txt'
    File.open(@file, "w"){}
  end 

  after(:each) do
    File.delete("temp.txt")
    @file = nil
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
end

describe "File::Constants" do  
  # These mode and permission bits are platform dependent
  specify "File::RDONLY" do 
    defined?(File::RDONLY).should == "constant" 
  end
 
  specify "File::WRONLY" do  
    defined?(File::WRONLY).should == "constant" 
  end
 
  specify "File::CREAT" do     
    defined?(File::CREAT).should == "constant" 
  end
 
  specify "File::RDWR" do  
    defined?(File::RDWR).should == "constant" 
  end
 
  specify "File::APPEND" do      
    defined?(File::APPEND).should == "constant" 
  end
 
  specify "File::TRUNC" do     
    defined?(File::TRUNC).should == "constant" 
  end   
  
  unless WINDOWS # Not sure about VMS here
    specify "File::NOCTTY" do     
      defined?(File::NOCTTY).should == "constant" 
    end   
  end
  
  specify "File::NONBLOCK" do     
    defined?(File::NONBLOCK).should == "constant" 
  end   
  
  specify "File::LOCK_EX" do     
    defined?(File::LOCK_EX).should == "constant" 
  end   
  
  specify "File::LOCK_NB" do     
    defined?(File::LOCK_NB).should == "constant" 
  end   
  
  specify "File::LOCK_SH" do     
    defined?(File::LOCK_SH).should == "constant" 
  end   
  
  specify "File::LOCK_UN" do     
    defined?(File::LOCK_UN).should == "constant" 
  end  
  
  specify "File::SEPARATOR" do     
    defined?(File::SEPARATOR).should == "constant" 
  end  
  specify "File::Separator" do     
    defined?(File::Separator).should == "constant" 
  end  
  
  specify "File::PATH_SEPARATOR" do     
    defined?(File::PATH_SEPARATOR).should == "constant" 
  end  
  
  specify "File::SEPARATOR" do     
    defined?(File::SEPARATOR).should == "constant" 
    File::SEPARATOR.should == "/" 
  end   
  
  if WINDOWS #|| VMS 
    specify "File::ALT_SEPARATOR" do     
      defined?(File::ALT_SEPARATOR).should == "constant" 
      File::PATH_SEPARATOR.should == ";" 
    end  
  else 
    specify "File::PATH_SEPARATOR" do     
      defined?(File::PATH_SEPARATOR).should == "constant"       
      File::PATH_SEPARATOR.should == ":"
    end  
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
  specify "the separator constant" do
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

  after(:each) do
    @base = nil
    @tmpdir = nil
    @rootdir = nil    
    @pwd  = nil
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
    File.open(@file, "w"){} # touch
  end

  after(:each) do
    File.delete(@file) rescue nil
    @null = nil
    @file = nil
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

describe "File.join" do   
  before(:each) do
    @root = WINDOWS ? "C:\\" : "/"
    @dirs = ['usr', 'local', 'bin']
  end 

  after(:each) do
    @root = nil
    @dirs = nil
  end

  if WINDOWS
    it "returns a new string formed by joining the strings using File::SEPARATOR (windows)" do 
      File.join(*@dirs).should == "usr/local/bin"
      File.join(@root, *@dirs).should == "C:\\usr/local/bin"
    end

    it "returns a new string formed by joining the strings using File::SEPARATOR (edge cases on windows) " do 
      File.join("").should = ""
      File.join("", "foo").should == "/foo"
      File.join("usr", "", "local", "", "bin").should == "usr/local/bin"
      File.join("\\\\", "usr", "local").should = "\\\\usr/local"
    end
  end
  
  unless WINDOWS 
    it "returns a new string formed by joining the strings using File::SEPARATOR (unix)" do 
      File.join(*@dirs).should == "usr/local/bin"
      File.join(@root, *@dirs).should == "/usr/local/bin"
    end
    
    it "returns a new string formed by joining the strings using File::SEPARATOR (edge cases on windows) " do 
      File.join("").should == ""
      File.join("", "foo").should == "/foo"
      File.join("usr", "", "local", "", "bin").should == "usr/local/bin"
    end
  end

  it "returns a new string formed by joining the strings using File::SEPARATOR (any plataform)" do 
    [ %w( a b c d ), %w( a ), %w( ), %w( a b .. c ) ].each do |a|
      a.join(File::SEPARATOR).should == File.join(*a)
    end
  end
  
  it "raise a TypeError exception when args are nil" do
    should_raise(TypeError){ File.join(nil, nil) }
  end
end

describe "File.new" do
  before(:each) do 
    @file = 'test.txt'
    @fh = nil 
    @flags = File::CREAT | File::TRUNC | File::WRONLY
    File.open(@file, "w"){} # touch
  end

  after(:each) do   
    File.delete("test.txt") if File.exists?("test.txt")
    @fh    = nil
    @file  = nil
    @flags = nil
  end

  it "return a new File with mode string" do
    @fh = File.new(@file, 'w')
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "return a new File with mode num" do   
    @fh = File.new(@file, @flags) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "return a new File with modus num and premissions " do 
    File.delete(@file) 
    @fh = File.new(@file, @flags, 0755)
    @fh.class.should == File
    File.stat(@file).mode.to_s(8).should == "100755"
    File.exists?(@file).should == true
  end

  it "return a new File with modus fd " do 
    @fh = File.new(@file) 
    @fh = File.new(@fh.fileno) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end
  
  it "create a new file when use File::EXCL mode " do 
    @fh = File.new(@file, File::EXCL) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "raise an Errorno::EEXIST if the file exists when create a new file with File::CREAT|File::EXCL" do 
    should_raise(Errno::EEXIST){@fh = File.new(@file, File::CREAT|File::EXCL)}
  end
  
  it "create a new file when use File::WRONLY|File::APPEND mode" do 
    @fh = File.new(@file, File::WRONLY|File::APPEND) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "raise an Errno::EINVAL error with File::APPEND" do 
    should_raise(Errno::EINVAL){@fh = File.new(@file, File::APPEND)}
  end
  
  
  it "raise an Errno::EINVAL error with File::RDONLY|File::APPEND" do 
    should_raise(Errno::EINVAL){@fh = File.new(@file, File::RDONLY|File::APPEND)}
  end
  
  it "raise an Errno::EINVAL error with File::RDONLY|File::WRONLY" do 
    @fh = File.new(@file, File::RDONLY|File::WRONLY)
    @fh.class.should == File
    File.exists?(@file).should == true
  end
  
  
  it "create a new file when use File::WRONLY|File::TRUNC mode" do 
    @fh = File.new(@file, File::WRONLY|File::TRUNC) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end
  
  specify  "expected errors " do
    should_raise(TypeError){ File.new(true) }
    should_raise(TypeError){ File.new(false) }
    should_raise(TypeError){ File.new(nil) }
    should_raise(Errno::EBADF){ File.new(-1) }
    should_raise(ArgumentError){ File.new(@file, File::CREAT, 0755, 'test') }
  end

  # You can't alter mode or permissions when opening a file descriptor
  #
  it "can't alter mode or permissions when opening a file" do 
    @fh = File.new(@file)
    should_raise(Errno::EINVAL){ File.new(@fh.fileno, @flags) }
  end
end 

describe "File.open" do 
  before(:each) do         
    @file = 'test.txt'    
    File.delete(@file) if File.exists?(@file)
    File.delete("fake") if File.exists?("fake")
    @fh = nil
    @fd = nil
    @flags = File::CREAT | File::TRUNC | File::WRONLY
    File.open(@file, "w"){} # touch
  end
  
  it "open the file (basic case)" do 
    @fh = File.open(@file) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open file when call with a block (basic case)" do
    File.open(@file){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) } # Should be closed by block
    File.exists?(@file).should == true
  end

  it "open with mode string" do
    @fh = File.open(@file, 'w') 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open a file with mode string and block" do
    File.open(@file, 'w'){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.exists?(@file).should == true
  end

  it "open a file with mode num" do 
    @fh = File.open(@file, @flags)
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open a file with mode num and block" do
    File.open(@file, 'w'){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.exists?(@file).should == true
  end

  # For this test we delete the file first to reset the perms
  it "open the file when call with mode, num and permissions" do
    File.delete(@file)
    @fh = File.open(@file, @flags, 0755)
    File.stat(@file).mode.to_s(8).should == "100755"
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  # For this test we delete the file first to reset the perms
  it "open the flie when call with mode, num, permissions and block" do
    File.delete(@file)
    File.open(@file, @flags, 0755){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.stat(@file).mode.to_s(8).should == "100755"
    File.exists?(@file).should == true
  end

  it "open the file when call with fd" do
    @fh = File.open(@file)
    @fh = File.open(@fh.fileno) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  # Note that this test invalidates the file descriptor in @fh. That's
  # handled in the teardown via the 'rescue nil'.
  #
  it "open a file with a file descriptor d and a block" do 
    @fh = File.open(@file) 
    File.open(@fh.fileno){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
    
  it "open a file that no exists when use File::WRONLY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::WRONLY)  }
  end  
  
  it "open a file that no exists when use File::RDONLY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::RDONLY)  }
  end    
  
  it "open a file that no exists when use 'r' mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", 'r')  }
  end  
  
  it "open a file that no exists when use File::EXCL mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::EXCL)  }
  end  
  
  it "open a file that no exists when use File::NONBLOCK mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::NONBLOCK)  }
  end  
  
  it "open a file that no exists when use File::TRUNC mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::TRUNC)  }
  end  
  
  it "open a file that no exists when use File::NOCTTY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::NOCTTY)  }
  end  
  
  it "open a file that no exists when use File::CREAT mode" do 
    @fh = File.open("fake", File::CREAT)      
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  it "open a file that no exists when use 'a' mode" do 
    @fh = File.open("fake", 'a')      
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
     
  it "open a file that no exists when use 'w' mode" do 
    @fh = File.open("fake", 'w')  
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  # Check the grants associated to the differents open modes combinations.   
  it "raise an ArgumentError exception when call with an unknown mode" do 
    should_raise(ArgumentError){File.open(@file, "q")  }
  end
  
  it "can read in a block when call open with RDONLY mode" do 
    File.open(@file, File::RDONLY) do |f| 
      f.gets.should == nil
    end
  end
  
  it "can read in a block when call open with 'r' mode" do 
    File.open(@file, "r") do |f| 
      f.gets.should == nil
    end
  end
  
  it "raise an IO exception when write in a block opened with RDONLY mode" do 
    File.open(@file, File::RDONLY) do |f| 
      should_raise(IOError) { f.puts "writing ..." }
    end
  end
  
  it "raise an IO exception when write in a block opened with 'r' mode" do 
    File.open(@file, "r") do |f| 
      should_raise(IOError) { f.puts "writing ..." }
    end
  end
  
  it "can't write in a block when call open with File::WRONLY||File::RDONLY mode" do  
    File.open(@file, File::WRONLY|File::RDONLY ) do |f|  
      f.puts("writing").should == nil 
    end 
  end  
  
  it "can't read in a block when call open with File::WRONLY||File::RDONLY mode" do 
    should_raise(IOError) do
      File.open(@file, File::WRONLY|File::RDONLY ) do |f| 
        f.gets.should == nil       
      end
    end
  end    
  
  it "can write in a block when call open with WRONLY mode" do 
    File.open(@file, File::WRONLY) do |f| 
      f.puts("writing").should == nil
    end
  end
  
  it "can write in a block when call open with 'w' mode" do 
    File.open(@file, "w") do |f| 
      f.puts("writing").should == nil
    end
  end
  
  it "raise an IO exception when read in a block opened with WRONLY mode" do 
    File.open(@file, File::WRONLY) do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'w' mode" do 
    File.open(@file, "w") do |f| 
      should_raise(IOError) { f.gets   }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, "a") do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, "a") do |f|        
      f.puts("writing").should == nil      
      should_raise(IOError) { f.gets }
    end
  end  
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, File::WRONLY|File::APPEND ) do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with File::WRONLY|File::APPEND mode" do 
    File.open(@file, File::WRONLY|File::APPEND ) do |f|        
      f.puts("writing").should == nil  
    end
  end
  
  it "raise an IO exception when read in a block opened with File::RDONLY|File::APPEND mode" do 
    should_raise(Errno::EINVAL) do 
      File.open(@file, File::RDONLY|File::APPEND ) do |f|        
        f.puts("writing")  
      end
    end
  end
  
  it "can read and write in a block when call open with RDWR mode" do 
    File.open(@file, File::RDWR) do |f| 
      f.gets.should == nil      
      f.puts("writing").should == nil
      f.rewind
      f.gets.should == "writing\n"
    end
  end  
  
  it "can't read in a block when call open with File::EXCL mode" do 
    should_raise(IOError) do
      File.open(@file, File::EXCL) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
  
  it "can read in a block when call open with File::EXCL mode" do  
    File.open(@file, File::EXCL) do |f|  
      f.gets.should == nil      
    end 
  end    
    
  it "can read and write in a block when call open with File::RDWR|File::EXCL mode" do 
    File.open(@file, File::RDWR|File::EXCL) do |f| 
      f.gets.should == nil      
      f.puts("writing").should == nil
      f.rewind
      f.gets.should == "writing\n"
    end     
  end
  
  it "raise an Errorno::EEXIST if the file exists when open with File::CREAT|File::EXCL" do 
    should_raise(Errno::EEXIST) do
      File.open(@file, File::CREAT|File::EXCL) do |f|  
        f.puts("writing")
      end
    end
  end
 
  it "create a new file when use File::WRONLY|File::APPEND mode" do 
    @fh = File.open(@file, File::WRONLY|File::APPEND) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  it "open a file when use File::WRONLY|File::APPEND mode" do 
    File.open(@file, File::WRONLY) do |f|
      f.puts("hello file")
    end    
    File.open(@file, File::RDWR|File::APPEND) do |f|
      f.puts("bye file") 
      f.rewind
      f.gets().should == "hello file\n"
      f.gets().should == "bye file\n"
      f.gets().should == nil
    end     
  end  
  
  it "raise an Errorno::EEXIST if the file exists when open with File::RDONLY|File::APPEND" do 
    should_raise( Errno::EINVAL) do
      File.open(@file, File::RDONLY|File::APPEND) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
  
  it "create a new file when use File::TRUNC mode" do 
    # create and write in the file
    File.open(@file, File::RDWR) do |f|
      f.puts "hello file" 
    end    
    # Truncate the file    
    @fh = File.new(@file, File::TRUNC)   
    @fh.gets.should == nil
  end 
    
  
  it "can't read in a block when call open with File::TRUNC mode" do  
    File.open(@file, File::TRUNC) do |f|  
      f.gets  
    end 
  end
    
  it "open a file when use File::WRONLY|File::TRUNC mode" do 
    File.open(@file, File::WRONLY|File::TRUNC) 
    @fh.class.should == NilClass
    File.exists?(@file).should == true
  end
  
  it "can't write in a block when call open with File::TRUNC mode" do 
    should_raise(IOError) do
      File.open(@file, File::TRUNC) do |f|  
        f.puts("writing")
      end
    end
  end  
      
  it "raise an Errorno::EEXIST if the file exists when open with File::RDONLY|File::TRUNC" do 
    should_raise(IOError) do
      File.open(@file, File::RDONLY|File::TRUNC) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
   
  specify "expected errors " do
    should_raise(TypeError){ File.open(true) }
    should_raise(TypeError){ File.open(false) }
    should_raise(TypeError){ File.open(nil) }
    should_raise(SystemCallError){ File.open(-1) } # kind_of ?
    should_raise(ArgumentError){ File.open(@file, File::CREAT, 0755, 'test') }
  end
  
  after(:each) do         
    File.delete("fake") rescue nil
    @fh.delete if @fh  rescue nil
    @fh.close if @fh rescue nil
    @fh    = nil
    @fd    = nil
    @file  = nil
    @flags = nil
  end
end

describe "File.atime" do
  before(:each) do
    @file = File.join('test.txt')
    File.open(@file, "w"){} # touch
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
    File.delete("test.txt") if File.exist?("test.txt") 
    @file = nil
  end
end

describe "File.truncate" do  
  before(:each) do
    @fname = "test.txt"
    @file  = File.open(@fname, 'w')
    File.open(@fname,"w"){ |f| f.write("1234567890") }
  end
 
  it "truncate the a file" do 
    File.open(@fname, "w") { |f| f.puts "123456789" } 
    if WINDOWS
      File.size(@fname).should == 11
    else
      File.size(@fname).should == 10
    end
    File.truncate(@fname, 5)
    File.size(@fname).should == 5
    File.open(@fname, "r") do |f|
      f.read(99).should == "12345"
      f.eof?.should == true
    end
  end  

  it "truncate to 0 a file" do
    File.truncate(@fname, 0).should == 0    
    IO.read(@fname).should == ""
  end
 
  it "truncate to 5 a file"  do
    File.size(@fname).should == 10
    File.truncate(@fname, 5) 
    File.size(@fname).should == 5
    IO.read(@fname).should == "12345"
  end

  it "truncate to a larger size than the original file" do
    File.truncate(@fname, 12) 
    File.size(@fname).should == 12
    IO.read(@fname).should == "1234567890\000\000"
  end

  it "truncate to a the same size the original file" do
    File.truncate(@fname, File.size(@fname))    
    File.size(@fname).should == 10
    IO.read(@fname).should == "1234567890"
  end
   
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.truncate(@fname) }
    should_raise(Errno::EINVAL){ File.truncate(@fname, -1) } # May fail
    should_raise(TypeError){ File.truncate(@fname, nil) }
  end

  after :each do
    @file.close rescue nil
    @fname = nil
  end  
end

describe "File.umask" do
  before :each do
    @file = 'test.txt'
    File.open(@file, 'w'){}
  end
   
  it "return a Fixnum" do
    File.umask.class.should == Fixnum
  end    
  
  specify "umask should return the current umask value for the process" do
    File.umask(0006).should == 18
    File.umask.should == 6
  end
   
  if WINDOWS
    it "Returns the current umask value for this process. (basic)" do   
      File.umask.should == 0
    end
      
    # The value used here is the value of _S_IWRITE.
    it "Returns the current umask value for this process." do 
      File.umask(0000200) 
      File.umask.should == 128
    end
      
    it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do  
      File.umask(0006)  
      File.umask.should == 0
    end
  end

  after :each do
    File.delete(@file) if File.exists?(@file)
    @file = nil
  end
end 

describe "File.writable_real?" do  
  before(:each) do
    @file = '/tmp/i_exist'
  end

  after(:each) do
    File.delete(@file) rescue nil
  end
    
  it "returns true if named file is writable by the real user id of the process, otherwise false" do
    File.writable_real?('fake_file').should == false
    File.open(@file,'w'){
      File.writable_real?(@file).should == true
    }
  end
  
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do  
    should_raise(ArgumentError){ File.writable_real? }
    should_raise(TypeError){ File.writable_real?(1) }
    should_raise(TypeError){ File.writable_real?(nil) }
    should_raise(TypeError){ File.writable_real?(false) }
  end 
end

describe "File.split" do 
  before :each do
    @path_unix             = "/foo/bar/baz.rb"
    @path_windows_backward = "C:\\foo\\bar\\baz.rb"
    @path_windows_forward  = "C:/foo/bar/baz.rb"
  end 
  
  it "should split the given string into a directory and a file component and returns them in a 2 element array" do
    File.split("/rubinius/better/than/ruby").should == ["/rubinius/better/than", "ruby"]
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array. (unix)" do
    File.split(@path_unix).should == ["/foo/bar","baz.rb"]
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array. (edge cases)" do 
    File.split("").should == [".", ""]
    File.split("//foo////").should == ["/", "foo"]  
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array.(windows)" do
    File.split(@path_windows_backward).should ==  [".", "C:\\foo\\bar\\baz.rb"]
  end   
   
  it "should split the given string into a directory and a file component and returns them in a two-element array.(forward slash)" do
    File.split(@path_windows_forward).should == ["C:/foo/bar", "baz.rb"] 
  end
  
  it "should raise an exception if the number of arguments are incorrect or of incorrect type" do
    should_raise(ArgumentError){ File.split }
    should_raise(ArgumentError){ File.split('string', 'another string') }
    should_raise(TypeError){ File.split(1) }
  end
    
  it "should coerce argument to a string if a non string type is given" do
    class C; def to_str; "/rubinius/better/than/ruby"; end; end
    File.split(C.new).should == ["/rubinius/better/than", "ruby"]
  end
end


describe "File.delete" do     
  before :each do     
    @file1 = 'test.txt'
    @file2 = 'test2.txt'
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file2) if File.exists?(@file2)

    File.open(@file1, "w"){} # Touch
    File.open(@file2, "w"){} # Touch
  end 

  it "delete a file " do
    File.delete.should == 0
    File.delete(@file1).should == 1    
    File.exists?(@file1).should == false
  end

  it "should delete multiple files" do
    File.delete(@file1, @file2).should == 2    
    File.exists?(@file1).should == false
    File.exists?(@file2).should == false
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do
    should_raise(TypeError){ File.delete(1) }
  end
  
  it "raise an error when the target file no exists " do 
    should_raise(Errno::ENOENT){ File.delete('bogus') }
  end

  it "delete a file using the alias unlink"  do
    File.unlink(@file1) 
    File.exists?(@file1).should == false
  end  
  
  it "deletes the named files" do
    File.delete(@file1)
    should_raise(Errno::ENOENT){File.open(@file1,"r")}
  end
    
  it "return the number of names passed as arguments (0 and 1 argument)" do
    File.delete.should == 0
    File.delete(@file1).should == 1
  end

  it "return the number of names passed as arguments (multiple arguments)" do
    File.delete(@file1, @file2).should == 2
  end
 
  specify "delete should delete a file and return the number of names passed as arguments" do
    begin
      File.delete(@file1) if File.exists?(@file1)
      File.exists?(@file1).should == false
      File.new(@file1,'w').close
      File.exists?(@file1).should == true
      File.delete(@file1).should == 1
      File.exists?(@file1).should == false
    ensure
      File.delete(@file1) rescue nil
    end
  end  
  
  it "should coerce a given parameter into a string if possible" do
    class Coercable; def to_str; "test.txt"; end; end
    File.delete(Coercable.new).should == 1
  end
  
  after :each do
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file2) if File.exists?(@file2)

    @file1 = nil
    @file2 = nil
  end
end


describe "File.ftype" do
  
  # Identifies the type of the named file; the return string is one of ``file’’, 
  # `directory’’, ``characterSpecial’’, ``blockSpecial’’, ``fifo’’, ``link’’, 
  # `socket’’, or ``unknown’’.
  
  before :each do
    @file = "test.txt"
    @dir  = Dir.pwd
    File.open(@file, "w"){} # Touch
    #@char = Pathname.new(File.null).realpath

    if WINDOWS
      @block_dev = "NUL"
    else
      @fifo = "test_fifo"
      system("mkfifo #{@fifo}") unless File.exists?(@fifo)

      if File.exists?("/dev/fd0")
        @block = Pathname.new("/dev/fd0").realpath
        @link  = "/dev/fd0" if File.symlink?("/dev/fd0")
      elsif File.exists?("/dev/diskette")
        @block = Pathname.new("/dev/diskette").realpath
        @link  = "/dev/diskette" if File.symlink?("/dev/diskette")
      elsif File.exists?("/dev/cdrom")
        @block = Pathname.new("/dev/cdrom").realpath
        @link  = "/dev/cdrom" if File.symlink?("/dev/cdrom")
      elsif File.exists?("/dev/sr0") # CDROM
        @block = Pathname.new("/dev/sr0").realpath
        @link  = "/dev/sr0" if File.symlink?("/dev/sr0") 
      elsif File.exists?("/dev/disk0")
        @block = "/dev/disk0"
        @link  = "/tmp"
      else
        @block = nil
        @link  = nil
      end
    end
  end

  it "return a string " do  
    File.ftype(@file).class.should == String
  end

  it "return 'file' when is a file" do
    File.ftype(@file).should == 'file'
  end

  it "return 'directory' when is a dir" do
    File.ftype(@dir).should == 'directory'
  end

  #   it "return characterSpecial when is a char"  do
  #      File.ftype(@char).should = 'characterSpecial'
  #   end

  it "return blockSpecial when is a block" do
    File.ftype(@block).should == 'blockSpecial'
  end

  it "return link when is a link" do
    File.ftype(@link).should == 'link'
  end

  it "return fifo when is a fifo" do
    File.ftype(@fifo).should == 'fifo'
  end   
  
  it "should return the type of the named file" do
    File.ftype(@file).should == "file"
    File.ftype("/dev/tty").should == "characterSpecial"
    File.ftype("/").should == "directory"
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.ftype }
    should_raise(Errno::ENOENT){ File.ftype('bogus') }
  end

  after :each do 
    File.delete(@fifo)
      
    @file   = nil
    @dir    = nil
    @char   = nil
    @block  = nil
    @fifo   = nil
    @link   = nil
    @socket = nil
  end
end 


# Interestingly MRI 1.8 will fail on later load() calls without this...
# Big WTF. -- flgr
GC.start

File.delete("test.lnk")  if File.exists?("test.lnk") 
File.delete("test_fifo") if File.exists?("test_fifo")
File.delete("test.txt")  if File.exists?("test.txt")
File.delete("test2.txt") if File.exists?("test2.txt")
