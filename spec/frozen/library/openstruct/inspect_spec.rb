require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/inspect"

describe "OpenStruct#inspect" do
  it_behaves_like :ostruct_inspect, :inspect
end
