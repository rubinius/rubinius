class Class
  def new(*args, &block)
    obj = allocate()
    if block
      obj.initialize(*args, &block)
    else
      obj.initialize(*args)
    end
    obj
  end

  def self.new(sclass=Object, &block)
    obj = allocate()
    obj.superclass = sclass
    obj.class_eval(&block) if block
    obj
  end
end
