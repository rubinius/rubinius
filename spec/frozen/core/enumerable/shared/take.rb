describe :enumerable_take, :shared => true do
  before :each do
    @values = [4,3,2,1]
    @enum = EnumerableSpecs::Numerous.new(*@values)
  end

  ruby_version_is '1.8.7' do
    it "returns the first count elements if given a count" do
      @enum.send(@method, 2).should == [4, 3]
    end

    it "returns an empty array when passed count on an empty array" do
      empty = EnumerableSpecs::Empty.new
      empty.send(@method, 0).should == []
      empty.send(@method, 1).should == []
      empty.send(@method, 2).should == []
    end

    it "returns an empty array when passed count == 0" do
      @enum.send(@method, 0).should == []
    end

    it "returns an array containing the first element when passed count == 1" do
      @enum.send(@method, 1).should == [4]
    end

    it "raises an ArgumentError when count is negative" do
      lambda { @enum.send(@method, -1) }.should raise_error(ArgumentError)
    end

    it "returns the entire array when count > length" do
      @enum.send(@method, 10).should == @values
    end

    it "tries to convert the passed argument to an Integer using #to_int" do
      obj = mock('to_int')
      obj.should_receive(:to_int).and_return(3).at_most(:twice) # called twice, no apparent reason. should we care?
      @enum.send(@method, obj).should == [4, 3, 2]
    end

    it "raises a TypeError if the passed argument is not numeric" do
      lambda { @enum.send(@method, nil) }.should raise_error(TypeError)
      lambda { @enum.send(@method, "a") }.should raise_error(TypeError)

      obj = mock("nonnumeric")
      lambda { @enum.send(@method, obj) }.should raise_error(TypeError)
    end

  end
end