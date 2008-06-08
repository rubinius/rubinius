require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/../shared/delete'

describe "SortedSet#delete" do
  it_behaves_like :set_delete, SortedSet
end

describe "SortedSet#delete?" do
  it_behaves_like :set_delete_p, SortedSet
end
