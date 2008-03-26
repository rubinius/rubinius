class Compiler
  module Bootstrap
    HINTS = {
    :MatchData=>{:@__ivars__=>0, :@source=>1, :@regexp=>2, :@full=>3, :@region=>4},
    :Regexp=>{:@__ivars__=>0, :@source=>1, :@data => 2, :@names=>3},
    :MethodTable=>{:@values=>1, :@bins=>2, :@entries=>3},
    :MetaClass=>{:@__ivars__=>0, :@method_table=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@encloser=>5, :@superclass=>6, :@instance_fields=>7, :@has_ivars=>8, :@needs_cleanup=>9, :@object_type=>10, :@attached_instance=>11},
    :Class=>{:@__ivars__=>0, :@method_table=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@encloser=>5, :@superclass=>6, :@instance_fields=>7, :@has_ivars=>8, :@needs_cleanup=>9, :@object_type=>10},
    :BlockContext=>{},
    :Object=>{:@__ivars__=>0},
    :Float=>{},
    :Array=>{:@total=>0, :@tuple=>1, :@start => 2, :@shared => 3},
    :String=>{:@bytes=>0, :@characters=>1, :@encoding=>2, :@data=>3, :@hash => 4, :@shared => 5},
    :CompiledMethod=>{:@__ivars__=>0, :@primitive => 1, :@required=>2, :@serial=>3, :@bytecodes=>4, :@name=>5, :@file=>6, :@local_count=>7, :@literals=>8, :@total_args => 9, :@splat => 10, :@exceptions=>11, :@lines=>12, :@path=>13, :@bonus => 15, :@compiled => 16, :@staticscope => 17, :@args => 18},
    :SymbolTable=>{:@__ivars__=>0,:@symbols=>1, :@strings=>2},
    :IO=>{:@__ivars__ => 0, :@descriptor => 1, :@buffer => 2, :@mode => 3 },
    :Module=>{:@__ivars__=>0, :@method_table=>1, :@method_cache=>2, :@name=>3, :@constants=>4, :@encloser=>5, :@superclass => 6},
    :MethodContext=>{},
    :Hash=>{:@__ivars__=>0, :@keys=>1, :@values=>2,:@bins=>3, :@entries=>4, :@default=>5, :@default_proc=>6},
    :BlockEnvironment=>{:@__ivars__=>0, :@home=>1, :@initial_ip=>2, :@last_ip=>3, :@post_send=>4, :@home_block => 5, :@local_count => 6, :@bonus => 7, :@method => 8},
    :Exception => {:@__ivars__ => 0, :@message => 1, :@context => 2 },
    :StaticScope => {:@__ivars__ => 0, :@module => 1, :@parent => 2 },
    :Selector => {:@name => 0, :@send_sites => 1 },
    :LookupTable => {:@values=>1, :@bins=>2, :@entries=>3}
    }

    TYPES = {
      :symtbl => :SymbolTable, :matchdata => :MatchData,
      :array => :Array, :class => :Class, :bignum => :Bignum,
      :methtbl => :MethodTable, :floatpoint => :Float, :blokctx => :BlockContext,
      :object => :Object, :io => :IO, :module => :Module,
      :cmethod => :CompiledMethod, :metaclass => :MetaClass,
      :regexpdata => :RegexpData, :regexp => :Regexp, :bytearray => :ByteArray,
      :hash => :Hash, :string => :String, :tuple => :Tuple,
      :blokenv=>:BlockEnvironment, :symbol => :Symbol,
      :methctx => :MethodContext, :exception => :Exception,
      :staticscope => :StaticScope,
      :selector => :Selector, :lookuptable => :LookupTable
    }
  end
end
