shared :tuple_at do |cmd|
  describe "Tuple##{cmd}" do
    it "retrieves the element at the specified index" do
      t = Tuple.new(3)
      t.put(2, 'three')
      t.send(cmd, 2).should == 'three'
    end

    it "raises an InvalidIndexError when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      lambda { t.send(cmd, 1) }.should raise_error(InvalidIndexError)
      lambda { t.send(cmd, 5) }.should raise_error(InvalidIndexError)
    end
  
    it "raises an InvalidIndexError when index is less than zero" do
      t = Tuple.new(1)
      lambda { t.send(cmd, -1) }.should raise_error(InvalidIndexError)
    end
  end
end
