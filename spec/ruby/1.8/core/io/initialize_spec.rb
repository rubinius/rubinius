require File.dirname(__FILE__) + '/../../spec_helper'

describe 'IO#ininitialize' do

  before :each do
    @io = IO.allocate
  end

  it 'should initialize when fd is a Fixnum' do
    lambda { @io.send :initialize, $stdout.fileno }.should_not raise_error(TypeError)
  end

  it 'should initialize when fd responds to #to_int' do
    obj = Object.new
    def obj.to_int() $stdout.fileno end

    lambda { @io.send :initialize, obj }.should_not raise_error(TypeError)
  end

  it 'should throw an exception when fd is an IO' do
    lambda { @io.send :initialize, $stdout }.should raise_error(TypeError)
  end

  it 'should throw an exception when given a non-integer' do
    lambda { @io.send :initialize, @fname }.should raise_error(TypeError)
  end

  it 'should throw an exception when given a bad file descriptor' do
    lambda { @io.send :initialize, -1 }.should raise_error(Errno::EBADF)
  end

end

