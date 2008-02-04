require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#putc" do
  it "raises IOError on closed stream" do
    f = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    f.close
    lambda { f.putc('a') }.should raise_error(IOError)
  end
end
