require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/union'
require 'set'

describe "Set#+" do
  it_behaves_like :set_union, :+
end
