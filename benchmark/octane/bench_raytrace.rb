require 'benchmark'
require 'benchmark/ips'

module RayTracer
  class Color
    attr_accessor :red, :green, :blue
    def initialize(red, green, blue)
      @red = red
      @green = green
      @blue = blue
    end
    
    def self.add(c1, c2)
      result = new(0,0,0)
      result.red = c1.red + c2.red
      result.green = c1.green + c2.green
      result.blue = c1.blue + c2.blue
      result
    end
    
    def self.add_scalar(c1, s)
      result = new(0,0,0)
      result.red = c1.red + s
      result.green = c1.green + s
      result.blue = c1.blue + s
      result.limit
      result
    end
    
    def self.subtract(c1, c2)
      result = new(0,0,0)
      result.red = c1.red - c2.red
      result.green = c1.green - c2.green
      result.blue = c1.blue - c2.blue
      result
    end
    
    def self.multiply(c1, c2)
      result = new(0,0,0)
      result.red = c1.red * c2.red
      result.green = c1.green * c2.green
      result.blue = c1.blue * c2.blue
      result
    end
    
    def self.multiply_scalar(c1, f)
      result = new(0,0,0)
      result.red = c1.red * f
      result.green = c1.green * f
      result.blue = c1.blue * f
      result
    end
    
    def self.divide_factor(c1, f)
      result = new(0,0,0)
      result.red = c1.red / f
      result.green = c1.green / f
      result.blue = c1.blue / f
      result
    end
    
    def limit
      red = (self.red > 0.0) ? ( ( self.red > 1.0) ? 1.0 : self.red ) : 0.0
      green = (self.green > 0.0) ? ( ( self.green > 1.0) ? 1.0 : self.green ) : 0.0
      blue = (self.blue > 0.0) ? ( ( self.blue > 1.0) ? 1.0 : self.blue ) : 0.0
    end
    
    def distance(color)
      (red-color.red).abs + (green-color.green).abs + (blue-color.blue).abs
    end
    
    def self.blend(c1, c2, w)
      add(multiply_scalar(c1, 1-w), multiply_scalar(c2, w))
    end
    
    def brightness
      r = (red*255).floor
      g = (green*255).floor
      b = (blue*255).floor
      (r * 77 + g * 150 + b * 29) >> 8
    end
    
    def to_s
      r = (red*255).floor
      g = (green*255).floor
      b = (blue*255).floor
      "rgb(#{r},#{g},#{b})"
    end
  end
  
  class Light
    
    attr_accessor :position, :color, :intensity

    def initialize(pos, color, intensity = 10.0)
      @position = pos
      @color = color
      @intensity = intensity
    end
    
    def to_s
      "Light [#{position.x},#{position.y},#{position.z}]"
    end
    
  end
  
  class Vector
    
    attr_accessor :x, :y, :z

    def initialize(x, y, z)
      @x, @y, @z = x, y, z
    end

    def copy(vector)
      @x = vector.x
      @y = vector.y
      @z = vector.z
    end

    def normalize
      m = magnitude
      self.class.new(@x / m, @y / m, @z / m)
    end
    
    def magnitude
      Math.sqrt(@x*@x + @y*@y + @z*@z)
    end
    
    def cross(w)
      self.class.new(-@z*w.y + @y*w.z,
          @z*w.x - @x*w.z,
          -@y*w.x + @x*w.y)
    end

    def dot(w)
      @x*w.x + @y*w.y + @z*w.z
    end
    
    def self.add(v, w)
      self.new(w.x+v.x, w.y+v.y, w.z+v.z)
    end
    
    def self.subtract(v, w)
      self.new(v.x-w.x, v.y-w.y, v.z-w.z)
    end
    
    def self.multiply_vector(v, w)
      self.new(v.x*w.x, v.y*w.y, v.z*w.z)
    end
    
    def self.multiply_scalar(v, w)
      self.new(v.x*w, v.y*w, v.z*w)
    end
    
    def to_s
      "Vector [#{@x},#{@y},#{@z}]"
    end
    
  end
  
  class Ray
    attr_accessor :position, :direction

    def initialize(pos, dir)
      @position = pos
      @direction = dir
    end

    def to_s
      "Ray [#{@position},#{@direction}]"
    end
    
  end

  class Scene

    attr_accessor :camera, :shapes, :lights, :background

    def initialize
      @camera = Camera.new(Vector.new(0,0,-5),
                           Vector.new(0,0,1),
                           Vector.new(0,1,0))
      @shapes = []
      @lights = []
      @background = Background.new(Color.new(0,0,0.5), 0.2)
    end

  end

  class Material
    class BaseMaterial
      
      attr_accessor :color, :reflection, :transparency, :gloss, :has_texture

      def initialize
        @gloss = 2.0
        @transparency = 0.0
        @reflection = 0.0
        @refraction = 0.50
        @has_texture = false
      end

      def get_color(u, v)
      end
      
      def wrap_up(t)
        t = t % 2.0
        t += 2.0 if t < -1
        t -= 2.0 if t >= 1
        t
      end
      
      def to_s
        "Material [gloss=#{@gloss}, transparency=#{@transparency}, has_texture=#{@has_texture}]"
      end
      
    end

    class Solid < BaseMaterial

      def initialize(color, reflection, refraction, transparency, gloss)
        @color = color
        @reflection = reflection
        @transparency = transparency
        @gloss = gloss
        @has_texture = false
      end
      
      def get_color(u,v)
        @color
      end
      
      def to_s
        "SolidMaterial [gloss=#{@gloss}, transparency=#{@transparency}, has_texture=#{@has_texture}]"
      end

    end
    
    
    class Chessboard < BaseMaterial
      
      def initialize(color_even, color_odd, reflection , transparency, gloss, density)
        @color_even = color_even
        @color_odd = color_odd
        @reflection = reflection
        @transparency = transparency
        @gloss = gloss
        @density = density
        @has_texture = true
      end
      
      def get_color(u,v)
        t = wrap_up(u * @density) * wrap_up(v * @density)
        if t < 0.0
          @color_even
        else
          @color_odd
        end
      end

      def to_s
        "ChessMaterial [gloss=#{@gloss}, transparency=#{@transparency}, has_texture=#{@has_texture}"
      end
      
    end
  end
  
  class Shape
    

    class Sphere
      attr_accessor :radius, :position, :material

      def initialize(pos, radius, material)
        @radius = radius
        @position = pos
        @material = material
      end
      
      def intersect(ray)
        info = IntersectionInfo.new
        info.shape = self

        dst = Vector.subtract(ray.position, @position)
        
        b = dst.dot(ray.direction)
        c = dst.dot(dst) - (@radius * @radius)
        d = (b * b) -c

        if d > 0 # intersection
          info.is_hit = true
          info.distance = (-b) - Math.sqrt(d)
          info.position = Vector.add(ray.position,
                                     Vector.multiply_scalar(ray.direction, info.distance))
          info.normal = Vector.subtract(info.position, @position).normalize
          info.color = @material.get_color(0,0)
        else
          info.is_hit = false
        end
        info
      end
      
      def to_s
        "Sphere [position=#{@position}, radius=#{@radius}]"
      end
      
    end

    class Plane

      attr_accessor :material, :position

      def initialize(pos, d, material)
        @position = pos
        @d = d
        @material = material
      end
      
      def intersect(ray)
        info = IntersectionInfo.new

        vd = @position.dot(ray.direction)
        return info if (vd == 0) # no intersection

        t = -(@position.dot(ray.position) + @d) / vd
        return info if t <= 0

        info.shape = self
        info.is_hit = true
        info.position = Vector.add(ray.position,
                                   Vector.multiply_scalar(ray.direction, t))

        info.normal = @position
        info.distance = t

        if @material.has_texture
          vu = Vector.new(@position.y, @position.z, -@position.x)
          vv = vu.cross(@position)
          u = info.position.dot(vu)
          v = info.position.dot(vv)
          info.color = @material.get_color(u,v)
        else
          info.color = @material.get_color(0,0)
        end
        
        info
      end
      
      def to_s
        "Plane [#{@position}, d=#{@d}]"
      end
      
    end
    
  end
  
  class IntersectionInfo

    attr_accessor :is_hit, :hit_count, :shape, :position, :normal, :color, :distance

    def initialize
      @is_hit = false
      @hit_count = 0
      @color = Color.new(0,0,0)
    end

    def to_s
      "Intersection [#{@position}]"
    end
    
  end

  class Camera
    attr_accessor :position, :look_at, :equator, :up, :screen

    def initialize(pos, look_at, up)
      @position = pos
      @look_at = look_at
      @up = up
      @equator = look_at.normalize.cross(@up)
      @screen = Vector.add(@position, @look_at)
    end
    
    def get_ray(vx, vy)
      pos = Vector.subtract(@screen,
                            Vector.subtract(Vector.multiply_scalar(@equator, vx),
                                            Vector.multiply_scalar(@up, vy)))
      pos.y = pos.y * -1
      dir = Vector.subtract(pos, @position)
      Ray.new(pos, dir.normalize)
    end

    def to_s
      "Ray []"
    end
    
  end

  class Canvas

    def initialize(width, height)
      @width = width
      @height = height
    end
    
    def set_pixel(x, y, color)
    end
    
    def render(filename)
    end
    
  end
  
  class Background
    attr_accessor :color, :ambience
    def initialize(color, ambience)
      @color = color
      @ambience = ambience
    end

  end
  
  class Engine
    attr_accessor :canvas

    def initialize(options)
      @check_number = 0
      @options = {
        :canvas_height => 100,
        :canvas_width => 100,
        :pixel_width => 3,
        :pixel_height => 3,
        :render_diffuse => false,
        :render_shadows => false,
        :render_highlights => false,
        :render_reflections => false,
        :ray_depth => 2
      }.merge(options)
      @options[:canvas_height] /= @options[:pixel_height]
      @options[:canvas_width] /= @options[:pixel_width]
      @canvas = Canvas.new(@options[:canvas_width], @options[:canvas_height])
    end
    
    def set_pixel(x, y, color)
      @canvas.set_pixel(x, y, color)
    end

    def render_scene(scene, canvas)
      @check_number = 0
      canvas_height = @options[:canvas_height]
      canvas_width = @options[:canvas_width]
      0.upto(canvas_height) do |y|
        0.upto(canvas_width) do |x|
          yp = y * 1.0 / canvas_height * 2 - 1
          xp = x * 1.0 / canvas_width * 2 - 1
          ray = scene.camera.get_ray(xp, yp)
          color = get_pixel_color(ray, scene)
          set_pixel(x, y, color)
        end
      end
    end
    
    def get_pixel_color(ray, scene)
      info = test_intersection(ray, scene, nil)
      if info.is_hit
        color = ray_trace(info, ray, scene, 0)
        return color
      end
      return scene.background.color
    end
    

    def test_intersection(ray, scene, exclude)
      hits = 0
      best = IntersectionInfo.new
      best.distance = 2000
      
      scene.shapes.each do |shape|
        if shape != exclude
          info = shape.intersect(ray)
          if info.is_hit && info.distance >= 0 && info.distance < best.distance
            best = info
            hits += 1
          end
        end
      end
      best.hit_count = hits
      best
    end

    def get_reflection_ray(p,n,v)
      c1 = -n.dot(v)
      r1 = Vector.add(Vector.multiply_scalar(n, 2*c1),
                      v)
      Ray.new(p, r1)
    end
    
    def ray_trace(info, ray, scene, depth)
      # Calc ambient
      color = Color.multiply_scalar(info.color, scene.background.ambience)
      old_color = color
      shininess = 10**(info.shape.material.gloss+1)

      scene.lights.each do |light|
        # Calc diffuse lighting
        v = Vector.subtract(light.position, info.position).normalize
        if @options[:render_diffuse]
          l = v.dot(info.normal)
          if l > 0.0
            color = Color.add(color, 
                              Color.multiply(info.color, 
                                             Color.multiply_scalar(light.color, l)))
          end
        end
        
        # The greater the depth the more accurate the colours, but
        # this is exponentially (!) expensive
        if depth <= @options[:ray_depth]
          # calculate reflection ray
          if @options[:render_reflections] && info.shape.material.reflection > 0
            reflection_ray = get_reflection_ray(info.position, info.normal, ray.direction)
            refl = test_intersection(reflection_ray, scene, info.shape)

            if refl.is_hit && refl.distance > 0
              refl.color = ray_trace(refl, reflection_ray, scene, depth + 1)
            else
              refl.color = scene.background.color
            end
            color = Color.blend(color, refl.color, info.shape.material.reflection)
          end
          
        end
        
        # Render shadows and highlights
        shadow_info = IntersectionInfo.new
        if @options[:render_shadows]
          shadow_ray = Ray.new(info.position, v)
          shadow_info = test_intersection(shadow_ray, scene, info.shape)
          if shadow_info.is_hit && shadow_info.shape != info.shape
            va = Color.multiply_scalar(color, 0.5)
            db = 0.5 * shadow_info.shape.material.transparency**0.5
            color = Color.add_scalar(va, db)
          end
        end
        
        # Phong specular highlights
        if @options[:render_highlights] && !shadow_info.is_hit && info.shape.material.gloss > 0
          lv = Vector.subtract(info.shape.position, light.position).normalize
          e = Vector.subtract(scene.camera.position, info.shape.position).normalize
          h = Vector.subtract(e, lv).normalize
          gloss_weight = [info.normal.dot(h), 0].max ** shininess
          color = Color.add(Color.multiply_scalar(light.color, gloss_weight),
                            color)
        end
        
      end
      color.limit
      color
    end
    
  end
  

  def self.render_scene
    scene = Scene.new
    scene.camera = Camera.new(Vector.new(0,0,-15),
                              Vector.new(-0.2, 0, 5),
                              Vector.new(0, 1, 0))
    scene.background = Background.new(Color.new(0.5, 0.5, 0.5), 
                                      0.4)
    sphere = Shape::Sphere.new(Vector.new(-1.5, 1.5, 2),
                               1.5,
                               Material::Solid.new(Color.new(0,0.5,0.5),
                                                   0.3,
                                                   0.0,
                                                   0.0,
                                                   2.0))
    sphere1 = Shape::Sphere.new(Vector.new(1, 0.25, 1),
                                0.5,
                                Material::Solid.new(Color.new(0.9, 0.9, 0.9),
                                                    0.1,
                                                    0.0,
                                                    0.0,
                                                    1.5))
    plane = Shape::Plane.new(Vector.new(0.1, 0.9, -0.5).normalize,
                             1.2,
                             Material::Chessboard.new(Color.new(1,1,1),
                                                     Color.new(0,0,0),
                                                     0.2,
                                                     0.0,
                                                     1.0,
                                                     0.7))
    scene.shapes << plane << sphere << sphere1

    light = Light.new(Vector.new(5, 10, -1),
                      Color.new(0.8, 0.8, 0.8))
    light1 = Light.new(Vector.new(-3, 5, -15),
                       Color.new(0.8, 0.8, 0.8),
                       100)
    
    scene.lights << light << light1

    ray_tracer = Engine.new({ :canvas_width => 50,
                              :canvas_height => 50,
                              :pixel_width => 10,
                              :pixel_height => 10,
                              :render_diffuse => true,
                              :render_highlights => true,
                              :render_shadows => true,
                              :render_reflections => true,
                              :ray_depth => 2 })
    ray_tracer.render_scene(scene, nil)
  end
  
end

Benchmark.ips do |x|
  x.report '#render_scene' do
    RayTracer.render_scene
  end
end

