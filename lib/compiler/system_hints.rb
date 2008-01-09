class Compiler
  module Bootstrap
    HINTS = {
    :MatchData=>{:@__ivars__=>0, :@source=>1, :@regexp=>2, :@full=>3, :@region=>4},
    :Regexp=>{:@__ivars__=>0, :@source=>1, :@data => 2, :@names=>3},
    :MethodTable=>{:@__ivars__=>0, :@keys=>1, :@values=>2, :@bins=>3, :@entries=>4, :@default=>5, :@default_proc=>6,},
    :MetaClass=>{:@__ivars__=>0, :@methods=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@parent=>5, :@superclass=>6, :@instance_fields=>7, :@has_ivars=>8, :@needs_cleanup=>9, :@object_type=>10, :@attached_instance=>11},
    :Class=>{:@__ivars__=>0, :@methods=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@parent=>5, :@superclass=>6, :@instance_fields=>7, :@has_ivars=>8, :@needs_cleanup=>9, :@object_type=>10},
    :BlockContext=>{},
    :Object=>{:@__ivars__=>0, :@module_names => 1},
    :Float=>{:@__ivars__=>0},
    :Array=>{:@total=>0, :@tuple=>1, :@start => 2, :@shared => 3},
    :String=>{:@bytes=>0, :@characters=>1, :@encoding=>2, :@data=>3, :@hash => 4, :@shared => 5},
    :CompiledMethod=>{:@__ivars__=>0, :@primitive => 1, :@required=>2, :@serial=>3, :@bytecodes=>4, :@name=>5, :@file=>6, :@locals=>7, :@literals=>8, :@arguments=>9, :@scope=>10, :@exceptions=>11, :@lines=>12, :@path=>13, :@cache=>14, :@bonus => 15, :@compiled => 16, :@staticscope => 17},
    :SymbolTable=>{:@__ivars__=>0,:@symbols=>1, :@strings=>2},
    :IO=>{:@__ivars__ => 0, :@descriptor => 1, :@buffer => 2, :mode => 3 },
    :Module=>{:@__ivars__=>0, :@methods=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@parent=>5, :@superclass => 6},
    :MethodContext=>{},
    :Hash=>{:@__ivars__=>0, :@keys=>1, :@values=>2,:@bins=>3, :@entries=>4, :@default=>5, :@default_proc=>6},
    :BlockEnvironment=>{:@__ivars__=>0, :@home=>1, :@initial_ip=>2, :@last_ip=>3, :@post_send=>4, :@home_block => 5, :@local_count => 6, :@bonus => 7, :@method => 8},
    :Exception => {:@__ivars__ => 0, :@message => 1, :@context => 2 },
    :InlineCache => {:@__ivars__ => 0, :@method => 1, :@class => 2, :@module => 3, :@serial => 4, :@hotness => 5, :@trip => 6 },
    :StaticScope => {:@__ivars__ => 0, :@module => 1, :@parent => 2 }
    }

    TYPES = { 
      :symtbl => :SymbolTable, :blank => :BlankObject, :matchdata => :MatchData, 
      :array => :Array, :class => :Class, :bignum => :Bignum, 
      :methtbl => :MethodTable, :floatpoint => :Float, :blokctx => :BlockContext,
      :object => :Object, :io => :IO, :module => :Module, 
      :cmethod => :CompiledMethod, :metaclass => :MetaClass, 
      :regexpdata => :RegexpData, :regexp => :Regexp, :bytearray => :ByteArray, 
      :hash => :Hash, :string => :String, :tuple => :Tuple,
      :blokenv=>:BlockEnvironment, :symbol => :Symbol, 
      :methctx => :MethodContext, :exception => :Exception,
      :icache => :InlineCache, :staticscope => :StaticScope
    }
  end
end
