require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'

describe "Float#%" do
  it_behaves_like(:float_modulo, :%)
end

describe "Float#modulo" do
  it_behaves_like(:float_modulo, :modulo)
end
