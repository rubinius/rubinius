require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/begin', __FILE__)

describe "Range#first" do
  it_behaves_like(:range_begin, :first)

  ruby_version_is "1.9" do
    it "returns the first count elements if given a count" do
      (0..2).first(2).should == [0, 1]
    end

    it "returns an empty array when passed count on an empty range" do
      (0...0).first(0).should == []
      (0...0).first(1).should == []
      (0...0).first(2).should == []
    end

    it "returns an empty array when passed count == 0" do
      (0..2).first(0).should == []
    end

    it "returns an array containing the first element when passed count == 1" do
      (0..2).first(1).should == [0]
    end

    it "raises an ArgumentError when count is negative" do
      lambda { (0..2).first(-1)}.should raise_error(ArgumentError)
    end

    it "returns the entire array when count > length" do
      (0..2).first(4).should == [0, 1, 2]
    end

    it "tries to convert the passed argument to an Integer using #to_int" do
      obj = mock('to_int')
      obj.should_receive(:to_int).and_return(2)
      (0..2).first(obj).should == [0, 1]
    end

    it "raises a TypeError if the passed argument is not numeric" do
      lambda { (0..2).first(nil) }.should raise_error(TypeError)
      lambda { (0..2).first("a") }.should raise_error(TypeError)

      obj = mock("nonnumeric")
      lambda { (0..2).first(obj) }.should raise_error(TypeError)
    end
  end
end
