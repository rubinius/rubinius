require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/transpose'

describe "Matrix#transpose" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it_behaves_like(:matrix_transpose, :t)
end
