require File.dirname(__FILE__)+'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/deque'

describe "Queue#shift" do
  it_behaves_like :queue_deq, :shift
end
