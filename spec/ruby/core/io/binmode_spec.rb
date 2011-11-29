require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#binmode" do
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

  ruby_version_is ""..."1.9" do
    ruby_bug "#2046", "1.8.7.174" do
      it "raises an IOError on closed stream" do
        lambda { IOSpecs.closed_io.binmode }.should raise_error(IOError)
      end
    end
  end

  ruby_version_is "1.9" do
    it "raises an IOError on closed stream" do
      lambda { IOSpecs.closed_io.binmode }.should raise_error(IOError)
    end
  end
  
  describe "read" do
    before :each do
      @file.binmode
      @file.write("PNG\r\n\032\n")
      @file.close
      @file = File.open(@filename, "r")
    end
    
    platform_is :windows do
      it "modifies the line endings without binmode" do
        @file.read.should == "PNG\n\032\n"
      end
    end

    it 'doesn\'t modify \r\n in binmode' do
      @file.binmode
      @file.read.should == "PNG\r\n\032\n"
    end
    
    it "has binmode propagated from IO object dup'ed" do
      @file.binmode
      @duped = @file.dup
      @duped.read.should == "PNG\r\n\032\n"
    end
  end

  platform_is :windows do
    it "modifies line endings when writing without binmode" do
      @file.write("PNG\n\032\n")
      @file.close
      
      @file = File.open(@filename, "r")
      @file.binmode
      @file.read.should == "PNG\r\n\032\r\n"
    end
  end
  
  it "propagates to dup'ed IO objects when writing" do
    @file.binmode
    duped = @file.dup
    duped.write("PNG\r\n\032\n")
    duped.close
    @file.close
    
    @file = File.open(@filename, "r")
    @file.binmode
    @file.read.should == "PNG\r\n\032\n"
  end

  # Even if it does nothing in Unix it should not raise any errors.
  it "puts a stream in binary mode" do
    lambda { @file.binmode }.should_not raise_error
  end
end

ruby_version_is "1.9" do
  describe "IO#binmode?" do
    it "needs to be reviewed for spec completeness"
    
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
