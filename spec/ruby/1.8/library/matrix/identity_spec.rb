require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/identity'

describe "Matrix.identity" do
  it_behaves_like(:matrix_identity, :identity)
end
