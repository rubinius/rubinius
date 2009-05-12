describe :file_blockdev, :shared => true do
  it "returns true/false depending if the named file is a block device" do
    @object.send(@method, tmp("/")).should == false
  end
end
