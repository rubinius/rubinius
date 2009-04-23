describe :tuple_put, :shared => true do
  it "inserts an element at the specified index" do
    t = Rubinius::Tuple.new(1)
    t.send(@method, 0, "Whee")
    t.at(0).should == "Whee"
  end

  it "raises an Rubinius::ObjectBoundsExceededError when index is greater than or equal to size" do
    t = Rubinius::Tuple.new(1)
    lambda {
      t.send(@method, 1, 'wrong')
    }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an Rubinius::ObjectBoundsExceededError when index is less than zero" do
    t = Rubinius::Tuple.new(1)
    lambda {
      t.send(@method, -1, 'wrong')
    }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
