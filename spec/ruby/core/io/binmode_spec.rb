require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#binmode" do
  ruby_bug "#2046", "1.8.7.174" do
    it "raises an IOError on closed stream" do
      lambda { IOSpecs.closed_io.binmode }.should raise_error(IOError)
    end
  end
end

ruby_version_is "1.9" do
  describe "IO#binmode?" do
    before :each do
      @filename = tmp("IO_binmode_file")
      @file = File.open(@filename, "w")
      @duped = nil
    end

    after :each do
      @duped.close if @duped
      @file.close
      File.unlink @filename
    end
    
    it "is true after a call to IO#binmode" do
      @file.binmode?.should be_false
      @file.binmode
      @file.binmode?.should be_true
    end
    
    it "propagates to dup'ed IO objects" do
      @file.binmode
      @duped = @file.dup
      @duped.binmode?.should == @file.binmode?
    end
  end
end
