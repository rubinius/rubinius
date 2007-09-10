require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#join" do
  it "returns the thread when it is finished" do
    t = Thread.new {}
    t.join.should.equal? t
  end

  it "returns the thread when it is finished when given a timeout" do
    t = Thread.new {}
    t.join
    t.join(0).should.equal? t
  end

  it "returns nil if it is not finished when given a timeout" do
    c = Channel.new
    t = Thread.new { c.receive }
    begin
      t.join(0).should == nil
    ensure
      c << true
    end
  end

  it "raises any exceptions encountered in the thread body" do
    t = Thread.new { raise NotImplementedError.new("Just kidding") }
    should_raise(NotImplementedError) { t.join }
  end
end
