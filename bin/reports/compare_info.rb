# isolate the reporting from other crapola
require File.dirname(__FILE__) + '/compare_tools'
include CompareTools

info_methods = CompareTools.public_instance_methods - ['mri_classes', 'rubinius_classes']
info = info_methods.inject({}) do |info, meth|
  info.merge( meth.to_sym => send(meth) )
end
p info
