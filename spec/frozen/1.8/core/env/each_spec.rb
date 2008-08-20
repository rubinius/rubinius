require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each.rb'

describe "ENV.each" do
  it_behaves_like(:env_each, :each)
end
