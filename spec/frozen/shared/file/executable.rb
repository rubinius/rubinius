describe :file_executable, :shared => true do
  before :each do
    @file1 = tmp('temp1.txt')
    @file2 = tmp('temp2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){} # touch

    File.chmod(0755, @file1)
  end

  after :each do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2)

    @file1 = nil
    @file2 = nil
  end

  platform_is_not :windows do
    it "returns true if named file is executable by the effective user id of the process, otherwise false" do
      @object.send(@method, '/etc/passwd').should == false
      @object.send(@method, @file1).should == true
      @object.send(@method, @file2).should == false
    end

    it "return true if the argument is an executable file" do
      @object.send(@method, @file1).should == true
      @object.send(@method, @file2).should == false
    end
  end

  it "raises an ArgumentError if not passed one argument" do
    lambda { @object.send(@method) }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if not passed a String type" do
    lambda { @object.send(@method, 1)     }.should raise_error(TypeError)
    lambda { @object.send(@method, nil)   }.should raise_error(TypeError)
    lambda { @object.send(@method, false) }.should raise_error(TypeError)
  end
end

describe :file_executable_missing, :shared => true do
  it "returns false if the file does not exist" do
    @object.send(@method, 'fake_file').should == false
  end
end
