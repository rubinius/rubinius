describe :file_size, :shared => true do
  before :each do
    @empty = "i_am_empty"
    File.delete @empty if File.exist? @empty
    File.open(@empty,'w') { }

    @exists = tmp('i_exist')
    File.open(@exists,'w') { |f| f.write 'rubinius' }
  end

  after :each do
    File.delete @empty if File.exist? @empty
    File.delete @exists if File.exist? @exists
  end

  it "returns nil if the file has zero size" do
    @object.send(@method, @empty).should == nil
  end

  it "returns the size of the file if it exists and is not empty" do
    @object.send(@method, @exists).should == 8
  end
end

describe :file_size_missing, :shared => true do
  before :each do
    @missing = "i_dont_exist"
    File.delete @missing if File.exists? @missing
  end

  it "returns nil if file_name doesn't exist" do
    @object.send(@method, @missing).should == nil
  end
end
