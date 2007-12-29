require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/intersection'
require 'set'

describe "Set#intersection" do
  it_behaves_like :set_intersection, :intersection
end

describe "Set#&" do
  it_behaves_like :set_intersection, :&
end
