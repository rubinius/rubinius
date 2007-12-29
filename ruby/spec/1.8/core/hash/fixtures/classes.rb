class MyHash < Hash; end
class ToHashHash < Hash
  def to_hash() { "to_hash" => "was", "called!" => "duh." } end
end

module HashSpecs
  def self.empty_frozen_hash
    @empty ||= {}
    @empty.freeze
    @empty
  end
  
  def self.frozen_hash
    @hash ||= {1 => 2, 3 => 4}
    @hash.freeze
    @hash
  end
end
