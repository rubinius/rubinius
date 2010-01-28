describe :io_set_pos, :shared => true do
  before :each do
    @fname = tmp('test.txt')
    File.open @fname, 'w' do |f| f.write "123" end
  end

  after :each do
    File.unlink @fname
  end

  it "sets the offset" do
    File.open @fname do |f|
      val1 = f.read 1
      f.send @method, 0
      f.read(1).should == val1
    end
  end

  it "converts arguments to Integers" do
    File.open @fname do |io|
      o = mock("o")
      o.should_receive(:to_i).and_return(1)

      io.send @method, o
      io.pos.should == 1
    end
  end

  it "does not accept Bignums that don't fit in a C long" do
    File.open @fname do |io|
      lambda { io.send @method, 2**128 }.should raise_error(RangeError)
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.send @method, 0 }.should raise_error(IOError)
  end

end
