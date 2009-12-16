require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/dup_clone'

describe "Object#clone" do
  it_behaves_like :object_dup_clone, :clone

  it "preserves frozen state from the original" do
    o = ObjectSpecDupInitCopy.new
    o2 = o.clone
    o.freeze
    o3 = o.clone

    o2.frozen?.should == false
    o3.frozen?.should == true
  end
end

