require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/include'
require 'set'

describe "SortedSet#include?" do
  it_behaves_like :sorted_set_include, :include?
end
