require 'mini_rspec'

def try(a, b=true)
  yield
  return nil
rescue a
  return b
end

class Object
  def should_include(other)
    unless self.include?(other)
      raise Exception.new("Expected " + self.inspect + " to include " + other.inspect)
    end
  end

  def should_be_close(value, tolerance)
    unless (value - self).abs <= tolerance
      raise Exception.new("Expected " + self.inspect + " to be close to " + value.inspect)
    end
  end
end

def should_raise(exception_class = Exception)
  begin
    yield
  rescue Exception => e
    if e.is_a? exception_class
      return true
    end
  end

  raise ArgumentError.new("Did not raise specified exception")
end
