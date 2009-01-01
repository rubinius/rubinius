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
end
