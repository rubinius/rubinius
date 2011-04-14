require File.expand_path('../../../spec_helper', __FILE__)

describe "Range#step" do
  it "passes each nth element to the block" do
    a = []
    (-5..5).step(2) { |x| a << x }
    a.should == [-5, -3, -1, 1, 3, 5]

    a = []
    ("A".."F").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]

    a = []
    ("A"..."G").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]

    a = []
    (0.5..2.4).step(0.5) { |x| a << x }
    a.should == [0.5, 1, 1.5, 2]
  end

  it "raises an ArgumentError if stepsize is 0 or negative" do
    lambda { (-5..5).step(0) { |x| x }       }.should raise_error(ArgumentError)
    lambda { (-5.5..5.7).step(0.0) { |x| x } }.should raise_error(ArgumentError)
    lambda { (-5..5).step(-2) { |x| x }      }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if the first element does not respond to #succ" do
    b = mock('x')
    (a = mock('1')).should_receive(:<=>).with(b).and_return(1)
    a.should_not respond_to(:succ)

    lambda { (a..b).step(1) { |i| i } }.should raise_error(TypeError)
  end

  it "returns self" do
    (1..10).step(1) {}.should == (1..10)
  end

  it "raises TypeError if the argument is non-numeric" do
    obj = mock("mock")
    lambda { (1..10).step(obj) {} }.should raise_error(TypeError)
  end

  it "coerces the argument to integer by invoking to_int" do
    (obj = mock("2")).should_receive(:to_int).and_return(2)
    res = []
    (1..10).step(obj) {|x| res << x}
    res.should == [1, 3, 5, 7, 9]
  end

  ruby_version_is "1.8.7" do
    it "returns an enumerator when no block given" do
      enum = (1..10).step(4)
      enum.should be_an_instance_of(enumerator_class)
      enum.to_a.should == [1, 5, 9]
    end
  end

end
