##
# This is used to prevent recursively traversing an object graph.

module RecursionGuard
  def self.inspecting?(obj)
    stack.include?(obj.object_id)
  end

  def self.inspect(obj, &block)
    stack.push(obj.object_id)
    begin
      yield
    ensure
      stack.pop
    end
  end

  def self.stack
    stack = Thread.current[:inspecting] ||= []
  end
end
