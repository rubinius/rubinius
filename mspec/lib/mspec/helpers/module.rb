class Object
  def labeled_module(name, &block)
    Module.new do
      singleton_class.class_eval {define_method(:to_s) {name}}
      class_eval(&block) if block
    end
  end

  def labeled_class(name, superclass = Object, &block)
    Class.new(superclass) do
      singleton_class.class_eval {define_method(:to_s) {name}}
      class_eval(&block) if block
    end
  end
end