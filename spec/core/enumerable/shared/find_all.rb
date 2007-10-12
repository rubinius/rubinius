@enumerable_find_all = shared "Enumerable#find_all" do |cmd|
  describe "Enumerable##{cmd}" do 
    # #find_all and #select are aliases, so we only need one function
    # that tests both cmds
    
    it "find_all and Select are synonymous" do
      Numerous.new.send(cmd){ |i| i % 2 == 0 }.should == Numerous.new.select { |i| i % 2 == 0 }
      Numerous.new.send(cmd){ |i| i % 2 == 0 }.should == Numerous.new.find_all { |i| i % 2 == 0 }
    end
  
    it "return an array of all elements for which block is not false" do
      Numerous.new.send(cmd) { |i| i % 2 == 0 }.should == [2, 6, 4]
    end

    # TODO: fix this insane mess
    # it "generic_test_find_all(cmd)" do
    #   EachDefiner.new().send(cmd) { |a| true }.should == []
    #   @a.send(cmd) { |a| false }.should == []
    #   @a.send(cmd) { |a| a < 6 }.should == [2,4]
    #   @a.send(cmd) { |a| a == 4 }.should == [4]
    #   @a.send(cmd) { |a| true }.should == @a.arr
    # end
  end
end
