require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.new" do
  before :each do
    @file = tmp('test.txt')
    @fh = nil
    @flags = File::CREAT | File::TRUNC | File::WRONLY
    File.open(@file, "w") {} # touch
  end

  after :each do
   @fh.close if @fh 
    File.delete(@file) if File.exists?(@file)
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

  it "return a new File with modus num and permissions" do
    File.delete(@file)
    File.umask(0011)
    @fh = File.new(@file, @flags, 0755)
    @fh.class.should == File
    File.stat(@file).mode.to_s(8).should == "100744"
    File.exists?(@file).should == true
  end

  it "creates the file and returns writable descriptor when called with 'w' mode and r-o permissions" do
    # it should be possible to write to such a file via returned descriptior,
    # even though the file permissions are r-r-r.

    File.delete(@file) if File.exists?(@file)
    begin
      f = File.new(@file, "w", 0444)
      lambda { f.puts("test") }.should_not raise_error(IOError)
    ensure
      f.close
    end
    File.exist?(@file).should == true
    File.read(@file).should == "test\n"
  end

  it "opens the existing file, does not change permissions even when they are specified" do
    File.chmod(0664, @file)           # r-w perms
    orig_perms = File.stat(@file).mode.to_s(8)
    begin
      f = File.new(@file, "w", 0444)    # r-o perms, but they should be ignored
      f.puts("test")
    ensure
      f.close
    end
    File.stat(@file).mode.to_s(8).should == orig_perms

    # it should be still possible to read from the file
    File.read(@file).should == "test\n"
  end

  it "return a new File with modus fd " do
    begin
      @fh_orig = File.new(@file)
      @fh = File.new(@fh_orig.fileno)
      @fh.class.should == File
      File.exists?(@file).should == true
    ensure
      @fh.close rescue nil if @fh
      @fh = nil
      @fh_orig.close rescue nil if @fh_orig
      @fh_orig = nil
    end
  end

  it "create a new file when use File::EXCL mode " do
    @fh = File.new(@file, File::EXCL)
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "raises an Errorno::EEXIST if the file exists when create a new file with File::CREAT|File::EXCL" do
    lambda { @fh = File.new(@file, File::CREAT|File::EXCL) }.should raise_error(Errno::EEXIST)
  end

  it "create a new file when use File::WRONLY|File::APPEND mode" do
    @fh = File.new(@file, File::WRONLY|File::APPEND)
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "raises an Errno::EINVAL error with File::APPEND" do
    lambda { @fh = File.new(@file, File::APPEND) }.should raise_error(Errno::EINVAL)
  end

  it "raises an Errno::EINVAL error with File::RDONLY|File::APPEND" do
    lambda { @fh = File.new(@file, File::RDONLY|File::APPEND) }.should raise_error(Errno::EINVAL)
  end

  it "raises an Errno::EINVAL error with File::RDONLY|File::WRONLY" do
    @fh = File.new(@file, File::RDONLY|File::WRONLY)
    @fh.class.should == File
    File.exists?(@file).should == true
  end


  it "create a new file when use File::WRONLY|File::TRUNC mode" do
    @fh = File.new(@file, File::WRONLY|File::TRUNC)
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "coerces filename using to_str" do
    name = mock("file")
    name.should_receive(:to_str).and_return(@file)
    File.new(name, "w") { }
    File.exists?(@file).should == true
  end

  specify  "expected errors " do
    lambda { File.new(true)  }.should raise_error(TypeError)
    lambda { File.new(false) }.should raise_error(TypeError)
    lambda { File.new(nil)   }.should raise_error(TypeError)
    lambda { File.new(-1) }.should raise_error(Errno::EBADF)
    lambda { File.new(@file, File::CREAT, 0755, 'test') }.should raise_error(ArgumentError)
  end

  # You can't alter mode or permissions when opening a file descriptor
  #
  it "can't alter mode or permissions when opening a file" do
    @fh = File.new(@file)
    lambda { File.new(@fh.fileno, @flags) }.should raise_error(Errno::EINVAL)
  end
end
