class Proc::Method < Proc
  def self.__from_method__(meth)
    obj = allocate()
    obj.bound_method = meth

    return obj
  end
end
