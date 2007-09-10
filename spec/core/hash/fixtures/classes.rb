class MyHash < Hash; end
class ToHashHash < Hash
  def to_hash() { "to_hash" => "was", "called!" => "duh." } end
end

empty = {}
empty.freeze

hash = {1 => 2, 3 => 4}
hash.freeze
