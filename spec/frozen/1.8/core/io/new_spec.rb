require "#{File.dirname __FILE__}/../../spec_helper"
require "#{File.dirname __FILE__}/shared/new_shared"
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO.new" do
  it_behaves_like :io_new, :new
end

describe "IO.new" do
  before :all do
    @filename = tmp("rubinius-spec-io-new-#{$$}.txt")
  end

  after :all do
    File.unlink @filename
  end

  before :each do
    @file = File.open @filename, "w"
  end

  after :each do
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "does not execute a block if given one" do
    l = lambda {
      io = IO.new(@file.fileno, 'w') {|io| raise Exception, "N-uh" }
      io.close
    }
    l.should_not raise_error(Exception, "N-uh")
  end

  it "raises IOError on closed stream" do
    lambda { IO.new(IOSpecs.closed_file.fileno, 'w') }.should raise_error(IOError)
  end
  
  it "does not close the stream automatically if given a block" do
    begin
      io = IO.new(@file.fileno, 'w') {|f| puts f.read }
      io.closed?.should == false
      @file.closed?.should == false
    ensure
      io.close
    end
  end

  it "emits a warning if given a block" do
    lambda {
      io = IO.new(@file.fileno, 'w') {|io| puts io.read }
      io.close
    }.should complain(/IO::new.*does not take block.*IO::open.*instead/)
  end
  
  it "accepts only one argument" do
    # By default, IO.new without an arg assumes RO
    @file.close
    @file = File.open @filename, 'r'
    lambda { IO.new(@file.fileno) }.should_not raise_error()
  end
end

describe "IO#initialize" do
  it "needs to be reviewed for spec completeness"
end
