describe :file_directory, :shared => true do
  before :each do
    @dir = tmp("file_directory")
    @file = tmp("file_directory.txt")

    mkdir_p @dir
    touch @file
  end

  after :each do
    rm_r @dir, @file
  end

  it "return true if the argument is a directory" do
    @object.send(@method, @dir).should be_true
  end

  it "returns false if the argument is not a directory" do
    @object.send(@method, @file).should be_false
  end

  ruby_version_is "1.9" do
    it "accepts an object that has a #to_path method" do
      @object.send(@method, mock_to_path(@dir)).should be_true
    end
  end

  it "raises a TypeError when passed an Integer" do
    lambda { @object.send(@method, 1) }.should raise_error(TypeError)
    lambda { @object.send(@method, bignum_value) }.should raise_error(TypeError)
  end

  it "raises a TypeError when passed nil" do
    lambda { @object.send(@method, nil) }.should raise_error(TypeError)
  end
end
