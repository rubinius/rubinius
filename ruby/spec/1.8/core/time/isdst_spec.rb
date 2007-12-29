require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/isdst'

describe "Time#isdst" do
  it_behaves_like(:time_isdst, :isdst)
end  
