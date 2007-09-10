require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#indices" do
  it "is a DEPRECATED synonyms for values_at" do
    h = {:a => 9, :b => 'a', :c => -10, :d => nil}
    h.indices(:a, :d, :b).should == h.values_at(:a, :d, :b)
    h.indices().should == h.values_at()
  end
end
