require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/to_i'

describe "Time#to_i" do
  it_behaves_like(:time_to_i, :to_i)
end
