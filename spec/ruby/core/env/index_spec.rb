require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/key.rb'

describe "ENV.index" do
  it_behaves_like(:env_key, :index)
end