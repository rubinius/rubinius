require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/../shared/difference'

describe "SortedSet#-" do
  it_behaves_like :set_difference, SortedSet, :-
end
