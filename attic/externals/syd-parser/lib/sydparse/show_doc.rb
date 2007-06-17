require 'rubygems'
require 'sydparse'
require 'sexp/sexp'
require 'sexp/processor'

class CountUndocumented < SexpProcessor
    def initialize
        super
        @classes = 0
        @class_names = []
        @modules = 0
        @module_names = []
        @methods = 0
        @method_names = []
        @class_list = []
        @comment = false
    end
    
    attr_reader :classes, :modules, :methods, :class_names, :module_names, :method_names
    
    def process_comment(x)
        x.clear
        @comment = true
        s()
    end
    
    def enter_class(c)
        @class_list << @current_class
        @current_class = c
    end
    
    def leave_class
        @current_class = @class_list.pop
    end
    
    def process_class(x)
        x.shift
        name = x.shift
        unless @comment
            @classes += 1
            @class_names << name
        end
        @comment = false
        x.shift
        enter_class name
        out = process(x)
        leave_class
        return out
    end
    
    def process_module(x)
        x.shift
        name = x.shift
        unless @comment
            @modules += 1
            @module_names << name
        end
        @comment = false
        enter_class name
        out = process(x.shift)
        leave_class
        return out
    end
    
    def process_sclass(x)
        x.shift
        x.shift
        @comment = false
        process(x.shift)
    end
    
    def process_defn(x)
        x.shift
        name = x.shift
        unless @comment
            @methods += 1
            @method_names << "#{@current_class}##{name}"
        end
        
        @comment = false
        x.shift
        s()
    end
    
    def process_defs(x)
        x.shift
        who = process(x.shift).first
        if who == :self
            who = @current_class
        end
        name = x.shift
        unless @comment
            @methods += 1
            @method_names << "#{who}.#{name}"
        end
        x.shift
        s()
    end
end

class ShowDoc < SexpProcessor
    def initialize
        super
        @last_comment = nil
        @current_class = nil
        @classes = []
    end
    
    def process_comment(x)
        x.shift
        @last_comment = x.dup
        x.clear
        s()
    end
    
    def enter_class(c)
        @classes << @current_class
        @current_class = c
    end
    
    def leave_class
        @current_class = @classes.pop
    end
    
    def process_class(x)
        x.shift
        name = x.shift
        puts "Class #{name}:"
        @current_class_name = name
        if @last_comment
            puts @last_comment
        else
            puts " NOT DOCUMENTED"
        end
        @last_comment = nil
        puts
        x.shift
        
        enter_class "#{name}#"
        out = process(x)
        @current_class_name = nil
        out
    end
    
    def process_module(x)
        x.shift
        name = x.shift
        puts "Module #{name}:"
        @current_class_name = name
        if @last_comment
            puts @last_comment
        else
            puts " NOT DOCUMENTED"
        end
        
        @last_comment = nil
        enter_class "#{name}#"
        out = process(x.shift)
        @current_class_name = nil
        return out
    end
    
    def process_sclass(x)
        x.shift
        who = x.shift
        if who == [:self]
            enter_class "#{@current_class_name}."
        else
            enter_class "#{who}."
        end
        out = process(x.shift)
        leave_class
        out
    end
    
    def process_defn(x)
        x.shift
        name = x.shift
        puts "method #{@current_class}#{name}:"
        if @last_comment
            puts @last_comment
        else
            puts " NOT DOCUMENTED"
        end
        @last_comment = nil
        puts
        x.shift
        s()
    end
    
    def process_const(x)
        x.shift
        s(x.shift)
    end
    
    def process_defs(x)
        x.shift
        who = process(x.shift).first
        meth = x.shift
        puts "method #{who}.#{meth}:"
        if @last_comment
            puts @last_comment
        else
            puts " NOT DOCUMENTED"
        end
        @last_comment = nil
        puts
        x.shift
        s()
    end 
end

require 'find'

classes = 0
modules = 0
methods = 0

ARGV.each do |file|

    Find.find(file) do |path|
        next if File.directory?(path)
    
        File.open(path) do |io|
            sexp = SydneyParser.unified_sexp io
            undoc = CountUndocumented.new
            undoc.process(sexp)
            classes += undoc.classes
            modules += undoc.modules
            methods += undoc.methods
            
            print (path + ":").ljust(40)
            puts " #{undoc.classes} classes, #{undoc.modules} modules, #{undoc.methods} methods"
            if undoc.classes > 0
                puts "  Classes: #{undoc.class_names.join(", ")}"
            end
            
            if undoc.modules > 0
                puts "  Modules: #{undoc.module_names.join(", ")}"
            end
            
            if undoc.methods > 0
                puts "  Methods: #{undoc.method_names.join(", ")}"
            end
        end
    end
end
puts
puts "Total: #{classes} classes, #{modules} modules, #{methods} methods"

exit

doc = ShowDoc.new

doc.process(sexp)

