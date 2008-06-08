require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../shared/add'
require 'set'

describe "SortedSet#add" do
  it_behaves_like :set_add, SortedSet, :add
end

describe "SortedSet#add?" do
  it_behaves_like :set_add_p, SortedSet
end
