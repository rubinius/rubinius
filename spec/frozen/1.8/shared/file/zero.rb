describe :file_zero, :shared => true do
  before :each do
    @zero_file    = 'test.txt'
    @nonzero_file = 'test2.txt'

    File.open(@zero_file, "w") {} # Touch
    File.open(@nonzero_file, "w") { |f| f.puts "hello" }
  end

  after :each do
    File.delete(@zero_file) if File.exists?(@zero_file)
    File.delete(@nonzero_file) if File.exists?(@nonzero_file)
    @zero_file    = nil
    @nonzero_file = nil
  end

  it "return true if the length of a file its zero, otherwise true" do
    @object.send(@method, @zero_file).should == true
    @object.send(@method, @nonzero_file).should == false
  end

  platform_is :windows do
    it "returns true for NUL" do
      @object.send(@method, 'NUL').should == true
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

  it "zero? should return true if the named file exists and has a zero size." do
    begin
      file = tmp('i_exist')
      File.open(file,'w') { @object.send(@method, file).should == true }
    ensure
      File.delete(file) rescue nil
    end
  end
end

describe :file_zero_missing, :shared => true do
  it "returns false if the file does not exist" do
    @object.send(@method, 'fake_file').should == false
  end
end
