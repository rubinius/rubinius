require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/to_i', __FILE__)

describe "Integer#round" do
  it_behaves_like(:integer_to_i, :round)
end
