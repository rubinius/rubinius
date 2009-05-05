class Array
  # Recursively flatten any contained Arrays into an one-dimensional result.
  def flatten(level=Undefined)
    dup.flatten!(level) || self
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  def flatten!(level=Undefined)
    if level == Undefined
      level = -1
    else
      level = Type.coerce_to(level, Integer, :to_int)
    end
    ret, out = nil, []
    ret = recursively_flatten(self, out, level)
    replace(out) if ret
    ret
  end
end
