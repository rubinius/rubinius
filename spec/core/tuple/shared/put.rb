shared :tuple_put do |cmd|
  describe "Tuple##{cmd}" do
    it "inserts an element at the specified index" do
      t = Tuple.new(1)
      t.send(cmd, 0, "Whee")
      t.at(0).should == "Whee"
    end

    it "raises InvalidIndex when index is greater than or equal to tuple size" do
      t = Tuple.new(1)
      lambda { t.send(cmd, 1, 'wrong') }.should raise_error(InvalidIndex)
    end
  
    it "raises InvalidIndex when index is less than zero" do
      t = Tuple.new(1)
      lambda { t.send(cmd, -1, 'wrong') }.should raise_error(InvalidIndex)
    end
  end
end
