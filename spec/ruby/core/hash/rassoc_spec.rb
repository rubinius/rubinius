require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Hash#rassoc" do
    before(:each) do
      @h = new_hash(:a => 1, :b => 2, :c => 1, :d => 3)
    end

    it "returns a 2-element Array if the argument is a value of the Hash" do
      @h.rassoc(2).should == [:b, 2]
    end

    it "only returns the first matching key-value pair" do
      @h.rassoc(1).should == [:a, 1]
    end

    it "uses #== to compare the argument to the values" do
      value = mock("Hash#rassoc value")
      value.should_receive(:==).with(1).and_return(true)

      @h.rassoc(value).should == [:a, 1]
    end

    it "returns nil if the argument is not a value of the Hash" do
      @h.rassoc(4).should be_nil
    end
  end
end
