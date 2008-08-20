require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'

describe "Fixnum#%" do
  it_behaves_like(:fixnum_modulo, :%)
end

describe "Fixnum#modulo" do
  it_behaves_like(:fixnum_modulo, :modulo)
end
