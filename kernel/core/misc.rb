# depends on: object.rb
def self.to_s
  "main"
end

module Rubinius
  AtExit = []
end

class NilClass
  alias_method :|, :^

  def call(*a)
    raise LocalJumpError, "not callable"
  end
end

NIL = nil

class TrueClass
  alias_method :inspect, :to_s
end

TRUE = true

class FalseClass
  alias_method :|, :^
  alias_method :inspect, :to_s
end

FALSE = false

Undefined = Object.new

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
