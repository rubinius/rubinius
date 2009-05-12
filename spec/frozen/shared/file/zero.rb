describe :file_zero, :shared => true do
  before :each do
    @zero_file    = 'test.txt'
    @nonzero_file = 'test2.txt'
    @dir = File.dirname(__FILE__)

    File.open(@zero_file, "w") {|f| @zero_fh = f} # Touch
    File.open(@nonzero_file, "w") { |f| f.puts "hello"; @nonzero_fh = f }
  end

  after :each do
    @zero_fh.close unless @zero_fh.closed?
    @nonzero_fh.close unless @nonzero_fh.closed?
    File.delete(@zero_file) if File.exists?(@zero_file)
    File.delete(@nonzero_file) if File.exists?(@nonzero_file)
    @zero_file    = nil
    @zero_fh    = nil
    @nonzero_file = nil
    @nonzero_fh = nil
  end

  it "returns true if the file is empty" do
    @object.send(@method, @zero_file).should == true
  end

  it "returns false if the file is not empty" do
    @object.send(@method, @nonzero_file).should == false
  end

  platform_is :windows do
    it "returns true for NUL" do
      @object.send(@method, 'NUL').should == true
      @object.send(@method, 'nul').should == true
    end
  end

  platform_is_not :windows do
    it "returns true for /dev/null" do
      @object.send(@method, '/dev/null').should == true
    end
  end

  it "raises an ArgumentError if not passed one argument" do
    lambda { File.zero? }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if not passed a String type" do
    lambda { @object.send(@method, nil)   }.should raise_error(TypeError)
    lambda { @object.send(@method, true)  }.should raise_error(TypeError)
    lambda { @object.send(@method, false) }.should raise_error(TypeError)
  end

  it "returns true inside a block opening a file if it is empty" do
    File.open(@zero_file,'w') do
      @object.send(@method, @zero_file).should == true
    end
  end

  platform_is_not :windows do
    it "returns false for a directory" do
      @object.send(@method, @dir).should == false
    end
  end

  platform_is :windows do
    ruby_bug("redmine #449", "1.8.6") do
      it "returns false for a directory" do
        @object.send(@method, @dir).should == false
      end
    end
  end
end

describe :file_zero_missing, :shared => true do
  it "returns false if the file does not exist" do
    @object.send(@method, 'fake_file').should == false
  end
end
