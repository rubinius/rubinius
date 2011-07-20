require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Hash#flatten" do
    before(:each) do
      @h = new_hash(:a => 1, :b => [2, 3], :c => 4)
    end

    it "returns an empty Array for an empty Hash" do
      new_hash.flatten.should == []
    end

    it "returns an unflattened Array of keys and values when not passed an argument" do
      @h.flatten.should == [:a, 1, :b, [2, 3], :c, 4]
    end

    it "does not recursively flatten Hash values when not passed an argument" do
      h = new_hash(:c => 3)
      @h[:b] = h
      @h.flatten.should == [:a, 1, :b, h, :c, 4]
    end

    it "recursively flattens Array values when passed 2" do
      @h.flatten(2).should == [:a, 1, :b, 2, 3, :c, 4]
    end

    it "recursively flattens Array values to the given depth" do
      @h[:b] = [[5, [6], 7], 8]
      @h.flatten(3).should == [:a, 1, :b, 5, [6], 7, 8, :c, 4]
    end

    it "raises an TypeError if given a non-Integer argument" do
      lambda { @h.flatten(Object.new) }.should raise_error(TypeError)
    end
  end
end
