shared :set_classify do |klass|
  describe "#{klass}#classify" do
    before(:each) do
      @set = klass["one", "two", "three", "four"]
    end

    it "yields each Object in self" do
      res = []
      @set.classify { |x| res << x }
      res.sort.should == ["one", "two", "three", "four"].sort
    end

    it "raises a LocalJumpError when passed no block" do
      lambda { @set.classify }.should raise_error(LocalJumpError)
    end

    it "classifies the Objects in self based on the block's return value" do
      classified = @set.classify { |x| x.length }
      classified.should == { 3 => klass["one", "two"], 4 => klass["four"], 5 => klass["three"] }
    end
  end
end