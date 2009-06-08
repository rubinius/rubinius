describe :kernel_object_id, :shared => true do
  # #object_id and #__id__ are aliases, so we only need one function
  # that tests both methods
  it "returns an integer" do
    mock('fixnum').send(@method).class.should == Fixnum
    nil.send(@method).class.should == Fixnum
  end

  it "returns the same value on all calls to id for a given object" do
    o1 = mock('x')
    o1.send(@method).should == o1.send(@method)
  end

  it "returns different values for different objects" do
    o1 = mock('o1')
    o2 = mock('o2')
    o1.send(@method).should_not == o2.send(@method)
  end

  it "returns the same value for two Fixnums with the same value" do
    o1 = 1
    o2 = 1
    o1.send(@method).should == o2.send(@method)
  end

  it "returns the same value for two Symbol literals" do
    o1 = :hello
    o2 = :hello
    o1.send(@method).should == o2.send(@method)
  end

  it "returns the same value for two true literals" do
    o1 = true
    o2 = true
    o1.send(@method).should == o2.send(@method)
  end

  it "returns the same value for two false literals" do
    o1 = false
    o2 = false
    o1.send(@method).should == o2.send(@method)
  end

  it "returns the same value for two nil literals" do
    o1 = nil
    o2 = nil
    o1.send(@method).should == o2.send(@method)
  end

  it "returns a different value for two Bignum literals" do
    o1 = 2e100.to_i
    o2 = 2e100.to_i
    o1.send(@method).should_not == o2.send(@method)
  end

  it "returns a different value for two Float literals" do
    o1 = 1.0
    o2 = 1.0
    o1.send(@method).should_not == o2.send(@method)
  end

  it "returns a different value for two String literals" do
    o1 = "hello"
    o2 = "hello"
    o1.send(@method).should_not == o2.send(@method)
  end

  it "returns a different value for an object and its dup" do
    o1 = mock("object")
    o2 = o1.dup
    o1.send(@method).should_not == o2.send(@method)
  end
end
