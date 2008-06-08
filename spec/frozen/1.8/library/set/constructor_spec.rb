require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/constructor'

describe "Set.[]" do
  it_behaves_like :set_constructor, Set
end
