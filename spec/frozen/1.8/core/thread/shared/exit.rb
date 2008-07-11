shared :thread_exit do |cmd|
  describe "Thread\##{cmd}" do

    # This case occurred in JRuby where native threads are used to provide
    # the same behavior as MRI green threads. Key to this issue was the fact
    # that the thread which called #exit in its block was also being explicitly
    # sent #join from outside the thread. The 100.times provides a certain
    # probability that the deadlock will occur. It was sufficient to reliably
    # reproduce the deadlock in JRuby.
    it "does not deadlock when called from within the thread while being joined from without" do
      100.times do
        t = Thread.new { Thread.stop; Thread.current.send(cmd) }
        Thread.pass until t.status == "sleep"
        t.wakeup.should == t
        t.join.should == t
      end
    end
  end
end
