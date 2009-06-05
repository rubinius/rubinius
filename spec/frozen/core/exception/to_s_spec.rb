require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_s'

describe "Exception#to_s" do
  it_behaves_like :to_s, :to_s
end  
