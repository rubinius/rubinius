class List
  define_fields :count, :first, :last

  class Node
    define_fields :object, :next
  end
end
