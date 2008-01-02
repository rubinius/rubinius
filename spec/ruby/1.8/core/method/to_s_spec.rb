require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_s'

describe "Method#to_s" do
  it_behaves_like(:method_to_s, :to_s)
end
