class BeComputedByMatcher
  def initialize(sym)
    @method = sym
  end

  def matches?(array)
    array.each do |line|
      @receiver = line.shift
      @value = line.pop
      @arguments = line
      return false unless @receiver.send(@method, *@arguments) == @value
    end

    return true
  end

  def method_call
    method_call = "#{@receiver.inspect}.#{@method}"
    unless @arguments.empty?
      method_call << " from #{@arguments.map { |x| x.inspect }.join(", ")}"
    end
    method_call
  end

  def failure_message
    ["Expected #{@value}", "to be computed by #{method_call}"]
  end
end

class Object
  def be_computed_by(sym)
    BeComputedByMatcher.new(sym)
  end
end
