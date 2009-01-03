require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/collect'

describe "Set#map!" do
  it_behaves_like :set_collect_bang, :map!
end
