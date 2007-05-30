class List
  define_fields :count, :first, :last
  ivar_as_index :count => 0, :first => 1, :last => 2
  
  def initialize
    @count = 0
    @first = nil
    @last = nil
  end

  def <<(obj)
    node = Node.new(obj)
    unless @first
      @first = node
    end

    if @last
      @last.next = node
      @last = node
    else
      @last = node
    end

    @count += 1
  end

  def shift
    return nil unless @count > 0
    node = @first
    @first = node.next
    @count -= 1
    return node.object
  end

  def each(&blk)
    node = @first
    while node
      blk.call node.object
      node = node.next
    end

    return nil
  end

  class Node
    define_fields :object, :next
    ivar_as_index :object => 0, :next => 1

    def initialize(obj)
      @object = obj
      @next = nil
    end

    def next=(obj)
      @next = obj
    end
  end
end

