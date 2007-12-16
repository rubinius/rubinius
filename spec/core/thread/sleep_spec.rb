require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

extension :rubinius do
  describe "Thread.sleep" do
    it "pauses execution for approximately the duration requested" do
      start = Time.now
      Thread.sleep 0.1
      (Time.now - start).should be_close(0.1, 0.1)
    end

    it "returns the rounded number of seconds asleep" do
      Thread.sleep(0.1).should be_kind_of(Integer)
    end

    it "raises a TypeError when passed a non-numeric duration" do
      lambda { Thread.sleep(nil)   }.should raise_error(TypeError)
      lambda { Thread.sleep('now') }.should raise_error(TypeError)
      lambda { Thread.sleep('2')   }.should raise_error(TypeError)
    end

    it "pauses execution indefinitely if not given a duration" do
      flag = nil
      t = Thread.new do
        Thread.sleep
        flag = 5
      end

      flag.should == nil
      Thread.pass until t.status == 'sleep'
      flag.should == nil
      t.run
      flag.should == 5
    end
  end
end
