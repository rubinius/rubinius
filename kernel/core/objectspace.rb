# depends on: module.rb

module ObjectSpace
  def self.each_object(what=nil, &block)
    unless what == Class
      raise ArgumentError, "ObjectSpace doesn't support '#{what}' yet"
    end
    
    emit_subclasses(Object, block)
  end
  
  def self.emit_subclasses(start, prc)
    subs = start.__subclasses__
    return 0 unless subs
    count = 0
    
    subs.each do |c|
      count += 1
      prc.call(c)
      count += emit_subclasses(c, prc)
    end
    
    return count
  end

  # Finalizer support. Uses WeakRef to detect object death.
  # WeakRef uses the GC to do all the real work.

  @finalizers = Hash.new

  def self.define_finalizer(obj, prc=nil, &block)
    prc ||= block

    if prc.nil? or !prc.respond_to?(:call)
      raise ArgumentError, "action must respond to call"
    end

    @finalizers[obj.object_id] = [WeakRef.new(obj), prc]
    return nil
  end

  def self.undefine_finalizer(obj)
    @finalizers.delete(obj.object_id)
  end

  def self.run_finalizers
    @finalizers.each_pair do |key, val|
      unless val[0].weakref_alive?
        @finalizers.delete key
        val[1].call(key)
      end
    end
  end

  def self.garbage_collect
    GC.start
  end

  def self.after_loaded
    # Fire up the Thread that will process finalization
    @thread = Thread.new do
      loop do
        Rubinius::ON_GC.receive
        ObjectSpace.run_finalizers
      end
    end
  end
end

