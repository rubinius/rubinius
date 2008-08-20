require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/union'
require 'set'

describe "SortedSet#union" do
  it_behaves_like :sorted_set_union, :union
end

describe "SortedSet#|" do
  it_behaves_like :sorted_set_union, :|
end
