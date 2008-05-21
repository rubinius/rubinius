require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/identity'

describe "Matrix.unit" do
  it_behaves_like(:matrix_identity, :unit)
end
