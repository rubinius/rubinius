require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#reopen" do
  it "raises IOError on closed stream" do
    File.open(File.dirname(__FILE__) + '/fixtures/gets.txt', 'r') { |f|
      lambda { f.reopen(IOSpecs.closed_file) }.should raise_error(IOError)
    }
  end
end
