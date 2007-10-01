require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/call_spec.rb'

describe "Method#[]" do
  it_behaves_like(@method_call, :[])
end
