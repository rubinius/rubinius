shared :enumerable_include do |cmd|
  describe "Enumerable##{cmd}" do     
    it "returns true if any element == argument" do
      class EnumerableSpecIncludeP; def ==(obj) obj == 5; end; end

      elements = (0..5).to_a
      EnumerableSpecs::Numerous.new(*elements).send(cmd,5).should == true 
      EnumerableSpecs::Numerous.new(*elements).send(cmd,10).should == false
      EnumerableSpecs::Numerous.new(*elements).send(cmd,EnumerableSpecIncludeP.new).should == true
    end
    
    it "returns true if any member of enum equals obj when == compare different classes (legacy rubycon)" do
      # equality is tested with ==
      EnumerableSpecs::Numerous.new(2,4,6,8,10).send(cmd, 2.0).should == true
      EnumerableSpecs::Numerous.new(2,4,[6,8],10).send(cmd, [6, 8]).should == true
      EnumerableSpecs::Numerous.new(2,4,[6,8],10).send(cmd, [6.0, 8.0]).should == true
    end
  end
end
