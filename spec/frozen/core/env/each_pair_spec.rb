require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each.rb'

describe "ENV.each_pair" do
  it_behaves_like(:env_each, :each_pair)
end
