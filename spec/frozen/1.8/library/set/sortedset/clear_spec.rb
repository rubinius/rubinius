require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/../shared/clear'

describe "SortedSet#clear" do
  it_behaves_like :set_clear, SortedSet
end
