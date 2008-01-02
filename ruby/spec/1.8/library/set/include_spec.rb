require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/include'
require 'set'

describe "Set#include?" do
  it_behaves_like :set_include, :include?
end
