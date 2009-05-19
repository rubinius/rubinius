module ObjectSpace
  class << self
    alias_method :each_object_with_block, :each_object
    def each_object(what = nil, &block)
      return to_enum :each_object, what unless block_given?
      each_object_with_block(what, &block)
    end
  end
end
