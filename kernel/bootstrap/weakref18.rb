# -*- encoding: us-ascii -*-

class WeakRef
  def respond_to?(method, include_private=false)
    super || __getobj__.respond_to?(method, include_private)
  end
end
