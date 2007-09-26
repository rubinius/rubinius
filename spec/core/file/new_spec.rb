require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.new" do
  before :each do 
    @file = 'test.txt'
    @fh = nil 
    @flags = File::CREAT | File::TRUNC | File::WRONLY
    File.open(@file, "w") {} # touch
  end

  after :each do   
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
