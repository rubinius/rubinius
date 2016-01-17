require File.expand_path('../../../spec_helper', __FILE__)

describe "Hash#dig" do
  it "returns the nested value specified by the sequence of keys" do
    h = new_hash(foo: new_hash(bar: new_hash(baz: 1)))

    h.dig(:foo, :bar, :baz).should == 1
  end

  it "returns the nested value specified if the sequence includes an index" do
    h = new_hash(foo: [1, 2, 3])

    h.dig(:foo, 2).should == 3
  end

  it "returns nil if any intermediate step is nil" do
    h = new_hash(foo: new_hash(bar: new_hash(baz: 1)))

    h.dig(:foo, :zot, :xyz).should == nil
  end

  it "raises a TypeError if any intermediate step does not respond to #dig" do
    h = new_hash(foo: 1)

    lambda { h.dig(:foo, 3) }.should raise_error(TypeError)
  end

  it "raises an ArgumentError if no arguments provided" do
    h = {}

    lambda { h.dig }.should raise_error(ArgumentError)
  end

  it "calls #dig on any intermediate step with the rest of the sequence as arguments" do
    o = Object.new
    h = new_hash(foo: o)

    def o.dig(*args)
      {dug: args}
    end

    h.dig(:foo, :bar, :baz).should == {dug: [:bar, :baz]}
  end
end
