module ObjectSpace
  # HACK disable finalizers
  #@thread = Thread.new do
  #  loop do
  #    Rubinius::ON_GC.receive
  #    ObjectSpace.run_finalizers
  #  end
  #end
end
