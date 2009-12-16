require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/gm'
require File.dirname(__FILE__) + '/shared/time_params'

describe "Time.gm" do
  it_behaves_like(:time_gm, :gm)
  it_behaves_like(:time_params, :gm)
end
