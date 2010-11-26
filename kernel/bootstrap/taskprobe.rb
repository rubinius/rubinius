class Rubinius::TaskProbe
  def self.allocate
    Ruby.primitive :taskprobe_allocate
    raise PrimtiveFailure, "TaskProbe.allocate primitive failed"
  end

  # Enable +probe+ for this instance. Unless this instance
  # is the currently installed TaskProbe, enabling +probe+
  # will not cause any output. See <tt>TaskProbe.install</tt>
  # and <tt>TaskProbe.enable</tt>.
  def enable(probe)
    Ruby.primitive :taskprobe_enable
    raise PrimtiveFailure, "TaskProbe#enable primitive failed"
  end

  # Disable +probe+ for this instance. See <tt>TaskProbe#enable</tt>
  # and <tt>TaskProbe.disable</tt>.
  def disable(probe)
    Ruby.primitive :taskprobe_disable
    raise PrimtiveFailure, "TaskProbe#disable primitive failed"
  end

  # Returns true if +probe+ is enabled.
  def enabled?(probe)
    Ruby.primitive :taskprobe_enabled_p
    raise PrimitiveFailure, "TaskProbe#enabled? primitive failed"
  end
end
