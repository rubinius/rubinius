class JRubyTargetError < RuntimeError; end

class RubiniusSpecExample; end

module JRubyTarget
  def example(src='', &block)
    raise ArgumentError, "you must pass a block" unless block_given?
    Object.class_eval(src)
    yield
  end
end
