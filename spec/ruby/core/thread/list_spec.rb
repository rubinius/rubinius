require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Thread.list" do
  it "includes the current and main thread" do
    Thread.list.should include(Thread.current)
    Thread.list.should include(Thread.main)
  end

  it "includes threads of non-default thread groups" do
    t = Thread.new { sleep }
    ThreadGroup.new.add(t)
    Thread.list.should include(t)
    t.kill
  end

  it "does not include deceased threads" do
    t = Thread.new { 1; }
    t.join
    Thread.list.should_not include(t)
  end

  it "includes waiting threads" do
    running = false
    wait = true
    t = Thread.new { running = true; Thread.pass while wait }
    begin
      Thread.pass until running = true
      Thread.list.should include(t)
    ensure
      wait = false
      t.join
    end
  end
end

describe "Thread.list" do
  it "needs to be reviewed for spec completeness"
end
