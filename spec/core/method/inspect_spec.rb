require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/to_s_spec'

describe "Method#inspect" do
  it_behaves_like(@method_to_s, :inspect)
end
