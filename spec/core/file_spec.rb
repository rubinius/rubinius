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

  specify "expand_path should give a full path" do
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
