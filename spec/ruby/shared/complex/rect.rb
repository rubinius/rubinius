require File.expand_path('../../../spec_helper', __FILE__)

describe :complex_rect, :shared => true do
  before(:each) do
    @numbers = [
      Complex(1),
      Complex(0, 20),
      Complex(0, 0),
      Complex(0.0),
      Complex(9999999**99),
      Complex(-20),
      Complex.polar(76, 10)
    ]
  end

  it "returns an Array" do
    @numbers.each do |number|
      number.send(@method).should be_an_instance_of(Array)
    end
  end

  it "returns a two-element Array" do
    @numbers.each do |number|
      number.send(@method).size.should == 2
    end
  end

  it "returns the real part of self as the first element" do
   @numbers.each do |number|
     number.send(@method).first.should == number.real
   end
  end

  it "returns the imaginary part of self as the last element" do
     @numbers.each do |number|
       number.send(@method).last.should == number.imaginary
     end
  end

  it "raises an ArgumentError if given any arguments" do
    @numbers.each do |number|
      lambda { number.send(@method, number) }.should raise_error(ArgumentError)
    end
  end
end
