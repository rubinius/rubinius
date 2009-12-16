require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal'

describe "Proc#==" do
  it_behaves_like(:proc_equal, :==)
end
