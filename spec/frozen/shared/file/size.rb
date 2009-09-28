describe :file_size, :shared => true do
  before :each do
    @exists = tmp('i_exist')
    File.open(@exists,'w') { |f| f.write 'rubinius' }
  end

  after :each do
    File.delete @exists if File.exist? @exists
  end

  it "returns the size of the file if it exists and is not empty" do
    @object.send(@method, @exists).should == 8
  end

  it "accepts a String-like (to_str) parameter" do
    o = [@exists]
    def o.to_str; self[0]; end

    @object.send(@method, o).should == 8
  end

  ruby_version_is "1.9" do
    it "accepts an object that has a #to_path method" do
      @object.send(@method, mock_to_path(@exists)).should == 8
    end
  end
end

describe :file_size_raise_when_missing, :shared => true do
  before :each do
    @missing = "i_dont_exist"
    File.delete @missing if File.exists? @missing
  end

  it "raises an error if file_name doesn't exist" do
    lambda {@object.send(@method, @missing)}.should raise_error(Errno::ENOENT)
  end
end

describe :file_size_nil_when_missing, :shared => true do
  before :each do
    @missing = "i_dont_exist"
    File.delete @missing if File.exists? @missing
  end

  it "returns nil if file_name doesn't exist or has 0 size" do
     @object.send(@method, @missing).should == nil
  end
end

describe :file_size_0_when_empty, :shared => true do
  before :each do
    @empty = "i_am_empty"
    File.delete @empty if File.exist? @empty
    File.open(@empty,'w') { }
  end

  after :each do
    File.delete @empty if File.exist? @empty
  end

  it "returns 0 if the file is empty" do
    @object.send(@method, @empty).should == 0
  end
end

describe :file_size_nil_when_empty, :shared => true do
  before :each do
    @empty = "i_am_empty"
    File.delete @empty if File.exist? @empty
    File.open(@empty,'w') { }
  end

  after :each do
    File.delete @empty if File.exist? @empty
  end

  it "returns nil if file_name is empty" do
    @object.send(@method, @empty).should == nil
  end
end

describe :file_size_with_file_argument, :shared => true do
  before :each do
    @exists = tmp('i_exist')
    File.open(@exists,'w') { |f| f.write 'rubinius' }
  end

  after :each do
    File.delete @exists if File.exist? @exists
  end

  it "accepts a File argument" do
    @object.send(@method,File.open(@exists)).should == 8
  end
end