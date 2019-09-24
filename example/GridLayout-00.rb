$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = true
DEBUG = true

class MyWindow < B::Window
  def quit_requested
    B.app.post_message(B::QUIT_REQUESTED)
    true
  end
end

class MyView < B::View
  def initialize(name, flag)
    super
    @points = []
  end

  def mouse_moved(point, flag, message)
    @points << point
    @points.shift if @points.length > 20
    invalidate
  end

  def draw(rect)
    return if @points.empty?
    move_pen_to @points.first
    (@points.size - 1).times do |i|
      stroke_line @points[i + 1]
    end
  end
end

class MyApp < B::Application
  def create_window
    rect = B::Rect.new(20.0, 20.0, 300.0, 300.0)
    window = MyWindow.new(rect, "Test", B::DOCUMENT_WINDOW, 0)
    group = B::GroupLayout.new(B::VERTICAL)
    window.set_layout group
    2.times do
      item = group.add_view MyView.new("test", B::WILL_DRAW)
      item.set_explicit_alignment(B::Alignment.new(B::ALIGN_HORIZONTAL_CENTER, B::ALIGN_VERTICAL_CENTER))
    end
    grid = B::GridLayout.new
    group.add_item grid
    window.set_layout group
    m = 4
    m.times do |i|
      x = i % (m / 2)
      y = i / (m / 2)
      button = B::Button.new("button(#{x},#{y})")
      p grid.add_view button, x, y
    end
    window.show
    p group.count_items
    p grid.count_items
    p group.owner
    p grid.owner
  end
  
  def ready_to_run
    set_name "This is Ruby"
    create_window
    set_pulse_rate 1000000
  end
  
  def quit_requested
    true
  end

  def pulse
    # GC.start
  end
end

# GC.disable
app = MyApp.new 'application/x-vnd.HaikuTest'
app.run
