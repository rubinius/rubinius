require File.dirname(__FILE__)+'/../../spec_helper'
require 'thread'
require File.dirname(__FILE__)+'/shared/enque'

describe "Queue#push" do
  it_behaves_like :queue_enq, :push
end
