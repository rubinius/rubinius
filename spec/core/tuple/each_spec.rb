require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#each" do
  it "passes each element to the block" do
    t = Rubinius::Tuple[:a, :c, :e, :g]
    ary = []
    t.each { |e| ary << e }
    ary.should == [:a, :c, :e, :g]
  end

  it "does nothing if tuple has zero length" do
    t = Rubinius::Tuple.new(0)
    ary = []
    t.each { |e| ary << e }
    ary.should == []
  end
end
