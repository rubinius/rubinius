describe :file_identical, :shared => true do
  before :each do
    @file1 = tmp('test.txt')
    @file2 = tmp('test2.txt')
    @file3 = tmp('test.lnk')
    File.delete(@file3) if File.exists?(@file3)

    touch(@file1) { |f| f.puts "file1" }
    touch(@file2) { |f| f.puts "file2" }
    File.link(@file1, @file3)
  end

  after :each do
    File.unlink(@file3)
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file2) if File.exists?(@file2)
  end

  it "return true if they are identical" do
    @object.send(@method, @file1, @file1).should == true
    @object.send(@method, @file1, @file2).should == false
    @object.send(@method, @file1, @file3).should == true
  end

  ruby_version_is "1.9" do
    it "accepts an object that has a #to_path method" do
      @object.send(@method, mock_to_path(@file1), mock_to_path(@file3)).should == true
    end
  end


  it "raises an ArgumentError if not passed two arguments" do
    lambda { @object.send(@method, @file1, @file2, @file3) }.should raise_error(ArgumentError)
    lambda { @object.send(@method, @file1) }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if not passed String types" do
    lambda { @object.send(@method, 1,1) }.should raise_error(TypeError)
  end

  it "returns true if both named files are identical" do
    @object.send(@method, @file1, @file1).should be_true
    @object.send(@method, @file1, @file2).should be_false
  end
end
