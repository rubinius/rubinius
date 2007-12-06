class MyHash < Hash; end
class ToHashHash < Hash
  def to_hash() { "to_hash" => "was", "called!" => "duh." } end
end

module HashSpecs
  def self.empty
    h = {}
    h.freeze
    h
  end
  
  def self.hash
    h = {1 => 2, 3 => 4}
    h.freeze
    h
  end
end
