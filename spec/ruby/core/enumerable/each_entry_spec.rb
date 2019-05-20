require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/enumerable_enumeratorized', __FILE__)

describe "Enumerable#each_entry" do
  before :each do
    ScratchPad.record []
  end

  it "yields multiple arguments as an array" do
    enum = EnumerableSpecs::YieldsMixed2.new
    acc = []
    enum.each_entry {|g| acc << g}.should equal(enum)
    acc.should == EnumerableSpecs::YieldsMixed2.gathered_yields
  end

  it "returns an enumerator if no block" do
    enum = EnumerableSpecs::YieldsMixed2.new
    e = enum.each_entry
    e.should be_an_instance_of(Enumerator)
    e.to_a.should == EnumerableSpecs::YieldsMixed2.gathered_yields
  end

  it "passes through the values yielded by #each_with_index" do
    [:a, :b].each_with_index.each_entry { |x, i| ScratchPad << [x, i] }
    ScratchPad.recorded.should == [[:a, 0], [:b, 1]]
  end

  it "raises an ArgumentError when extra arguments" do
    enum = EnumerableSpecs::YieldsMixed.new
    lambda { enum.each_entry("one").to_a   }.should raise_error(ArgumentError)
    lambda { enum.each_entry("one"){}.to_a }.should raise_error(ArgumentError)
  end

  it "passes extra arguments to #each" do
    enum = EnumerableSpecs::EachCounter.new(1, 2)
    enum.each_entry(:foo, "bar").to_a.should == [1,2]
    enum.arguments_passed.should == [:foo, "bar"]
  end

  it_behaves_like :enumerable_enumeratorized_with_origin_size, :each_entry
end
