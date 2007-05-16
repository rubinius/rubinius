module Rubinius
  BOOTSTRAP_HINTS = {
  "MatchData"=>{:@source=>1, :@regexp=>2, :@full=>3, :@region=>4, :@__ivars__=>0},
  "Regexp"=>{:@source=>1, :@names=>3, :@data=>2, :@__ivars__=>0},
  "MethodTable"=>{:@entries=>4, :@default=>5, :@default_proc=>6, :@keys=>1, :@values=>2, :@__ivars__=>0, :@bins=>3},
  "MetaClass"=>{:@methods=>1, :@method_cache=>2, :@constants=>4, :@parent=>5, :@superclass=>6, :@instance_fields=>7, :@attached_instance=>9, :@instance_flags=>8, :@name=>3, :@__ivars__=>0},
  "Class"=>{:@methods=>1, :@method_cache=>2, :@constants=>4, :@parent=>5, :@superclass=>6, :@instance_fields=>7, :@instance_flags=>8, :@name=>3, :@__ivars__=>0},
  "BlockContext"=>{:@raiseable=>5, :@env=>6, :@sender=>1, :@ip=>2, :@sp=>3, :@__ivars__=>0, :@block=>4},
  "Object"=>{:@__ivars__=>0},
  "Float"=>{:@__ivars__=>0},
  "Array"=>{:@total=>0, :@tuple=>1},
  "String"=>{:@bytes=>0, :@characters=>1, :@encoding=>2, :@data=>3},
  "CompiledMethod"=>{:@file=>6, :@required=>2, :@bytecodes=>4, :@locals=>7, :@path=>13, :@serial=>3, :@arguments=>9, :@scope=>10, :@lines=>12, :@cache=>14, :@name=>5, :@literals=>8, :@exceptions=>11, :@__ivars__=>0, :@primitive=>1},
  "SymbolTable"=>{:@symbols=>1, :@strings=>2, :@__ivars__=>0},
  "IO"=>{:@descriptor=>0},
  "Module"=>{:@methods=>1, :@method_cache=>2, :@constants=>4, :@parent=>5, :@name=>3, :@__ivars__=>0},
  "MethodContext"=>{:@raiseable=>5, :@bytecodes=>7, :@locals=>10, :@argcount=>11, :@receiver=>9, :@method=>6, :@sender=>1, :@module=>13, :@ip=>2, :@name=>12, :@sp=>3, :@literals=>8, :@__ivars__=>0, :@block=>4},
  "Hash"=>{:@entries=>4, :@default=>5, :@default_proc=>6, :@keys=>1, :@values=>2, :@__ivars__=>0, :@bins=>3},
  "BlockEnvironment"=>{:@last_ip=>3, :@post_send=>4, :@home=>1, :@__ivars__=>0, :@initial_ip=>2}
}
  BOOTSTRAP_TYPES = {:symtbl=>"SymbolTable", :blank=>"BlankObject", :matchdata=>"MatchData", :array=>"Array", :class=>"Class", :bignum=>"Bignum", :methtbl=>"MethodTable", :floatpoint=>"Float", :blokctx=>"BlockContext", :object=>"Object", :io=>"IO", :module=>"Module", :cmethod=>"CompiledMethod", :metaclass=>"MetaClass", :regexpdata=>"RegexpData", :regexp=>"Regexp", :bytearray=>"ByteArray", :hash=>"Hash", :string=>"String", :tuple=>"Tuple", :blokenv=>"BlockEnvironment", :symbol=>"Symbol", :methctx=>"MethodContext"}
end
