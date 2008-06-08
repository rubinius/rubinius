shared :set_add do |klass, cmd|
  describe "#{klass}##{cmd}" do
    before :each do
      @set = klass.new
    end
    
    it "adds the passed Object to self" do
      @set.send(cmd, "dog")
      @set.should include("dog")
    end
    
    it "returns self" do
      @set.send(cmd, "dog").should equal(@set)
    end
  end
end

shared :set_add_p do |klass|
  describe "#{klass}#add?" do
    before :each do
      @set = klass.new
    end

    it "adds the passed Object to self" do
      @set.add?("cat")
      @set.should include("cat")
    end

    it "returns self when the Object has not yet been added to self" do
      @set.add?("cat").should equal(@set)
    end

    it "returns nil when the Object has already been added to self" do
      @set.add?("cat")
      @set.add?("cat").should be_nil
    end
  end
end
