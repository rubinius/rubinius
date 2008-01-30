require "#{File.dirname __FILE__}/../../spec_helper"
require "#{File.dirname __FILE__}/shared/new_shared"

describe "IO.new" do
  it_behaves_like :io_new, :new
end

describe "IO.new" do
  it "does not execute a block if given one" do
    l = lambda { IO.new @file.fileno, 'w') {|io| raise Exception, "N-uh" } }
    l.should_not raise_error(Exception, "N-uh")
  end

  it "does not close the stream automatically if given a block" do
    io = IO.new(@file.fileno, 'w') {|f| puts f.read }

    io.closed?.should == false
    @file.closed?.should == false
  end

  it "emits a warning if given a block" do
    lambda {
      IO.new(@file.fileno, 'w') {|io| puts io.read }
    }.should complain(/IO::new.*does not take block.*IO::open.*instead/)
  end
end
