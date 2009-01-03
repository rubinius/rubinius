require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/object_id'

describe "Object.object_id" do
  it_behaves_like(:kernel_object_id, :object_id) 
end

describe "Kernel#object_id" do
  it "needs to be reviewed for spec completeness"
end
