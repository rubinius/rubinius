require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'
require File.dirname(__FILE__) + '/shared/each'

describe "GetoptLong#each_option" do
  it_behaves_like(:getoptlong_each, :each_option)
end