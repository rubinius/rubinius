# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Numeric
  def step(limit, step=1)
    raise ArgumentError, "step cannot be 0" if step == 0
    idx = self
    if idx.is_a?(Float) || limit.is_a?(Float) || step.is_a?(Float)
      idx, limit, step = FloatValue(idx), FloatValue(limit), FloatValue(step)
    end
    cmp = step > 0 ? :> : :<
    until (idx.send(cmp,limit))
      yield(idx)
      idx += step
    end
    return self
  end
end
