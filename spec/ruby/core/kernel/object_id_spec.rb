require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/object_id', __FILE__)

describe "Object.object_id" do
  it_behaves_like(:kernel_object_id, :object_id)
end

describe "Kernel#object_id" do
  it "needs to be reviewed for spec completeness"
end
