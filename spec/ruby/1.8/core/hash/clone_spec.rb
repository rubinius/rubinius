require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#clone" do
  it "clones" do
    hash = { 'key' => 'value' }

    clone = hash.clone

    clone.should == hash
    clone.object_id.should_not == hash.object_id
  end
end

