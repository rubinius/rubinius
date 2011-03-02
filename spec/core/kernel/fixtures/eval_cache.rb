# DO NOT CHANGE THIS.

_uniq = var = KernelSpecs.objectifier do |var|
  var.class.name
end

# The reread is important to test get as well as set
"#{_uniq}:#{var}"
