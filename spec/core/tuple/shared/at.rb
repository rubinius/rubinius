describe :tuple_at, :shared => true do
  it "retrieves the element at the specified index" do
    t = Rubinius::Tuple.new(3)
    t.put(2, 'three')
    t.send(@method, 2).should == 'three'
  end

  it "raises an Rubinius::ObjectBoundsExceededError when index is greater than or equal to tuple size" do
    t = Rubinius::Tuple.new(1)
    lambda { t.send(@method, 1) }.should raise_error(Rubinius::ObjectBoundsExceededError)
    lambda { t.send(@method, 5) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an InvalidIndexError when index is less than zero" do
    t = Rubinius::Tuple.new(1)
    lambda { t.send(@method, -1) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
