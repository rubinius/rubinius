class WeakRef
  def self.create_weakref(obj)
    Ruby.primitive :make_weak_ref
  end
end
