require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'
require File.dirname(__FILE__) + '/shared/get'

describe "GetoptLong#get_option" do
  it_behaves_like(:getoptlong_get, :get_option)
end