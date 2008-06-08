shared :set_clear do |klass|
  describe "#{klass}#clear" do
    before(:each) do
      @set = klass["one", "two", "three", "four"]
    end

    it "removes all elements from self" do
      @set.clear
      @set.should be_empty
    end

    it "returns self" do
      @set.clear.should equal(@set)
    end
  end
end