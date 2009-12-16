require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/value.rb'

describe "ENV.has_value?" do
  it_behaves_like(:env_value, :has_value?)
end
