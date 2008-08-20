require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readline" do
  it "raises EOFError on end of stream" do
    testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
    File.open(testfile, 'r') do |f|
      lambda { loop { f.readline } }.should raise_error(EOFError)
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.readline }.should raise_error(IOError)
  end
end
