describe :mutex_locked, :shared => true do
  it "returns true if locked" do
    m = Mutex.new
    m.lock
    m.locked?.should be_true
  end

  it "returns false if unlocked" do
    m = Mutex.new
    m.locked?.should be_false
  end

  it "returns the status of the lock" do
    m1 = Mutex.new
    m2 = Mutex.new

    m2.lock # hold th with only m1 locked
    m1_locked = false

    th = Thread.new do
      m1.lock
      m1_locked = true
      m2.lock
    end

    Thread.pass until m1_locked

    m1.locked?.should be_true
    m2.unlock # release th
    th.join

    # There is a race here. While it's true that 'm1' will eventually be
    # lockable (ie will be released by 'th' above), there is not a
    # deterministic synchronization point for this because how long it takes
    # the OS to complete running the underlying 'th' function is not a state
    # that can be controlled.
    #
    # So we test that 'm1' was eventually unlocked by looping.
    true while m1.locked?

    m1.locked?.should be_false
  end
end
