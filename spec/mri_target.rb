require 'test/unit'

class MRITarget < Test::Unit::TestCase
  def example(&block)
    yield
  end
end
