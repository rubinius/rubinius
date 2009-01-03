require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/value.rb'

describe "ENV.value?" do
  it_behaves_like(:env_value, :value?)
end
