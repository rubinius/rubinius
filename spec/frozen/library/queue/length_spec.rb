require File.dirname(__FILE__)+'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/length'

describe "Queue#length" do
  it_behaves_like :queue_length, :length
end
