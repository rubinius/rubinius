require 'mspec/runner/mspec'

class Object
  def shared(msg, &block)
    MSpec.store msg.to_sym, block
  end

  def it_behaves_like(behavior, *args)
    p = MSpec.retrieve behavior.to_sym
    p[*args]
  end
end
