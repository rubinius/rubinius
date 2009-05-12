require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/dup_clone'

describe "Object#dup" do
  it_behaves_like :object_dup_clone, :dup

  it "does not preserve frozen state from the original" do
    o = ObjectSpecDupInitCopy.new
    o.freeze
    o2 = o.dup

    o2.frozen?.should == false
  end
end
