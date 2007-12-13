require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  
  it "compiles a rescue with an empty body and no else" do
    x = [:rescue, 
          [:resbody, [:array, [:const, :O]], 
              [:lit, 13]], nil]
    
    gen x do |g|
      g.push_modifiers
      g.push :nil
      g.pop_modifiers
    end
  end
  
  it "compiles a rescue with an empty body with an else" do
    x = [:rescue, 
          [:resbody, [:array, [:const, :O]], 
              [:lit, 13]], 
          [:lit, 14]]
          
    gen x do |g|
      g.push_modifiers
      g.push 14
      g.pop_modifiers
    end
    
  end
    
  it "compiles a rescue with one condition" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:array, [:const, :String]],
             [:fixnum, 13], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      g.push_exception
      g.push_const :String
      g.send :===, 1
      body = g.new_label
      
      g.git body
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
  end
  
  it "compiles a rescue with one condition, no rescue class" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, nil,
             [:fixnum, 13], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      g.push_exception
      g.push_const :StandardError
      g.send :===, 1
      body = g.new_label
      
      g.git body
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
  end
  
  
  it "compiles a rescue with multiple contitions" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:array, [:const, :String], [:const, :Blah]],
             [:fixnum, 13], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      body = g.new_label
      
      g.push_exception
      g.push_const :String
      g.send :===, 1
      g.git body
      
      g.push_exception
      g.push_const :Blah
      g.send :===, 1
      g.git body
      
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
  end
  
  it "compiles mutiple rescues" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:array, [:const, :String]],
             [:fixnum, 13], 
             [:resbody, [:array, [:const, :Blah]],
                 [:fixnum, 14], nil]
          ]
        ]
    
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      body = g.new_label
      
      g.push_exception
      g.push_const :String
      g.send :===, 1
      g.git body
      
      next_cond = g.new_label
      
      g.goto next_cond
      body.set!
      g.push 13
      g.goto last
      
      next_cond.set!
      body2 = g.new_label
      g.push_exception
      g.push_const :Blah
      g.send :===, 1
      g.git body2
      
      g.goto rr
      body2.set!
      g.push 14
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
  end
  
  it "compiles a rescue and an else" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:array, [:const, :String]],
             [:fixnum, 13], nil
          ],
          [:fixnum, 14]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      g.push_exception
      g.push_const :String
      g.send :===, 1
      body = g.new_label
      
      g.git body
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      g.pop
      g.push 14
      
      last.set!
      g.pop_modifiers
    end
  end
  
  it "compiles a rescue with a splat" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:splat, [:vcall, :blah]],
             [:fixnum, 13], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      g.push_exception
      g.push :self
      g.send :blah, 0, true
      g.cast_array
      g.send :__rescue_match__, 1
      body = g.new_label
      
      g.git body
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
    
  end
  
  it "compiles a rescue with a condition and a splat" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:argscat, [:array, [:const, :String]], [:vcall, :blah]],
             [:fixnum, 13], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      
      body = g.new_label
      
      g.push_exception
      g.push_const :String
      g.send :===, 1
      g.git body
      
      g.push_exception
      g.push :self
      g.send :blah, 0, true
      g.cast_array
      g.send :__rescue_match__, 1
      
      g.git body
      g.goto rr
      body.set!
      g.push 13
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
    
  end
  
  it "clears the exception when there is a return in a rescue" do
    x = [:rescue, [:fixnum, 12], 
          [:resbody, [:array, [:const, :String]],
             [:return, [:nil]], nil
          ]
        ]
        
    gen x do |g|
      g.push_modifiers
      exc_start = g.new_label
      exc_handle = g.new_label
      
      fin = g.new_label
      rr = g.new_label
      last = g.new_label
      
      exc_start.set!
      g.push 12
      g.goto fin
      
      exc_handle.set!
      g.push_exception
      g.push_const :String
      g.send :===, 1
      body = g.new_label
      
      g.git body
      g.goto rr
      body.set!
      g.clear_exception
      g.push :nil
      g.sret
      g.goto last
      
      rr.set!
      
      g.push_exception
      g.raise_exc
      
      fin.set!
      
      last.set!
      g.pop_modifiers
    end
  end
  
  it "compiles a basic ensure" do
    x = [:ensure, [:fixnum, 12], [:fixnum, 13]]
    
    gen x do |g|
      ok = g.new_label
      g.exceptions do |ex|
        g.push 12
        g.goto ok
        
        ex.handle!
        g.push 13
        g.pop
        
        g.push_exception
        g.raise_exc
      end
      
      ok.set!
      g.push 13
      g.pop
    end
  end
  
  # We don't have return in an ensure working yet.
=begin
  
  it "compiles an ensure with a return in the body" do
    x = [:ensure, [:block, [:fixnum, 14], [:return, [:fixnum, 2]]], [:fixnum, 13]]
    
    gen x do |g|
      ok = g.new_label
      g.exceptions do |ex|
        g.push 14
        g.pop
        g.goto ok
        g.goto ok
        
        ex.handle do
          g.push 13
          g.pop
          
          g.push_exception
          g.raise_exc
        end
      end
      
      ok.set!
      g.push 13
      g.pop
      
      g.push 2
      g.sret
      
    end
  end
  
  it "compiles an ensure with a return in the body, inside another ensure" do
    x = [:ensure, [:ensure, [:block, [:fixnum, 14], [:return, [:fixnum, 2]]], [:fixnum, 13]], [:fixnum, 15]]
    
    gen x do |g|
      ok = g.new_label
      ok2 = g.new_label
      
      g.exceptions do |ex|
        g.exceptions do |ex2|
          g.push 14
          g.pop
          g.goto ok2
          g.goto ok2
          
          ex2.handle do
            g.push 13
            g.pop

            g.push_exception
            g.raise_exc
          end
          
        end
        
        ok2.set!
        g.push 13
        g.pop
        g.goto ok
        g.goto ok
        
        ex.handle do
          g.push 15
          g.pop
          
          g.push_exception
          g.raise_exc
        end
        
      end
      
      ok.set!
      g.push 15
      g.pop
      
      g.push 2
      g.sret
      
    end
  end
  
  it "compiles an ensure with a return in the body, inside another ensure with a return" do
    x = [:ensure, 
          [:block, [:fixnum, 14], [:return, [:fixnum, 2]]], 
          [:ensure, [:block, [:fixnum, 15], [:return, [:fixnum, 3]]], 
             [:fixnum, 16]]]
    
    gen x do |g|
      ok = g.new_label
      ok2 = g.new_label
      
      g.exceptions do |ex|
        
        g.push 14
        g.pop
        g.goto ok
        g.goto ok
                
        ex.handle do
          
          g.exceptions do |ex2|
            g.push 15
            g.pop
            g.goto ok2
            g.goto ok2
            
            ex2.handle do
              g.push 16
              g.pop
              
              g.push_exception
              g.raise_exc
            end
          end
          
          g.pop
          
          g.push_exception
          g.raise_exc
          
          ok2.set!
          g.push 16
          g.pop
          
          g.push 3
          g.sret
        end
        
      end
      
      ok.set!
      
      ok3 = g.new_label
      g.exceptions do |ex2|
        g.push 15
        g.pop
        g.goto ok3
        g.goto ok3
        
        ex2.handle do
          g.push 16
          g.pop
          
          g.push_exception
          g.raise_exc
        end
      end
      
      ok3.set!
      g.push 16
      g.pop
      
      g.push 3
      g.sret
      g.pop
      
      g.push 2
      g.sret
    end
  end
=end

end