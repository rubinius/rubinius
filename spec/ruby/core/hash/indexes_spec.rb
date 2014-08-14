require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/values_at', __FILE__)

describe "Hash#indexes" do
  it_behaves_like(:hash_values_at, :indexes)
end
