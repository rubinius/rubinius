require File.dirname(__FILE__) + '/shared/abs'

describe "Float#abs" do
  it_behaves_like(:float_abs, :abs)
end
