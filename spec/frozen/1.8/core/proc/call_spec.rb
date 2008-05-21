require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/call'

describe "Proc#call" do
  it_behaves_like(:proc_call, :call)
end
