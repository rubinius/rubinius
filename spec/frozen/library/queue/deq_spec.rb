require File.dirname(__FILE__)+'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/deque'

describe "Queue#deq" do
  it_behaves_like :queue_deq, :deq
end
