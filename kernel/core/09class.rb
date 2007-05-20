class Class
  def new(*args, &block)
    obj = allocate()
    obj.initialize(*args, &block)
    obj
  end
end
