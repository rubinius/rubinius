# -*- encoding: us-ascii -*-

module Enumerable
  def lazy
    Enumerator::Lazy.new(self, enumerator_size) do |yielder, *args|
      yielder.<<(*args)
    end
  end

  def enumerator_size
    Rubinius::Type.object_respond_to?(self, :size) ? size : nil
  end
  private :enumerator_size
end
