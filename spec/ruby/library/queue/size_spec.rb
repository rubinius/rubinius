require File.dirname(__FILE__) +'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/length'

describe "Queue#size" do
  it_behaves_like :queue_length, :size
end
