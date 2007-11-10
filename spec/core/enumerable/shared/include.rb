shared :enumerable_include do |cmd|
  describe "Enumerable#{cmd}" do     
    it "include? should return true if any element == argument" do
      class EnumerableSpecIncludeP; def ==(obj) obj == 5; end; end

      elements = (0..5).to_a
      Numerous.new(*elements).send(cmd,5).should == true 
      Numerous.new(*elements).send(cmd,10).should == false
      Numerous.new(*elements).send(cmd,EnumerableSpecIncludeP.new).should == true
    end
    
    it "Returns true if any member of enum equals obj when == compare different classes (legacy rubycon)" do
      # equality is tested with ==
      Numerous.new(2,4,6,8,10).send(cmd, 2.0).should == true
      Numerous.new(2,4,[6,8],10).send(cmd, [6, 8]).should == true
      Numerous.new(2,4,[6,8],10).send(cmd, [6.0, 8.0]).should == true
    end
  end
end
