require File.dirname(__FILE__) +'/../../spec_helper'
require 'thread'

describe "Queue#num_waiting" do
  it "reports the number of threads waiting on the Queue" do
    q = Queue.new
    threads = []

    5.times do |i|
      q.num_waiting.should == i
      threads << Thread.new { q.deq }
    end

    threads.each { q.enq Object.new }
    threads.each {|t| t.join }
  end
end
