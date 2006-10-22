require 'sydparse'
require 'translation/extractors'
require 'translation/normalize'
require 'translation/local_scoping'
require 'sexp/composite_processor'
require 'translation/states'
require 'translation/generate_header'
require 'translation/to_c'

class RubiniusTranslator
  def initialize(prefix="rbs_")
    @info = TypeInfo.new(prefix)
    @typer = RsTyper.new(@info)
    @prefix = prefix
  end
  
  attr_reader :header, :body, :hints
  
  def load(code)
    s1 = SydneyParser.load_string code, true
    
    # norm = RsNormalizer.new(RsLocalState.new, true)
    
    # hinted = norm.process hinted
    hinted = s1.sexp(true)
    
    @hints = ExtractHints.new(s1.comments, true)
    @hints.process hinted
    
    # pp @hints.hints
    
    @typer.hints = @hints.hints
    syd = SydneyParser.load_string code
    norm = @typer.normalize syd.sexp
    @typer.process norm
    out = @typer.process norm
    # pp out
  end
  
  def classes
    @info.classes
  end
  
  require 'pp'
    
  def translate_to_c
    total = []
    @header = ""
    cs = classes.sort { |a,b| a[0].to_s <=> b[0].to_s }
    cs.each do |name, klass|
      # p [name, klass]
      gh = GenerateHeader.new(klass, @info)
      gh.prefix = @prefix
      @header << gh.generate
      
      if klass.meta
        codes = []
      else
        codes = [gh.generate_new_body]
      end
      
      sorted = klass.functions.sort { |a,b| a[0].to_s <=> b[0].to_s }
      
      sorted.each do |fname, func|
        next unless func.body
        raise "#{fname} is incompleted." if func.type.unknown?
        puts "#{name}##{fname}.."
        # puts "========== #{fname}"
        # pp func.body
        
        cg = TypeInfo::CodeGenerator.new(@info)
        func.arguments.each do |f_name, f_type|
          cg.add_local f_name, f_type
        end
        
        rc = RsToCProcessor.new(@info, cg, "  ")
        sbody = func.body
        # sbody.unshift :block
        unless sbody.last.first == :return
          ending = t(:return, sbody[-1])
          ending.set_type sbody[-1].type
          sbody[-1] = ending
        end
        body = rc.process(sbody)
        pre = cg.preamble "  "
        unless pre.empty?
          body = pre + "\n" + body
        end
        
        struct = cg.local_struct
        unless struct.empty?
          codes << struct
          codes << ""
        end
        
        codes << gh.wrap_function(fname, func, body)
      end
      
      total << codes.join("\n")
    end
    
    @body = total.join("\n")
  end
  
  def rehash_functions
    classes.each do |name, klass|
      klass.functions.each do |fname, func|
        next unless func.body
        ty = RsTyper.new(@info)
        ty.process_function klass, func
      end
    end
  end
  
  def load_types(kind="c")
    @info.add_c_type Type.Fixnum, "int"
    
    Dir["lib/translation/types/#{kind}/*.rb"].each do |path|
      @info.load_file path
    end
  end  
end