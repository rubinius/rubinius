require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#binmode" do
  before :each do
    @filename = tmp("IO_binmode_file")
    @file = File.open(@filename, "w")
  end

  after :each do
    @file.close
    File.unlink @filename
  end
  
  ruby_version_is ""..."1.9" do
    ruby_bug "#2046", "1.8.7.174" do
      it "raises an IOError on closed stream" do
        lambda { IOSpecs.closed_file.binmode }.should raise_error(IOError)
      end
    end
  end

  ruby_version_is "1.9" do
    it "raises an IOError on closed stream" do
      lambda { IOSpecs.closed_file.binmode }.should raise_error(IOError)
    end
  end

  # Even if it does nothing in Unix it should not raise any errors.
  it "puts a stream in binary mode" do
    lambda { @file.binmode }.should_not raise_error
  end
end
