require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/inspect"

describe "OpenStruct#to_s" do
  it_behaves_like :ostruct_inspect, :to_s
end