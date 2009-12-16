require File.dirname(__FILE__)+'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/enque'

describe "Queue#enq" do
  it_behaves_like :queue_enq, :enq
end
