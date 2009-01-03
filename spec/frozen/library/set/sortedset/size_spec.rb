require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/length'
require 'set'

describe "SortedSet#size" do
  it_behaves_like :sorted_set_length, :size
end
