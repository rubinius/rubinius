require 'mini_rspec'

def try(a, b=true)
  yield
  return nil
rescue a
  return b
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
