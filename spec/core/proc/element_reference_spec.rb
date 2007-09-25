require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/call_spec'

describe "Proc#[]" do
  it_behaves_like(@proc_call, :[])
end