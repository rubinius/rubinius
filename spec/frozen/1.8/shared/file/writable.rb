describe :file_writable, :shared => true do
  before :each do
    @file = tmp('i_exist')
  end

  after :each do
    File.delete(@file) if File.exists?(@file)
  end

  it "returns true if named file is writable by the effective user id of the process, otherwise false" do
    @object.send(@method, '/etc/passwd').should == false
    File.open(@file,'w') { @object.send(@method, @file).should == true }
  end
end

describe :file_writable_missing, :shared => true do
  it "returns false if the file does not exist" do
    @object.send(@method, 'fake_file').should == false
  end
end
