require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/new', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO.open" do
  it_behaves_like :io_new, :open
end

describe "IO.open" do
  before :each do
    @name = tmp("io_open.txt")
    @fd = new_fd @name
  end

  after :each do
    @io.close if @io and !@io.closed?
    rm_r @name
  end

  it "calls #close after yielding to the block" do
    IO.open(@fd, "w") do |io|
      io.should_receive(:close)
      io.closed?.should be_false
    end
  end

  it "propagates an exception raised by #close that is not a StandardError" do
    lambda do
      IO.open(@fd, "w") do |io|
        io.should_receive(:close).and_raise(Exception.new)
      end
    end.should raise_error(Exception)
  end

  it "does not propagate a StandardError raised by #close" do
    IO.open(@fd, "w") do |io|
      io.should_receive(:close).and_raise(StandardError.new)
    end
  end
end
