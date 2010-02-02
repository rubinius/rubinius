require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/include.rb'
require File.dirname(__FILE__) + '/shared/key.rb'

describe "ENV.key?" do
  it_behaves_like(:env_include, :key?)
end

describe "ENV.key" do
  ruby_version_is "1.9" do
    it_behaves_like(:env_key, :key)
  end
end