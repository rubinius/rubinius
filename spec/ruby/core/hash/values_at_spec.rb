require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe :hash_values_at, :shared => true do
  it "returns an array of values for the given keys" do
    h = new_hash(:a => 9, :b => 'a', :c => -10, :d => nil)
    h.send(@method).should be_kind_of(Array)
    h.send(@method).should == []
    h.send(@method, :a, :d, :b).should be_kind_of(Array)
    h.send(@method, :a, :d, :b).should == [9, nil, 'a']
  end
end

describe "Hash#values_at" do
  it_behaves_like(:hash_values_at, :values_at)
end
