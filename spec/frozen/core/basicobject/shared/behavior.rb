module BasicObjectBehavior
  def private_features
    [ :initialize, :method_missing, :singleton_method_added, :singleton_method_removed, :singleton_method_undefined ]
  end

  def protected_features
    []
  end

  def public_features
    [ :!, :!=, :==, :__send__, :equal?, :instance_eval, :instance_exec ]
  end
end

module BasicObjectClassBehavior
end
