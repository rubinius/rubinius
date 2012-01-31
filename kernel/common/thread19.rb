class Thread
  MUTEX_FOR_THREAD_EXCLUSIVE = Mutex.new

  def Thread.exclusive
    MUTEX_FOR_THREAD_EXCLUSIVE.synchronize { yield }
  end
end
