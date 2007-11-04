shared :enumerable_include do |cmd|
  describe "Enumerable#{cmd}" do     
    # #include? and #member? are aliases, so we only need one function
    # that tests both methods
    
    it "include? should return true if any element == argument" do
      class EnumerableSpecIncludeP; def ==(obj) obj == 5; end; end

      Numerous.new.send(cmd,5).should == true 
      Numerous.new.send(cmd,10).should == false
      Numerous.new.send(cmd,EnumerableSpecIncludeP.new).should == true
    end
  
    it "member? should be a synonym for include?" do
      Numerous.new.send(cmd,5).should ==  Numerous.new.member?(5)
      Numerous.new.send(cmd,10).should ==  Numerous.new.member?(10)
    end
    
    it "Returns true if any member of enum equals obj (legacy rubycon)" do
      EachDefiner.new().send(cmd, 1).should == false
      EachDefiner.new().send(cmd, nil).should == false
      EachDefiner.new(2,4,6,8,10).send(cmd, 2).should == true
      EachDefiner.new(2,4,6,8,10).send(cmd, 3).should == false
      EachDefiner.new(2,4,6,8,10).send(cmd, 4).should == true
      EachDefiner.new(nil).send(cmd, nil).should == true
    end
    
    it "Returns true if any member of enum equals obj when == compare different classes (legacy rubycon)" do
      # equality is tested with ==
      EachDefiner.new(2,4,6,8,10).send(cmd, 2.0).should == true
      EachDefiner.new(2,4,[6,8],10).send(cmd, [6, 8]).should == true
      EachDefiner.new(2,4,[6,8],10).send(cmd, [6.0, 8.0]).should == true
    end
  end
end
