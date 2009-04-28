describe :thread_wakeup, :shared => true do
  it "is not queued" do
    exit_loop = false
    after_sleep1 = false
    after_sleep2 = false
    t = Thread.new do
      loop do
        if exit_loop == true
          break
        end
      end
      
      sleep
      after_sleep1 = true
      
      sleep
      after_sleep2 = true
    end

    10.times { t.send(@method); Thread.pass } # These will all get ignored because the thread is not sleeping yet

    exit_loop = true
    
    Thread.pass while t.status and t.status != "sleep"
    after_sleep1.should == false # t should be blocked on the first sleep
    t.send(@method)

    Thread.pass while after_sleep1 != true
    Thread.pass while t.status and t.status != "sleep"
    after_sleep2.should == false # t should be blocked on the second sleep
    t.send(@method)
    Thread.pass while after_sleep2 != true
    
    t.join
  end

  it "does not result in a deadlock" do
    t = Thread.new do
      1000.times {Thread.stop }
    end

    while(t.status != false) do
      t.send(@method)
      Thread.pass
    end
    
    1.should == 1 # test succeeds if we reach here
  end
end
