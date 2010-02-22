require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)
require File.expand_path('../shared/local', __FILE__)
require File.expand_path('../shared/time_params', __FILE__)

describe "Time.mktime" do
  it_behaves_like(:time_local, :mktime)
  it_behaves_like(:time_params, :mktime)
end
