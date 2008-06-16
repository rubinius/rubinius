require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/collect'

describe "SortedSet#collect!" do
  it_behaves_like :sorted_set_collect_bang, :collect!
end
