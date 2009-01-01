require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.umask" do
  before :each do
    @orig_umask = File.umask
    @file = 'test.txt'
    File.open(@file, 'w') {}
  end

  after :each do
    File.delete(@file) if File.exists?(@file)
    @file = nil
    File.umask(@orig_umask)
  end

  it "return a Fixnum" do
    File.umask.class.should == Fixnum
  end

  it "umask should return the current umask value for the process" do
    File.umask(022)
    File.umask(006).should == 022
    File.umask.should == 006
  end

  it "invokes to_int on non-integer argument" do
    (obj = mock(022)).should_receive(:to_int).any_number_of_times.and_return(022)
    File.umask(obj)
    File.umask(obj).should == 022
  end

  it "always succeeds with any integer values" do
    vals = [-2**30, -2**16, -2**8, -2,
      -1.5, -1, 0.5, 0, 1, 2, 7.77777, 16, 32, 64, 2**8, 2**16, 2**30]
    vals.each { |v|
      lambda { File.umask(v) }.should_not raise_error
    }
  end

  it "raises ArgumentError when more than one argument is provided" do
    lambda { File.umask(022, 022) }.should raise_error(ArgumentError)
  end

  platform_is :windows do
    it "Returns the current umask value for this process. (basic)" do
      File.umask.should == 0
    end

    # The value used here is the value of _S_IWRITE.
    it "Returns the current umask value for this process." do
      File.umask(0000200)
      File.umask.should == 0000200
    end

    # FIXME: wtf?
    it "raises an exception if the arguments are wrong type or are the incorect number of arguments " do
      File.umask(0006)
      File.umask.should == 0
    end
  end
end
