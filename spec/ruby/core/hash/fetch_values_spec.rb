require File.expand_path('../../../spec_helper', __FILE__)

describe "Hash#fetch_values" do
  before do
    @h = new_hash(foo: 1, bar: 2, baz: 3)
  end

  it "returns an array containing the values associated with the given keys" do
    @h.fetch_values(:foo, :baz).should == [1, 3]
  end

  context "when one of the given keys does not exist" do
    context "when no block is given" do
      it "raises a KeyError" do
        lambda { @h.fetch_values(:foo, :foobar) }.should raise_error(KeyError)
      end
    end

    context "when a block is given" do
      it "returns the value of the block for the missing key" do
        @h.fetch_values(:foo, :foobar) { |k| k.length }.should == [1, 6]
      end
    end
  end

  context "when called with no arguments" do
    it "returns an empty array" do
      @h.fetch_values.should == []
    end
  end
end
