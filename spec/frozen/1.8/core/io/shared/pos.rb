describe :io_pos, :shared => true do
  before :each do
    @fname = 'test.txt'
    File.open @fname, 'w' do |f| f.write "123" end
  end

  after :each do
    File.unlink @fname
  end

  it "gets the offset" do
    File.open @fname do |f|
      f.send(@method).should == 0
      f.read 1
      f.send(@method).should == 1
      f.read 2
      f.send(@method).should == 3
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.send(@method) }.should raise_error(IOError)
  end

  it "resets #eof?" do
    open @fname do |io|
      io.read 1
      io.read 1
      io.send(@method)
      io.eof?.should == false
    end
  end
end
