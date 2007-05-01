require 'mini_rspec'

def try(a, b=true)
  yield
  return nil
rescue a
  return b
end
