require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/length'
require 'set'

describe "SortedSet#length" do
  it_behaves_like :sorted_set_length, :length
end
