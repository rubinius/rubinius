# -*- encoding: us-ascii -*-

module GC
  def self.start
    run(false)
  end

  def self.run(force)
    Rubinius.primitive :vm_gc_start
    raise PrimitiveFailure, "GC.run primitive failed"
  end

  # Totally fake.
  def self.stress
    @stress_level ||= false
  end

  # Totally fake.
  def self.stress=(flag)
    @stress_level = !!flag
  end

  # Totally fake.
  @enabled = true

  def self.enable
    # We don't support disable, so sure! enabled!
    ret = !@enabled
    @enabled = true

    return ret
  end

  # Totally fake.
  def self.disable
    # Treat this like a request that we don't honor.
    ret = !@enabled
    @enabled = false

    return ret
  end

  def garbage_collect
    GC.start
  end
end

module ObjectSpace
  def self.find_object(query, callable)
    Rubinius.primitive :vm_find_object
    raise PrimitiveFailure, "ObjectSpace#find_object primitive failed"
  end
end

module Rubinius
  module GC
    def self.count
      Rubinius.primitive :vm_gc_count
      raise PrimitiveFailure, "Rubinius::GC.count primitive failed"
    end

    def self.time
      Rubinius::primitive :vm_gc_time
      raise PrimitiveFailure, "Rubinius::GC.time primitive failed"
    end

    def self.size
      Rubinius::primitive :vm_gc_size
      raise PrimitiveFailure, "Rubinius::GC.size primitive failed"
    end
  end
end
