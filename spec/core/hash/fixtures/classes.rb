class MyHash < Hash; end
class ToHashHash < Hash
  def to_hash() { "to_hash" => "was", "called!" => "duh." } end
end

module HashSpecs
  def self.empty
    @empty ||= {}
    @empty.freeze
    @empty
  end
  
  def self.hsh
    @hash ||= {1 => 2, 3 => 4}
    @hash.freeze
    @hash
  end
end
