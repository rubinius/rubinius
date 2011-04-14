require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/object_id', __FILE__)

describe "Kernel#__id__" do
  it_behaves_like(:kernel_object_id, :__id__)
end

describe "Kernel#__id__" do
  it "needs to be reviewed for spec completeness"
end
