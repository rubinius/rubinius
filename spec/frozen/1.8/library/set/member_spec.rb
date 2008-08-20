require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/include'
require 'set'

describe "Set#member?" do
  it_behaves_like :set_include, :member?
end
